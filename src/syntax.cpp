/**************************************************************************
*
* Copyright (c) 2012-2020 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#include "spellcheck.h"
#include "syntax.h"
#include "util.h"
#include "syntaxpatterns.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QTextBoundaryFinder>
#include <QDebug>

static const QRegularExpression DEFAULT_COMMENT = QRegularExpression( "(?!E)E" );

Syntax::Syntax( QTextDocument *document, QString synFName, SpellCheck *spell )
    : QSyntaxHighlighter( document )
    , m_syntaxFile( synFName )
    , m_spellCheck( spell )
{
    m_isSpellCheck = Overlord::getInstance()->isSpellCheck();
}

Syntax::~Syntax()
{
}

void Syntax::processSyntax( Settings *settings )
{
    SyntaxPatterns *patterns = Overlord::getInstance()->getSyntaxPatterns( m_syntaxFile);
    
    //
    HighlightingRule rule;

    for ( const QString &pattern : patterns->key_Patterns )
    {
        if ( pattern.trimmed().isEmpty() )
        {
            continue;
        }

        // key
        rule.format.setFontWeight( settings->currentTheme().syntaxKey().weight() );
        rule.format.setFontItalic( settings->currentTheme().syntaxKey().italic() );
        rule.format.setForeground( settings->currentTheme().syntaxKey().color() );
        rule.pattern = QRegularExpression( pattern );

        if ( patterns->ignoreCase )
        {
            rule.pattern.setPatternOptions( QPatternOption::CaseInsensitiveOption );
        }

        highlightingRules.append( rule );
    }

    for ( const QString &pattern : patterns->class_Patterns )
    {
        if ( pattern.trimmed().isEmpty() )
        {
            continue;
        }

        // class
        rule.format.setFontWeight( settings->currentTheme().syntaxClass().weight() );
        rule.format.setFontItalic( settings->currentTheme().syntaxClass().italic() );
        rule.format.setForeground( settings->currentTheme().syntaxClass().color() );
        rule.pattern = QRegularExpression( pattern );

        if ( patterns->ignoreCase )
        {
            rule.pattern.setPatternOptions( QPatternOption::CaseInsensitiveOption );
        }

        highlightingRules.append( rule );
    }

    for ( const QString &pattern : patterns->func_Patterns )
    {
        if ( pattern.trimmed().isEmpty() )
        {
            continue;
        }

        // func
        rule.format.setFontWeight( settings->currentTheme().syntaxFunc().weight() );
        rule.format.setFontItalic( settings->currentTheme().syntaxFunc().italic() );
        rule.format.setForeground( settings->currentTheme().syntaxFunc().color() );
        rule.pattern = QRegularExpression( pattern );

        if ( patterns->ignoreCase )
        {
            rule.pattern.setPatternOptions( QPatternOption::CaseInsensitiveOption );
        }

        highlightingRules.append( rule );
    }

    for ( const QString &pattern : patterns->type_Patterns )
    {
        if ( pattern.trimmed().isEmpty() )
        {
            continue;
        }

        // types
        rule.format.setFontWeight( settings->currentTheme().syntaxType().weight() );
        rule.format.setFontItalic( settings->currentTheme().syntaxType().italic() );
        rule.format.setForeground( settings->currentTheme().syntaxType().color() );
        rule.pattern = QRegularExpression( pattern );

        if ( patterns->ignoreCase )
        {
            rule.pattern.setPatternOptions( QPatternOption::CaseInsensitiveOption );
        }

        highlightingRules.append( rule );
    }

    // quoted text - everyone
    rule.format.setFontWeight( settings->currentTheme().syntaxQuote().weight() );
    rule.format.setFontItalic( settings->currentTheme().syntaxQuote().italic() );
    rule.format.setForeground( settings->currentTheme().syntaxQuote().color() );
    rule.pattern = QRegularExpression( "\".*?\"" );
    highlightingRules.append( rule );


    rule.format.setFontWeight( settings->currentTheme().syntaxComment().weight() );
    rule.format.setFontItalic( settings->currentTheme().syntaxComment().italic() );
    rule.format.setForeground( settings->currentTheme().syntaxComment().color() );
    rule.pattern = QRegularExpression( patterns->commentSingle );
    highlightingRules.append( rule );


    m_multiLineCommentFormat.setFontWeight( settings->currentTheme().syntaxMLine().weight() );
    m_multiLineCommentFormat.setFontItalic( settings->currentTheme().syntaxMLine().italic() );
    m_multiLineCommentFormat.setForeground( settings->currentTheme().syntaxMLine().color() );
    m_commentStartExpression = QRegularExpression( patterns->commentStart );
    m_commentEndExpression   = QRegularExpression( patterns->commentEnd );

    // spell check
    m_spellCheckFormat.setUnderlineColor( QColor( Qt::red ) );

    // pending CS 1.6.1
    // m_spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    m_spellCheckFormat.setUnderlineStyle( QTextCharFormat::WaveUnderline );

    // redo the current document
    rehighlight();
}


void Syntax::set_Spell( bool value )
{
    m_isSpellCheck = value;
}

void Syntax::highlightBlock( const QString &text )
{
    QRegularExpressionMatch match;

    for ( auto &rule : highlightingRules )
    {
        match = rule.pattern.match( text );

        while ( match.hasMatch() )
        {
            int index  = match.capturedStart( 0 ) - text.begin();
            int length = match.capturedLength();

            setFormat( index, length, rule.format );

            // get new match
            match = rule.pattern.match( text, match.capturedEnd( 0 ) );
        }
    }

    // multi line comments
    setCurrentBlockState( 0 );

    int startIndex = 0;

    if ( m_commentStartExpression.pattern().isEmpty() )
    {
        m_commentStartExpression = DEFAULT_COMMENT;
    }

    if ( m_commentEndExpression.pattern().isEmpty() )
    {
        m_commentEndExpression = DEFAULT_COMMENT;
    }

    if ( previousBlockState() != 1 )
    {
        startIndex = text.indexOf( m_commentStartExpression );
    }

    while ( startIndex >= 0 )
    {
        int commentLength;
        match = m_commentEndExpression.match( text, text.begin() + startIndex );

        if ( match.hasMatch() )
        {
            int endIndex  = match.capturedStart( 0 ) - text.begin();
            commentLength = endIndex - startIndex + match.capturedLength();

        }
        else
        {
            setCurrentBlockState( 1 );
            commentLength = text.length() - startIndex;

        }

        setFormat( startIndex, commentLength, m_multiLineCommentFormat );
        startIndex = text.indexOf( m_commentStartExpression, startIndex + commentLength );
    }

    // spell check

    if ( m_spellCheck && m_isSpellCheck )
    {
        QTextBoundaryFinder wordFinder( QTextBoundaryFinder::Word, text );

        while ( wordFinder.position() < text.length() )
        {
            int wordStart  = wordFinder.position();
            int wordLength = wordFinder.toNextBoundary() - wordStart;

            QStringView word = text.midView( wordStart, wordLength ).trimmed();

            if ( ! m_spellCheck->spell( word ) )
            {
                setFormat( wordStart, wordLength, m_spellCheckFormat );
            }
        }
    }
}
