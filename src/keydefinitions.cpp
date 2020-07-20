/**************************************************************************
*
* Copyright (c) 2020 Roland Hughes
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
#include "keydefinitions.h"
#include "qglobal.h"
#include <QKeySequence>
#include <QDebug>

KeyDefinitions::KeyDefinitions()
{
    qDebug() << "constructing KeyDefinitions()";
    setDefaultKeyValues();
}

KeyDefinitions::KeyDefinitions( const KeyDefinitions &def ) :
    m_close( def.m_close )
    , m_columnMode( def.m_columnMode )
    , m_copy( def.m_copy )
    , m_copyBuffer( def.m_copyBuffer )
    , m_cut( def.m_cut )
    , m_deleteLine( def.m_deleteLine )
    , m_deleteThroughEOL( def.m_deleteThroughEOL )
    , m_deleteToEOL( def.m_deleteToEOL )
    , m_find( def.m_find )
    , m_findNext( def.m_findNext )
    , m_findPrev( def.m_findPrev )
    , m_goBottom( def.m_goBottom )
    , m_goTop( def.m_goTop )
    , m_gotoLine( def.m_gotoLine )
    , m_indentDecrement( def.m_indentDecrement )
    , m_indentIncrement( def.m_indentIncrement )
    , m_lower( def.m_lower )
    , m_macroPlay( def.m_macroPlay )
    , m_newTab( def.m_newTab )
    , m_open( def.m_open )
    , m_paste( def.m_paste )
    , m_print( def.m_print )
    , m_printPreview( def.m_printPreview )
    , m_redo( def.m_redo )
    , m_reload( def.m_reload )
    , m_replace( def.m_replace )
    , m_save( def.m_save )
    , m_saveAs( def.m_saveAs )
    , m_selectAll( def.m_selectAll )
    , m_selectBlock( def.m_selectBlock )
    , m_selectLine( def.m_selectLine )
    , m_selectWord( def.m_selectWord )
    , m_showBreaks( def.m_showBreaks )
    , m_showSpaces( def.m_showSpaces )
    , m_spellCheck( def.m_spellCheck )
    , m_undo( def.m_undo )
    , m_upper( def.m_upper )
    , m_edtDeleteWord( def.m_edtDeleteWord )
    , m_edtGold( def.m_edtGold )
    , m_edtHelp( def.m_edtHelp )
    , m_edtFindNext( def.m_edtFindNext )
    , m_edtDeleteLine( def.m_edtDeleteLine )
    , m_edtPage( def.m_edtPage )
    , m_edtSection( def.m_edtSection )
    , m_edtAppend( def.m_edtAppend )
    , m_edtDeleteChar( def.m_edtDeleteChar )
    , m_edtAdvance( def.m_edtAdvance )
    , m_edtBackup( def.m_edtBackup )
    , m_edtCut( def.m_edtCut )
    , m_edtWord( def.m_edtWord )
    , m_edtEOL( def.m_edtEOL )
    , m_edtChar( def.m_edtChar )
    , m_edtLine( def.m_edtLine )
    , m_edtSelect( def.m_edtSelect )
    , m_edtEnter( def.m_edtEnter )
    , m_edtGotoLine( def.m_edtGotoLine )
    , m_edtEnabled( def.m_edtEnabled )
{
}

KeyDefinitions &KeyDefinitions::operator =( const KeyDefinitions &def )
{
    if ( this != &def )
    {
        m_close             = def.m_close;
        m_columnMode        = def.m_columnMode;
        m_copy              = def.m_copy;
        m_copyBuffer        = def.m_copyBuffer;
        m_cut               = def.m_cut;
        m_deleteLine        = def.m_deleteLine;
        m_deleteThroughEOL  = def.m_deleteThroughEOL;
        m_deleteToEOL       = def.m_deleteToEOL;
        m_find              = def.m_find;
        m_findNext          = def.m_findNext;
        m_findPrev          = def.m_findPrev;
        m_goBottom          = def.m_goBottom;
        m_goTop             = def.m_goTop;
        m_gotoLine          = def.m_gotoLine;
        m_indentDecrement   = def.m_indentDecrement;
        m_indentIncrement   = def.m_indentIncrement;
        m_lower             = def.m_lower;
        m_macroPlay         = def.m_macroPlay;
        m_newTab            = def.m_newTab;
        m_open              = def.m_open;
        m_paste             = def.m_paste;
        m_print             = def.m_print;
        m_printPreview      = def.m_printPreview;
        m_redo              = def.m_redo;
        m_reload            = def.m_reload;
        m_replace           = def.m_replace;
        m_save              = def.m_save;
        m_saveAs            = def.m_saveAs;
        m_selectAll         = def.m_selectAll;
        m_selectBlock       = def.m_selectBlock;
        m_selectLine        = def.m_selectLine;
        m_selectWord        = def.m_selectWord;
        m_showBreaks        = def.m_showBreaks;
        m_showSpaces        = def.m_showSpaces;
        m_spellCheck        = def.m_spellCheck;
        m_undo              = def.m_undo;
        m_upper             = def.m_upper;
        m_edtDeleteWord     = def.m_edtDeleteWord;
        m_edtGold           = def.m_edtGold;
        m_edtHelp           = def.m_edtHelp;
        m_edtFindNext       = def.m_edtFindNext;
        m_edtDeleteLine     = def.m_edtDeleteLine;
        m_edtPage           = def.m_edtPage;
        m_edtSection        = def.m_edtSection;
        m_edtAppend         = def.m_edtAppend;
        m_edtDeleteChar     = def.m_edtDeleteChar;
        m_edtAdvance        = def.m_edtAdvance;
        m_edtBackup         = def.m_edtBackup;
        m_edtCut            = def.m_edtCut;
        m_edtWord           = def.m_edtWord;
        m_edtEOL            = def.m_edtEOL;
        m_edtChar           = def.m_edtChar;
        m_edtLine           = def.m_edtLine;
        m_edtSelect         = def.m_edtSelect;
        m_edtEnter          = def.m_edtEnter;
        m_edtGotoLine       = def.m_edtGotoLine;
        m_edtEnabled        = def.m_edtEnabled;
    }

    return *this;
}

void KeyDefinitions::setDefaultKeyValues()
{
    qDebug() << "setDefaultKeyValues() called";
    // EDT keys
    //
#ifdef Q_OS_MAC
    m_edtDeleteWord = QKeySequence( Qt::MetaModifier, Qt::KeypadModifier, Qt::Key_Minus ).toString( QKeySequence::NativeText );
#else
    m_edtDeleteWord = QKeySequence( Qt::AltModifier, Qt::KeypadModifier, Qt::Key_Minus ).toString( QKeySequence::NativeText );
#endif
    m_edtEnabled    = false;
    m_edtGold       = QKeySequence( Qt::Key_NumLock ).toString( QKeySequence::NativeText );
    m_edtHelp       = QKeySequence( Qt::KeypadModifier, Qt::Key_Slash ).toString( QKeySequence::NativeText );
    m_edtFindNext   = QKeySequence( Qt::KeypadModifier, Qt::Key_Asterisk ).toString( QKeySequence::NativeText );
    m_edtDeleteLine = QKeySequence( Qt::KeypadModifier, Qt::Key_Minus ).toString( QKeySequence::NativeText );
    m_edtPage       = QKeySequence( Qt::KeypadModifier, Qt::Key_7 ).toString( QKeySequence::NativeText );
    m_edtSection    = QKeySequence( Qt::KeypadModifier, Qt::Key_8 ).toString( QKeySequence::NativeText );
    m_edtAppend     = QKeySequence( Qt::KeypadModifier, Qt::Key_9 ).toString( QKeySequence::NativeText );
    m_edtDeleteChar = QKeySequence( Qt::KeypadModifier, Qt::Key_Plus ).toString( QKeySequence::NativeText );
    m_edtAdvance    = QKeySequence( Qt::KeypadModifier, Qt::Key_4 ).toString( QKeySequence::NativeText );
    m_edtBackup     = QKeySequence( Qt::KeypadModifier, Qt::Key_5 ).toString( QKeySequence::NativeText );
    m_edtCut        = QKeySequence( Qt::KeypadModifier, Qt::Key_6 ).toString( QKeySequence::NativeText );
    m_edtWord       = QKeySequence( Qt::KeypadModifier, Qt::Key_1 ).toString( QKeySequence::NativeText );
    m_edtEOL        = QKeySequence( Qt::KeypadModifier, Qt::Key_2 ).toString( QKeySequence::NativeText );
    m_edtChar       = QKeySequence( Qt::KeypadModifier, Qt::Key_3 ).toString( QKeySequence::NativeText );
    m_edtLine       = QKeySequence( Qt::KeypadModifier, Qt::Key_0 ).toString( QKeySequence::NativeText );
    m_edtEnter      = QKeySequence( Qt::KeypadModifier, Qt::Key_Enter ).toString( QKeySequence::NativeText );
    m_edtSelect     = QKeySequence( Qt::KeypadModifier, Qt::Key_Period ).toString( QKeySequence::NativeText );
    m_edtGotoLine   = QKeySequence( Qt::Key_Equal ).toString( QKeySequence::NativeText );

    QString modifier;

#ifdef Q_OS_MAC
    modifier = "Meta+";
#else
    modifier = "Ctrl+";
#endif

    // Standard keys
    //
    m_open      = QKeySequence( QKeySequence::Open ).toString( QKeySequence::NativeText );
    m_close     = QKeySequence( QKeySequence::Close ).toString( QKeySequence::NativeText );
    m_save      = QKeySequence( QKeySequence::Save ).toString( QKeySequence::NativeText );
    m_saveAs    = QKeySequence( QKeySequence::SaveAs ).toString( QKeySequence::NativeText );
    m_print     = QKeySequence( QKeySequence::Print ).toString( QKeySequence::NativeText );
    m_undo      = QKeySequence( QKeySequence::Undo ).toString( QKeySequence::NativeText );
    m_redo      = QKeySequence( QKeySequence::Redo ).toString( QKeySequence::NativeText );
    m_cut       = QKeySequence( QKeySequence::Cut ).toString( QKeySequence::NativeText );
    m_copy      = QKeySequence( QKeySequence::Copy ).toString( QKeySequence::NativeText );
    m_paste     = QKeySequence( QKeySequence::Paste ).toString( QKeySequence::NativeText );
    m_selectAll = QKeySequence( QKeySequence::SelectAll ).toString( QKeySequence::NativeText );
    m_find      = QKeySequence( QKeySequence::Find ).toString( QKeySequence::NativeText );
    m_replace   = QKeySequence( QKeySequence::Replace ).toString( QKeySequence::NativeText );
    m_findNext  = QKeySequence( QKeySequence::FindNext ).toString( QKeySequence::NativeText );
    m_findPrev  = QKeySequence( QKeySequence::FindPrevious ).toString( QKeySequence::NativeText );
    m_goTop     = QKeySequence( QKeySequence::MoveToStartOfDocument ).toString( QKeySequence::NativeText );
    m_goBottom  = QKeySequence( QKeySequence::MoveToEndOfDocument ).toString( QKeySequence::NativeText );
    m_newTab    = QKeySequence( QKeySequence::AddTab ).toString( QKeySequence::NativeText );

    // User keys
    //
    m_selectLine        = modifier + "E";
    m_selectWord        = modifier + "D";
    m_selectBlock       = modifier + "B";
    m_upper             = modifier + "U";
    m_lower             = modifier + "L";
    m_indentIncrement   = modifier + "I";
    m_indentDecrement   = modifier + "Shift+I";
    m_deleteLine        = "Alt+D";
    m_deleteToEOL       = modifier + "K";
    m_columnMode        = "Alt+C";
    m_gotoLine          = "Alt+G";
    m_showSpaces        = "";
    m_showBreaks        = "";
    m_macroPlay         = "Alt+Return";
    m_spellCheck        = "F7";
    m_copyBuffer        = "F11";
    m_deleteThroughEOL  = "";
}

// if someone copies a json file between Windows, MAC, Linux, etc
// the key definitions will need tweaking.
//
void KeyDefinitions::platformAdjustKeyValues()
{
    m_close             = adjustKey( m_close );
    m_columnMode        = adjustKey( m_columnMode );
    m_copy              = adjustKey( m_copy );
    m_copyBuffer        = adjustKey( m_copyBuffer );
    m_cut               = adjustKey( m_cut );
    m_deleteLine        = adjustKey( m_deleteLine );
    m_deleteThroughEOL  = adjustKey( m_deleteThroughEOL );
    m_deleteToEOL       = adjustKey( m_deleteToEOL );
    m_find              = adjustKey( m_find );
    m_findNext          = adjustKey( m_findNext );
    m_findPrev          = adjustKey( m_findPrev );
    m_goBottom          = adjustKey( m_goBottom );
    m_goTop             = adjustKey( m_goTop );
    m_gotoLine          = adjustKey( m_gotoLine );
    m_indentDecrement   = adjustKey( m_indentDecrement );
    m_indentIncrement   = adjustKey( m_indentIncrement );
    m_lower             = adjustKey( m_lower );
    m_macroPlay         = adjustKey( m_macroPlay );
    m_newTab            = adjustKey( m_newTab );
    m_open              = adjustKey( m_open );
    m_paste             = adjustKey( m_paste );
    m_print             = adjustKey( m_print );
    m_printPreview      = adjustKey( m_printPreview );
    m_redo              = adjustKey( m_redo );
    m_reload            = adjustKey( m_reload );
    m_replace           = adjustKey( m_replace );
    m_save              = adjustKey( m_save );
    m_saveAs            = adjustKey( m_saveAs );
    m_selectAll         = adjustKey( m_selectAll );
    m_selectBlock       = adjustKey( m_selectBlock );
    m_selectLine        = adjustKey( m_selectLine );
    m_selectWord        = adjustKey( m_selectWord );
    m_showBreaks        = adjustKey( m_showBreaks );
    m_showSpaces        = adjustKey( m_showSpaces );
    m_spellCheck        = adjustKey( m_spellCheck );
    m_undo              = adjustKey( m_undo );
    m_upper             = adjustKey( m_upper );

    // EDT keys should never have prefix
    //
}

QString KeyDefinitions::adjustKey( QString sequence )
{
    QString modifier = sequence.left( 4 );

#ifdef Q_OS_MAC

    if ( modifier == "Ctrl" )
    {
        sequence.replace( 0,4,"Meta" );
    }

#else

    if ( modifier == "Meta" )
    {
        sequence.replace( 0,4,"Ctrl" );
    }

#endif

    return sequence;
}

