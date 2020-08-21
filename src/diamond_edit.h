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

#ifndef DIAMOND_TEXTEDIT_H
#define DIAMOND_TEXTEDIT_H

#include <QObject>
#include <QList>
#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QTextCursor>
#include <QWidget>

#include "spellcheck.h"
#include "overlord.h"
#include "syntax.h"


class LineNumberArea;

class DiamondTextEdit : public QPlainTextEdit
{
    CS_OBJECT( DiamondTextEdit )

public:
    DiamondTextEdit( QWidget *parent, QString owner = "tab" );
    ~DiamondTextEdit();

    QString m_owner;

    QString currentFile();
    void setCurrentFile( QString fileName );

    void lineNum_PaintEvent( QPaintEvent *event );
    int lineNum_Width();

    void set_ShowLineNum( bool yesNo );

    // column mode
    void set_ColumnMode( bool yesNo );
    bool get_ColumnMode();

    // copy buffer
    QList<QString> copyBuffer() const;

    // macro
    void macroStart();
    void macroStop();
    QList<QKeyEvent *> get_MacroKeyList();

    // spell
    void set_Spell( bool value );
    QTextCursor get_Cursor();


    // syntax
    QString get_SyntaxFile();
    void set_SyntaxFile( QString fname );
    Syntax *get_SyntaxParser();
    void set_SyntaxParser( Syntax *data );
    SyntaxTypes get_SyntaxEnum();
    void set_SyntaxEnum( SyntaxTypes data );
    void forceSyntax( SyntaxTypes data );


    CS_SLOT_1( Public, void cut() )
    CS_SLOT_2( cut )

    CS_SLOT_1( Public, void copy() )
    CS_SLOT_2( copy )

    CS_SLOT_1( Public, void paste() )
    CS_SLOT_2( paste )

    CS_SLOT_1( Public, void changeSettings( Settings *settings ) )
    CS_SLOT_2( changeSettings )

    CS_SLOT_1( Public, void rewrapParagraph() )
    CS_SLOT_2( rewrapParagraph )

    CS_SIGNAL_1( Public, void setSynType( SyntaxTypes data ) )
    CS_SIGNAL_2( setSynType, data )

    CS_SIGNAL_1( Public, void queueRunSyntax( QString synFName ) )
    CS_SIGNAL_2( queueRunSyntax, synFName )

    CS_SLOT_1( Public, void runSyntax( QString synFName ) )
    CS_SLOT_2( runSyntax )

    CS_SIGNAL_1( Public, void edtUpcase() )
    CS_SIGNAL_2( edtUpcase )

    CS_SIGNAL_1( Public, void edtLowerCase() )
    CS_SIGNAL_2( edtLowerCase )

    CS_SIGNAL_1( Public, void edtSplitH() )
    CS_SIGNAL_2( edtSplitH )

    CS_SIGNAL_1( Public, void edtSplitV() )
    CS_SIGNAL_2( edtSplitV )

    CS_SIGNAL_1( Public, void edtSaveFile( QString fileName, Overlord::SaveFiles fileType ) )
    CS_SIGNAL_2( edtSaveFile, fileName, fileType )

    CS_SIGNAL_1( Public, void edtAstyle() )
    CS_SIGNAL_2( edtAstyle )

    CS_SIGNAL_1( Public, void showEdtHelp() )
    CS_SIGNAL_2( showEdtHelp )

    CS_SIGNAL_1( Public, void edtAppend() )
    CS_SIGNAL_2( edtAppend )

    CS_SIGNAL_1( Public, void edtCopy() )
    CS_SIGNAL_2( edtCut )

    CS_SIGNAL_1( Public, void edtCut() )
    CS_SIGNAL_2( edtCut )

    CS_SIGNAL_1( Public, void edtCommand() )
    CS_SIGNAL_2( edtCommand )

    CS_SIGNAL_1( Public, void edtFillRegion() )
    CS_SIGNAL_2( edtFillRegion )

    CS_SIGNAL_1( Public, void edtReplace() )
    CS_SIGNAL_2( edtReplace )

    CS_SIGNAL_1( Public, void edtBottom() )
    CS_SIGNAL_2( edtBottom )

    CS_SIGNAL_1( Public, void edtTop() )
    CS_SIGNAL_2( edtTop )

    CS_SIGNAL_1( Public, void edtPaste() )
    CS_SIGNAL_2( edtPaste )

    CS_SIGNAL_1( Public, void edtSpecialInsert() )
    CS_SIGNAL_2( edtSpecialInsert )

    CS_SIGNAL_1( Public, void edtOpenLine() )
    CS_SIGNAL_2( edtOpenLine )

    CS_SIGNAL_1( Public, void edtClearSelection() )
    CS_SIGNAL_2( edtClearSelection )

    CS_SIGNAL_1( Public, void edtSubs() )
    CS_SIGNAL_2( edtSubs )

    CS_SIGNAL_1( Public, void not_done( QString txt ) )
    CS_SIGNAL_2( not_done, txt )

    // editing API for use by MainWindow
    //
    void indentIncr( QString route );
    void indentDecr( QString route );
    void deleteLine();
    void deleteEOL();
    void deleteThroughEOL();
    void insertSymbol();
    void selectAll();
    void selectBlock();
    void selectLine();
    void selectWord();
    void caseUpper();
    void caseLower();   // TODO:: add changeCase to change UglyCamelCase to uGLYcAMELcASE
    void caseCap();
    void goLine();
    void goColumn();
    void insertDate();
    void insertTime();
    void goTop();
    void goBottom();
    void moveBar();
    void fixTab_Spaces();
    void fixSpaces_Tab();
    void deleteEOL_Spaces();
    void rewrapParaphragh();
    void setSyntax();


    // spell check
    CS_SLOT_1( Private, void spell_addUserDict() )
    CS_SLOT_2( spell_addUserDict )


    CS_SLOT_1( Public, void update_display() )
    CS_SLOT_2( update_display )



protected:
    void contextMenuEvent( QContextMenuEvent *event );
    bool event( QEvent *event );
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );
    void resizeEvent( QResizeEvent *event );
    void mousePressEvent( QMouseEvent *event );

private:
    CS_SLOT_1( Private, void spell_replaceWord() )
    CS_SLOT_2( spell_replaceWord )

    void setScreenColors();
    void changeFont();
    bool handleEdtKey( int key, int modifiers );

    Settings *m_settingsPtr;
    Themes m_lastTheme;
    QFont m_lastNormalFont;
    QFont m_lastColumnFont;

    QWidget *m_lineNumArea;
    QString m_curFile; // TODO:: need to set this each time file opened

    // tab stops
    QList<int> m_tabStops;
    void setUpTabStops();  // TODO:: We should allow a list here so COBOL card format could be supported

    // column mode
    int m_undoCount;
    void removeColumnModeSpaces();

    bool m_colHighlight;
    bool m_currentKeyGold;

    int m_startRow;
    int m_startCol;
    int m_endRow;
    int m_endCol;
    int m_lastTabSpacing;
    //  TODO:: connect this to selectionChanged() signal for each tab.
    bool m_edtSelectActive;  // do not save this - always default to no when starting


    // copy buffer
    QList<QString> m_copyBuffer;
    void addToCopyBuffer( const QString &text );

    // macro
    bool m_record;
    QList<QKeyEvent *> m_macroKeyList;

    // spell check
    QStringList spell_getMaybe( QString word );
    void createSpellCheck();
    QTextCursor m_cursor;
    bool m_isSpellCheck;
    SpellCheck *m_spellCheck;

    // syntax
    Syntax *m_syntaxParser;
    QString m_synFName;
    SyntaxTypes m_syntaxEnum;

    CS_SLOT_1( Private, void update_LineNumWidth( int newBlockCount ) )
    CS_SLOT_2( update_LineNumWidth )

};


class LineNumArea : public QWidget
{
public:
    LineNumArea( DiamondTextEdit *editor ) : QWidget( editor )
    {
        m_editor = editor;
    }

    QSize sizeHint() const
    {
        return QSize( m_editor->lineNum_Width(), 0 );
    }

protected:
    void paintEvent( QPaintEvent *event )
    {
        m_editor->lineNum_PaintEvent( event );
    }

private:
    DiamondTextEdit *m_editor;
};


#endif

