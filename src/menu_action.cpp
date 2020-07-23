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

#include "dialog_macro.h"
#include "dialog_open.h"
#include "mainwindow.h"
#include "non_gui_functions.h"
#include "diamond_edit.h"

#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QTime>

// ** file
void MainWindow::newFile()
{
    // TODO:: this is wrong. It whacks the last opened tab. Needs to open a new tab.
    //        never saw querySave();
    bool okClose = querySave();

    if ( okClose )
    {
        m_textEdit->clear();
        setCurrentTitle( "" );
    }
}

void MainWindow::open_RelatedFile()
{
    QFileInfo tmp( m_curFile );
    QString ext = tmp.suffix();

    if ( ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm" || ext == "h" )
    {
        QStringList list;

        QString tFile;
        QString baseName = tmp.canonicalPath() + "/" +  tmp.completeBaseName();

        if ( ext == "cpp" || ext == "c" || ext == "cc" || ext == "m" || ext == "mm" )
        {
            tFile = baseName + ".h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + "_p.h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

        }
        else if ( baseName.endsWith( "_p" ) && ext == "h" )
        {

            baseName.chop( 2 );

            tFile = baseName + ".cpp";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".cc";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

        }
        else if ( ext == "h" )
        {

            tFile = baseName + ".cpp";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + ".c";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }

            tFile = baseName + "_p.h";

            if ( QFile::exists( tFile ) )
            {
                list.append( tFile );
            }
        }

        //
        int cnt = list.count();

        if ( cnt == 0 )
        {
            csError( tr( "Open Related Files" ), tr( "No related files were found" ) );

        }
        else if ( cnt == 1 )
        {
            // open the one related file
            this->loadFile( list.at( 0 ), true, false );

        }
        else
        {
            // display the full list of related files
            Dialog_Open *dw = new Dialog_Open( this, list );
            int result = dw->exec();

            if ( result == QDialog::Accepted )
            {
                QString tmpF = dw->get_FileName();
                this->loadFile( tmpF, true, false );
            }

            delete dw;
        }

    }
    else
    {
        csError( tr( "Open Related Files" ), tr( "Related files only configured for .cpp, .c, .cc, .mm and .h files" ) );

    }
}

void MainWindow::openDoc( QString path )
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QStringList fileList = QFileDialog::getOpenFileNames( this, tr( "Select File" ),
                           path, tr( "All Files (*)" ), &selectedFilter, options );

    for ( const QString &fileName : fileList )
    {
        if ( ! fileName.isEmpty() )
        {
            loadFile( fileName, true, false );
        }
    }
}

bool MainWindow::close_Doc()
{
    bool okClose = querySave();

    if ( okClose )
    {

        if ( m_isSplit )
        {

            if ( m_splitFileName == m_curFile )
            {
                // close the split tab
                split_CloseButton();
            }

            rm_splitCombo( m_curFile );
        }

        openTab_Delete();

        m_textEdit->clear();
        setCurrentTitle( "" );
    }

    return okClose;
}

bool MainWindow::closeAll_Doc( bool isExit )
{
    bool allClosed = true;

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    int count = m_tabWidget->count();
    int whichTab = 0;

    // clear open tab list
    Overlord::getInstance()->openedFilesClear();
    Overlord::getInstance()->openedModifiedClear();

    for ( int k = 0; k < count; ++k )
    {

        tmp = m_tabWidget->widget( whichTab );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            m_textEdit = textEdit;
            m_curFile  = this->get_curFileName( whichTab );

            bool okClose = querySave();

            if ( okClose )
            {

                if ( isExit && ( m_curFile != "untitled.txt" ) )
                {
                    // save for the auto reload
                    Overlord::getInstance()->openedFilesAppend( m_curFile );
                    Overlord::getInstance()->openedModifiedAppend( false );
                }

                if ( m_tabWidget->count() == 1 )
                {
                    // do not remove this tab !

                    m_textEdit->clear();
                    setCurrentTitle( "" );

                }
                else
                {
                    m_tabWidget->removeTab( whichTab );

                }

            }
            else
            {
                // modified file not closed, move over one tab
                ++whichTab;

                if ( m_curFile != "untitled.txt" )
                {
                    // save for the auto reload
                    Overlord::getInstance()->openedFilesAppend( m_curFile );
                    Overlord::getInstance()->openedModifiedAppend( true );
                }

                // at least one tab is staying open
                allClosed = false;
            }
        }
    }

    if ( isExit && allClosed )
    {
        // about to close diamond

    }
    else
    {
        if ( m_isSplit )
        {
            // close the split tab
            split_CloseButton();
        }

        // update open tab list
        openTab_UpdateActions();

        m_tabWidget->setCurrentIndex( 0 );
    }

    return allClosed;
}

void MainWindow::reload()
{
    if ( m_curFile.isEmpty() )
    {
        csError( tr( "Reload" ), tr( "Unable to reload a file which was not saved." ) );

    }
    else if ( m_textEdit->document()->isModified() )
    {

        QMessageBox quest;
        quest.setWindowTitle( tr( "Reload File" ) );
        quest.setText( tr( "File: " ) + m_curFile + tr( " has been modified. Reload file?" ) );

        QPushButton *reload = quest.addButton( "Reload", QMessageBox::AcceptRole );
        quest.setStandardButtons( QMessageBox::Cancel );
        quest.setDefaultButton( QMessageBox::Cancel );

        quest.exec();

        if ( quest.clickedButton() == reload )
        {
            loadFile( m_curFile, false, false, true );
        }

    }
    else
    {
        loadFile( m_curFile, false, false, true );

    }
}

bool MainWindow::save()
{
    if ( m_curFile.isEmpty() )
    {
        return saveAs( SAVE_ONE );

    }
    else
    {
        return saveFile( m_curFile, SAVE_ONE );

    }
}

bool MainWindow::saveAs( SaveFiles saveType )
{
    bool retval = false;

    QString selectedFilter;
    QFileDialog::Options options;

    // find the current or prior path
    QString path = pathName( m_curFile );

    if ( path.isEmpty() || path == "." )
    {
        path = Overlord::getInstance()->priorPath();

        if ( path.isEmpty() )
        {
            path = QDir::homePath();
        }
    }

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Create or Select File" ),
                       path, tr( "All Files (*)" ), &selectedFilter, options );

    if ( fileName.isEmpty() )
    {
        retval = false;

    }
    else
    {
        retval = saveFile( fileName, saveType );

        if ( retval )
        {
            // update open tab list
            openTab_Delete();

            if ( m_isSplit )
            {
                rm_splitCombo( m_curFile );
            }

            setCurrentTitle( fileName );

            // update open tab list
            openTab_Add();

            if ( m_isSplit )
            {
                add_splitCombo( m_curFile );
                set_splitCombo();
            }
        }
    }

    return retval;
}

void MainWindow::saveAll()
{
    // hold for reload
    DiamondTextEdit *hold_textEdit = m_textEdit;
    int hold_index = m_tabWidget->currentIndex();

    QString fileName;

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    int count = m_tabWidget->count();

    for ( int k = 0; k < count; ++k )
    {

        tmp = m_tabWidget->widget( k );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            m_textEdit = textEdit;
            fileName   = m_tabWidget->tabWhatsThis( k );

            if ( m_textEdit->document()->isModified() )
            {

                if ( fileName == "untitled.txt" )
                {
                    m_tabWidget->setCurrentIndex( k );
                    saveAs( SAVE_ALL );

                }
                else
                {
                    saveFile( fileName, SAVE_ALL );

                }
            }
        }
    }

    // reload the current textEdit again
    m_textEdit = hold_textEdit;

    if ( m_tabWidget->currentIndex() == hold_index )
    {

        fileName = m_tabWidget->tabWhatsThis( hold_index );

        if ( fileName == "untitled.txt" )
        {
            m_curFile = "";

        }
        else
        {
            m_curFile = fileName;
            setDiamondTitle( m_curFile );
        }

    }
    else
    {
        m_tabWidget->setCurrentIndex( hold_index );

    }

    if ( ! m_textEdit->document()->isModified() )
    {
        setWindowModified( false );
    }

    if ( m_isSplit )
    {
        set_splitCombo();
    }

    setStatusBar( tr( "File(s) saved" ), 2000 );
}


// **edit
void MainWindow::mw_undo()
{
    m_textEdit->undo();
}

void MainWindow::mw_redo()
{
    m_textEdit->redo();
}

void MainWindow::mw_cut()
{
    m_textEdit->cut();
}

void MainWindow::mw_copy()
{
    m_textEdit->copy();
}

void MainWindow::mw_paste()
{
    m_textEdit->paste();
}

void MainWindow::selectAll()
{
    m_textEdit->selectAll();
}

void MainWindow::selectBlock()
{
    m_textEdit->selectLine();
}

void MainWindow::selectLine()
{
    m_textEdit->selectLine();
}

void MainWindow::selectWord()
{
    m_textEdit->selectWord();
}

void MainWindow::caseUpper()
{
    m_textEdit->caseUpper();
}

void MainWindow::caseLower()
{
    m_textEdit->caseLower();
}

void MainWindow::caseCap()
{
    m_textEdit->caseCap();
}

void MainWindow::insertDate()
{
    m_textEdit->insertDate();
}

void MainWindow::insertTime()
{
    m_textEdit->insertTime();
}

void MainWindow::insertSymbol()
{
    m_textEdit->insertSymbol();
}

void MainWindow::indentIncr( QString route )
{
    m_textEdit->indentIncr( route );
}

void MainWindow::indentDecr( QString route )
{
    m_textEdit->indentDecr( route );
}

void MainWindow::deleteLine()
{
    m_textEdit->deleteLine();
}

void MainWindow::deleteEOL()
{
    m_textEdit->deleteEOL();
}

void MainWindow::deleteThroughEOL()
{
    m_textEdit->deleteThroughEOL();
}

void MainWindow::rewrapParagraph()
{
    m_textEdit->rewrapParagraph();
}

void MainWindow::columnMode()
{
    // alters cut, copy, paste
    Overlord::getInstance()->set_isColumnMode( false );

    if ( m_ui->actionColumn_Mode->isChecked() )
    {
        // on
        Overlord::getInstance()->set_isColumnMode( true );
    }

    setStatus_ColMode();    // TODO:: see if this needs to be in DiamondTextEdit
}


// ** search methods are located in search.cpp

void MainWindow::goLine()
{
    m_textEdit->goLine();
}

void MainWindow::goColumn()
{
    m_textEdit->goColumn();
}

void MainWindow::goTop()
{
    m_textEdit->goTop();
}

void MainWindow::goBottom()
{
    m_textEdit->goBottom();
}


// **view
void MainWindow::lineHighlight()
{
    Overlord::getInstance()->set_showLineHighlight( false );

    if ( m_ui->actionLine_Highlight->isChecked() )
    {
        // on
        Overlord::getInstance()->set_showLineHighlight( true );
    }

    moveBar();
}

void MainWindow::moveBar()
{
    m_textEdit->moveBar();
}

void MainWindow::lineNumbers()
{
    if ( m_ui->actionLine_Numbers->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showLineNumbers( true );
    }
    else
    {
        // off
        Overlord::getInstance()->set_showLineNumbers( false );
    }

}

void MainWindow::wordWrap()
{
    if ( m_ui->actionWord_Wrap->isChecked() )
    {
        //on
        Overlord::getInstance()->set_isWordWrap( true );
        m_textEdit->setWordWrapMode( QTextOption::WordWrap );

    }
    else
    {
        // off
        Overlord::getInstance()->set_isWordWrap( false );
        m_textEdit->setWordWrapMode( QTextOption::NoWrap );

    }

}

// TODO:: Move this into DiamondTextEdit
void MainWindow::show_Spaces()
{
    QTextDocument *td   = m_textEdit->document();
    QTextOption textOpt = td->defaultTextOption();

    bool oldValue = Overlord::getInstance()->showSpaces();

    if ( m_ui->actionShow_Spaces->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showSpaces( true );

        if ( Overlord::getInstance()->showBreaks() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces );
        }

        td->setDefaultTextOption( textOpt );

    }
    else
    {
        // off
        Overlord::getInstance()->set_showSpaces( false );

        if ( Overlord::getInstance()->showBreaks() )
        {
            textOpt.setFlags( QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( 0 );
        }

        td->setDefaultTextOption( textOpt );
    }
}

// TODO:: move this into DiamondTextEdit
void MainWindow::show_Breaks()
{
    QTextDocument *td = m_textEdit->document();
    QTextOption textOpt = td->defaultTextOption();

    bool oldValue = Overlord::getInstance()->showBreaks();

    if ( m_ui->actionShow_Breaks->isChecked() )
    {
        //on
        Overlord::getInstance()->set_showBreaks( true );

        if ( Overlord::getInstance()->showSpaces() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces | QTextOption::ShowLineAndParagraphSeparators );

        }
        else
        {
            textOpt.setFlags( QTextOption::ShowLineAndParagraphSeparators );

        }

        td->setDefaultTextOption( textOpt );

    }
    else
    {
        // off
        Overlord::getInstance()->set_showBreaks( false );

        if ( Overlord::getInstance()->showSpaces() )
        {
            textOpt.setFlags( QTextOption::ShowTabsAndSpaces );

        }
        else
        {
            textOpt.setFlags( 0 );

        }

        td->setDefaultTextOption( textOpt );
    }
}

void MainWindow::displayHTML()
{
    try
    {
        showHtml( "display", m_curFile );

    }
    catch ( std::exception &e )
    {
        // do nothing for now
    }
}


void MainWindow::setSynType( SyntaxTypes data )
{
    m_ui->actionSyn_C->setChecked( false );
    m_ui->actionSyn_Clipper->setChecked( false );
    m_ui->actionSyn_CMake->setChecked( false );
    m_ui->actionSyn_Css->setChecked( false );
    m_ui->actionSyn_Doxy->setChecked( false );
    m_ui->actionSyn_ErrorLog->setChecked( false );
    m_ui->actionSyn_Html->setChecked( false );
    m_ui->actionSyn_Java->setChecked( false );
    m_ui->actionSyn_Javascript->setChecked( false );
    m_ui->actionSyn_Json->setChecked( false );
    m_ui->actionSyn_Makefile->setChecked( false );
    m_ui->actionSyn_Nsis->setChecked( false );
    m_ui->actionSyn_Text->setChecked( false );
    m_ui->actionSyn_Shell->setChecked( false );
    m_ui->actionSyn_Perl->setChecked( false );
    m_ui->actionSyn_PHP->setChecked( false );
    m_ui->actionSyn_Python->setChecked( false );
    m_ui->actionSyn_Xml->setChecked( false );
    m_ui->actionSyn_None->setChecked( false );

    // m_ui->actionSyn_UnUsed1->setChecked(false);
    // m_ui->actionSyn_UnUsed2->setChecked(false);

    switch ( data )
    {
        case SYN_C:
            m_ui->actionSyn_C->setChecked( true );
            break;

        case SYN_CLIPPER:
            m_ui->actionSyn_Clipper->setChecked( true );
            break;

        case SYN_CMAKE:
            m_ui->actionSyn_CMake->setChecked( true );
            break;

        case SYN_CSS:
            m_ui->actionSyn_Css->setChecked( true );
            break;

        case SYN_DOXY:
            m_ui->actionSyn_Doxy->setChecked( true );
            break;

        case SYN_ERRLOG:
            m_ui->actionSyn_ErrorLog->setChecked( true );
            break;

        case SYN_HTML:
            m_ui->actionSyn_Html->setChecked( true );
            break;

        case SYN_JAVA:
            m_ui->actionSyn_Java->setChecked( true );
            break;

        case SYN_JS:
            m_ui->actionSyn_Javascript->setChecked( true );
            break;

        case SYN_JSON:
            m_ui->actionSyn_Json->setChecked( true );
            break;

        case SYN_MAKE:
            m_ui->actionSyn_Makefile->setChecked( true );
            break;

        case SYN_NSIS:
            m_ui->actionSyn_Nsis->setChecked( true );
            break;

        case SYN_TEXT:
            m_ui->actionSyn_Text->setChecked( true );
            break;

        case SYN_SHELL:
            m_ui->actionSyn_Shell->setChecked( true );
            break;

        case SYN_PERL:
            m_ui->actionSyn_Perl->setChecked( true );
            break;

        case SYN_PHP:
            m_ui->actionSyn_PHP->setChecked( true );
            break;

        case SYN_PYTHON:
            m_ui->actionSyn_Python->setChecked( true );
            break;

        case SYN_XML:
            m_ui->actionSyn_Xml->setChecked( true );
            break;

        case SYN_NONE:
            m_ui->actionSyn_None->setChecked( true );
            break;

            /*
                  case SYN_UNUSED1:
                     m_ui->actionSyn_UnUsed1->setChecked(true);
                     break;

                  case SYN_UNUSED2:
                     m_ui->actionSyn_UnUsed2->setChecked(true);
                     break;
            */
    }
}

void MainWindow::formatUnix()
{
    showNotDone( "Document, format Unix (LF)" );
}

void MainWindow::formatWin()
{
    showNotDone( "Document, format Windows (CR LF)" );
}

void MainWindow::fixTab_Spaces()
{
    m_textEdit->fixTab_Spaces();
}

void MainWindow::fixSpaces_Tab()
{
    m_textEdit->fixSpaces_Tab();
}

void MainWindow::deleteEOL_Spaces()
{
    m_textEdit->deleteEOL_Spaces();
}


// ** tools
void MainWindow::mw_macroStart()
{
    if ( ! m_record )
    {
        m_record = true;
        m_textEdit->macroStart();

        setStatusBar( tr( "Recording macro. . ." ), 0 );
    }
}

void MainWindow::mw_macroStop()
{
    if ( m_record )
    {
        m_record = false;
        m_textEdit->macroStop();

        // save macro to global list
        m_macroList = m_textEdit->get_MacroKeyList();

        setStatusBar( tr( "Macro recorded" ), 1200 );

    }
    else
    {
        setStatusBar( tr( "No recording in progress" ), 1200 );

    }
}

void MainWindow::macroPlay()
{
    if ( m_record )
    {
        csError( "Macro Playback", "Unable to play back a macro while recording" );

    }
    else
    {
        int cnt = m_macroList.count();

        if ( cnt == 0 )
        {
            csError( "Macro Playback", "No macro to play back" );

        }
        else
        {
            QKeyEvent *event;

            for ( int k = 0; k < cnt; ++k )
            {
                event = m_macroList.at( k );

                QKeyEvent *newEvent;
                newEvent = new QKeyEvent( *event );

                QApplication::postEvent( m_textEdit, newEvent );
            }
        }
    }
}

void MainWindow::macroLoad()
{
    QStringList macroIds = Overlord::getInstance()->loadMacroIds();

    if ( macroIds.count() == 0 )
    {
        csError( "Load Macros", "No exiting macros" );
        return;
    }

    Dialog_Macro *dw = new Dialog_Macro( this, Dialog_Macro::MACRO_LOAD, macroIds,
                                         Overlord::getInstance()->macroNames() );
    int result = dw->exec();

    if ( result == QDialog::Accepted )
    {
        QString text = dw->get_Macro();
        Overlord::getInstance()->loadMacro( text );
    }

    delete dw;
}

void MainWindow::macroEditNames()
{
    QStringList macroIds = Overlord::getInstance()->loadMacroIds();

    if ( macroIds.count() == 0 )
    {
        csError( "Load Macros", "No exiting macros" );
        return;
    }

    //  TODO:: No need to pass macroNames when we pass settings reference
    //
    Dialog_Macro *dw = new Dialog_Macro( this, Dialog_Macro::MACRO_EDITNAMES, macroIds,
                                         Overlord::getInstance()->macroNames() );
    dw->exec();

    delete dw;
}

// **
void MainWindow::spellCheck()
{
    if ( m_ui->actionSpell_Check->isChecked() )
    {
        //on
        Overlord::getInstance()->set_isSpellCheck( true );

    }
    else
    {
        // off
        Overlord::getInstance()->set_isSpellCheck( false );
    }

    // run for every tab
    int count = m_tabWidget->count();

    QWidget *tmp;
    DiamondTextEdit *textEdit;

    for ( int k = 0; k < count; ++k )
    {

        tmp      = m_tabWidget->widget( k );
        textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

        if ( textEdit )
        {
            // save new values & reHighlight
            textEdit->set_Spell( Overlord::getInstance()->isSpellCheck() );
        }
    }
}
