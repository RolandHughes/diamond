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

#include "dialog_options.h"
#include "util.h"
#include "overlord.h"

#include <QFileDialog>
#include <QKeySequence>
#include <QLineEdit>
#include <QString>
#include <QToolButton>

#include <qglobal.h>

Dialog_Options::Dialog_Options( QWidget *parent )
    : QDialog( parent ), m_ui( new Ui::Dialog_Options )
{
    m_options  = Overlord::getInstance()->pullLocalCopyOfOptions();

    m_ui->setupUi( this );
    this->setWindowIcon( QIcon( "://resources/diamond.png" ) );
    m_ui->keypad_label->setPixmap( QPixmap( "://resources/EDT-Keypad.png" ) );

    initData();

    connect( m_ui->dictMain_TB, &QToolButton::clicked, this, &Dialog_Options::pick_Main );
    connect( m_ui->dictUser_TB, &QToolButton::clicked, this, &Dialog_Options::pick_User );
    connect( m_ui->syntax_TB,   &QToolButton::clicked, this, &Dialog_Options::pick_Syntax );

    connect( m_ui->reset_PB,    &QPushButton::clicked, this, &Dialog_Options::reset_StandardKey );
    connect( m_ui->save_PB,     &QPushButton::clicked, this, &Dialog_Options::save );
    connect( m_ui->cancel_PB,   &QPushButton::clicked, this, &Dialog_Options::cancel );

    m_ui->tabWidget->setCurrentIndex(0);
}

Dialog_Options::~Dialog_Options()
{
    delete m_ui;
}

void Dialog_Options::initData()
{
    QStringList list;
    int index;

    // ** tab one

    // 1
    list << "MM/dd/yyyy" << "dd/MM/yyyy" << "MMM dd, yyyy"
         << "MMMM dd, yyyy" << "yyyyMMdd";

    m_ui->dateFormat_CB->addItems( list );
    m_ui->dateFormat_CB->setEditable( false );

    index = m_ui->dateFormat_CB->findText( m_options.formatDate() );
    m_ui->dateFormat_CB->setCurrentIndex( index );

    // 2
    list.clear();
    list << "hh:mm" << "hh:mm:ss" << "h:m:s ap" << "h:mm ap";

    m_ui->timeFormat_CB->addItems( list );
    m_ui->timeFormat_CB->setEditable( false );

    index = m_ui->timeFormat_CB->findText( m_options.formatTime() );
    m_ui->timeFormat_CB->setCurrentIndex( index );

    // 3
    list.clear();
    list << "3" << "4" << "8";
    m_ui->tabSpacing_CB->addItems( list );
    m_ui->tabSpacing_CB->setEditable( false );

    index = m_ui->tabSpacing_CB->findText( QString::number( m_options.tabSpacing() ) );
    m_ui->tabSpacing_CB->setCurrentIndex( index );

    m_ui->useSpaces_CKB->setChecked( m_options.useSpaces() );

    m_ui->removeSpace_CKB->setChecked( m_options.removeSpaces() );

    m_ui->autoLoad_CKB->setChecked( m_options.autoLoad() );

    m_ui->enable_EDT_CB->setChecked( m_options.keys().edtEnabled() );

    m_ui->dictMain->setText( m_options.mainDictionary() );
    m_ui->dictMain->setCursorPosition( 0 );

    m_ui->dictUser->setText( m_options.userDictionary() );
    m_ui->dictUser->setCursorPosition( 0 );

    m_ui->syntax->setText( m_options.syntaxPath() );
    m_ui->syntax->setCursorPosition( 0 );


    // ** tab two
    m_ui->key_open->setText( m_options.keys().open() );
    m_ui->key_close->setText( m_options.keys().close() );
    m_ui->key_save->setText( m_options.keys().save() );
    m_ui->key_saveAs->setText( m_options.keys().saveAs() );
    m_ui->key_print->setText( m_options.keys().print() );
    m_ui->key_undo->setText( m_options.keys().undo() );
    m_ui->key_redo->setText( m_options.keys().redo() );
    m_ui->key_cut->setText( m_options.keys().cut() );
    m_ui->key_copy->setText( m_options.keys().copy() );
    m_ui->key_paste->setText( m_options.keys().paste() );
    m_ui->key_selectAll->setText( m_options.keys().selectAll() );
    m_ui->key_find->setText( m_options.keys().find() );
    m_ui->key_replace->setText( m_options.keys().replace() );
    m_ui->key_findNext->setText( m_options.keys().findNext() );
    m_ui->key_findPrev->setText( m_options.keys().findPrev() );
    m_ui->key_goTop->setText( m_options.keys().goTop() );
    m_ui->key_goBottom->setText( m_options.keys().goBottom() );
    m_ui->key_newTab->setText( m_options.keys().newTab() );

    // ** tab three
    m_ui->key_printPreview->setText( m_options.keys().printPreview() );
    m_ui->key_reload->setText( m_options.keys().reload() );
    m_ui->key_selectLine->setText( m_options.keys().selectLine() );
    m_ui->key_selectWord->setText( m_options.keys().selectWord() );
    m_ui->key_selectBlock->setText( m_options.keys().selectBlock() );
    m_ui->key_upper->setText( m_options.keys().upper() );
    m_ui->key_lower->setText( m_options.keys().lower() );
    m_ui->key_indentIncrement->setText( m_options.keys().indentIncrement() );
    m_ui->key_indentDecrement->setText( m_options.keys().indentDecrement() );
    m_ui->key_deleteLine->setText( m_options.keys().deleteLine() );
    m_ui->key_deleteToEOL->setText( m_options.keys().deleteToEOL() );
    m_ui->key_deleteThroughEOL->setText( m_options.keys().deleteThroughEOL() );
    m_ui->key_columnMode->setText( m_options.keys().columnMode() );
    m_ui->key_goLine->setText( m_options.keys().gotoLine() );
    m_ui->key_show_Spaces->setText( m_options.keys().showSpaces() );
    m_ui->key_show_Breaks->setText( m_options.keys().showBreaks() );
    m_ui->key_macroPlay->setText( m_options.keys().macroPlay() );
    m_ui->key_spellCheck->setText( m_options.keys().spellCheck() );
    m_ui->key_copyBuffer->setText( m_options.keys().copyBuffer() );

    // ** tab four
    m_ui->key_EDT_Gold->setText( m_options.keys().edtGold() );
    m_ui->key_EDT_Help->setText( m_options.keys().edtHelp() );
    m_ui->key_EDT_FindNext->setText( m_options.keys().edtFindNext() );
    m_ui->key_EDT_DeleteLine->setText( m_options.keys().edtDeleteLine() );
    m_ui->key_EDT_DeleteWord->setText( m_options.keys().edtDeleteWord() );
    m_ui->key_EDT_Page->setText( m_options.keys().edtPage() );
    m_ui->key_EDT_Section->setText( m_options.keys().edtSection() );
    m_ui->key_EDT_Append->setText( m_options.keys().edtAppend() );
    m_ui->key_EDT_DeleteChar->setText( m_options.keys().edtDeleteChar() );
    m_ui->key_EDT_Advance->setText( m_options.keys().edtDirectionAdvance() );
    m_ui->key_EDT_Backup->setText( m_options.keys().edtDirectionBack() );
    m_ui->key_EDT_Cut->setText( m_options.keys().edtCut() );
    m_ui->key_EDT_Word->setText( m_options.keys().edtWord() );
    m_ui->key_EDT_EOL->setText( m_options.keys().edtEOL() );
    m_ui->key_EDT_Char->setText( m_options.keys().edtChar() );
    m_ui->key_EDT_Line->setText( m_options.keys().edtLine() );
    m_ui->key_EDT_Select->setText( m_options.keys().edtSelect() );
    m_ui->key_EDT_Enter->setText( m_options.keys().edtEnter() );
    m_ui->key_EDT_GotoLine->setText( m_options.keys().edtGotoLine() );
}

void Dialog_Options::save()
{
    // ** tab 1
    m_options.set_formatDate( m_ui->dateFormat_CB->currentText() );
    m_options.set_formatTime( m_ui->timeFormat_CB->currentText() );
    m_options.set_mainDictionary( m_ui->dictMain->text() );
    m_options.set_userDictionary( m_ui->dictUser->text() );
    m_options.set_syntaxPath( m_ui->syntax->text() );

    QString value = m_ui->tabSpacing_CB->currentText();
    m_options.set_tabSpacing( value.toInteger<int>() );

    m_options.set_useSpaces( m_ui->useSpaces_CKB->isChecked() );
    m_options.set_removeSpaces( m_ui->removeSpace_CKB->isChecked() );
    m_options.set_autoLoad( m_ui->autoLoad_CKB->isChecked() );

    // ** tab 2
    m_options.keys().set_open( m_ui->key_open->text() );
    m_options.keys().set_close( m_ui->key_close->text() );
    m_options.keys().set_save( m_ui->key_save->text() );
    m_options.keys().set_saveAs( m_ui->key_saveAs->text() );
    m_options.keys().set_print( m_ui->key_print->text() );
    m_options.keys().set_undo( m_ui->key_undo->text() );
    m_options.keys().set_redo( m_ui->key_redo->text() );
    m_options.keys().set_cut( m_ui->key_cut->text() );
    m_options.keys().set_copy( m_ui->key_copy->text() );
    m_options.keys().set_paste( m_ui->key_paste->text() );
    m_options.keys().set_selectAll( m_ui->key_selectAll->text() );
    m_options.keys().set_find( m_ui->key_find->text() );
    m_options.keys().set_replace( m_ui->key_replace->text() );
    m_options.keys().set_findNext( m_ui->key_findNext->text() );
    m_options.keys().set_findPrev( m_ui->key_findPrev->text() );
    m_options.keys().set_goTop( m_ui->key_goTop->text() );
    m_options.keys().set_goBottom( m_ui->key_goBottom->text() );
    m_options.keys().set_newTab( m_ui->key_newTab->text() );
    // ** tab 3
    m_options.keys().set_printPreview( m_ui->key_printPreview->text() );
    m_options.keys().set_reload( m_ui->key_reload->text() );
    m_options.keys().set_selectLine( m_ui->key_selectLine->text() );
    m_options.keys().set_selectWord( m_ui->key_selectWord->text() );
    m_options.keys().set_selectBlock( m_ui->key_selectBlock->text() );
    m_options.keys().set_upper( m_ui->key_upper->text() );
    m_options.keys().set_lower( m_ui->key_lower->text() );
    m_options.keys().set_indentIncrement( m_ui->key_indentIncrement->text() );
    m_options.keys().set_indentDecrement( m_ui->key_indentDecrement->text() );
    m_options.keys().set_deleteLine( m_ui->key_deleteLine->text() );
    m_options.keys().set_deleteToEOL( m_ui->key_deleteToEOL->text() );
    m_options.keys().set_deleteThroughEOL( m_ui->key_deleteThroughEOL->text() );
    m_options.keys().set_columnMode( m_ui->key_columnMode->text() );
    m_options.keys().set_gotoLine( m_ui->key_goLine->text() );
    m_options.keys().set_showSpaces( m_ui->key_show_Spaces->text() );
    m_options.keys().set_showBreaks( m_ui->key_show_Breaks->text() );
    m_options.keys().set_macroPlay( m_ui->key_macroPlay->text() );
    m_options.keys().set_spellCheck( m_ui->key_spellCheck->text() );
    m_options.keys().set_copyBuffer( m_ui->key_copyBuffer->text() );

    //** tab 4
    m_options.keys().set_edtDeleteWord( m_ui->key_EDT_DeleteWord->text() );
    m_options.keys().set_edtGold( m_ui->key_EDT_Gold->text() );
    m_options.keys().set_edtHelp( m_ui->key_EDT_Help->text() );
    m_options.keys().set_edtFindNext( m_ui->key_EDT_FindNext->text() );
    m_options.keys().set_edtDeleteLine( m_ui->key_EDT_DeleteLine->text() );
    m_options.keys().set_edtPage( m_ui->key_EDT_Page->text() );
    m_options.keys().set_edtSection( m_ui->key_EDT_Section->text() );
    m_options.keys().set_edtAppend( m_ui->key_EDT_Append->text() );
    m_options.keys().set_edtDeleteChar( m_ui->key_EDT_DeleteChar->text() );
    m_options.keys().set_edtDirectionAdvance( m_ui->key_EDT_Advance->text() );
    m_options.keys().set_edtDirectionBack( m_ui->key_EDT_Backup->text() );
    m_options.keys().set_edtCut( m_ui->key_EDT_Cut->text() );
    m_options.keys().set_edtWord( m_ui->key_EDT_Word->text() );
    m_options.keys().set_edtEOL( m_ui->key_EDT_EOL->text() );
    m_options.keys().set_edtChar( m_ui->key_EDT_Char->text() );
    m_options.keys().set_edtLine( m_ui->key_EDT_Line->text() );
    m_options.keys().set_edtSelect( m_ui->key_EDT_Select->text() );
    m_options.keys().set_edtEnter( m_ui->key_EDT_Enter->text() );
    m_options.keys().set_edtGotoLine( m_ui->key_EDT_GotoLine->text() );
    m_options.keys().set_edtEnabled( m_ui->enable_EDT_CB->isChecked() );

    Overlord::getInstance()->updateOptionsFromLocalCopy( m_options );

    this->done( QDialog::Accepted );
}

void Dialog_Options::cancel()
{
    this->done( QDialog::Rejected );
}

void Dialog_Options::pick_Main()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getOpenFileName( this, tr( "Select Main Dictionary" ),
                       m_ui->dictMain->text(), tr( "Dictionary File (*.dic)" ), &selectedFilter, options );

    if ( ! fileName.isEmpty() )
    {
        m_ui->dictMain->setText( fileName );
    }
}

void Dialog_Options::pick_User()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getOpenFileName( this, tr( "Select User Dictionary" ),
                       m_ui->dictUser->text(), tr( "User Dictionary File (*.txt)" ), &selectedFilter, options );

    if ( ! fileName.isEmpty() )
    {
        m_ui->dictUser->setText( fileName );
    }
}

void Dialog_Options::pick_Syntax()
{
    QString msg  = tr( "Select Diamond Syntax Folder" );
    QString path = m_ui->syntax->text();

    path = get_DirPath( this, msg, path );

    if ( ! path.isEmpty() )
    {
        m_ui->syntax->setText( path );
    }
}

void Dialog_Options::reset_StandardKey()
{
    m_options.keys().setDefaultKeyValues();
    initData();
}

