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

#include "dialog_buffer.h"
#include "dialog_getline.h"
#include "dialog_xp_getdir.h"
#include "mainwindow.h"
#include "non_gui_functions.h"

#include <QFSFileEngine>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QSysInfo>
#include <QUrl>
#include <QTime>

void MainWindow::argLoad( QList<QString> argList )
{
    int argCnt = argList.count();

    if ( !Overlord::getInstance()->flagNoAutoLoad() )
    {
        Overlord::getInstance()->openedFilesClear();
        Overlord::getInstance()->openedModifiedClear();
        Overlord::getInstance()->recentFileListClear();
    }

    for ( int k = 1; k < argCnt; k++ )
    {
        QString tempFile = argList.at( k );

        // change to forward slash
        tempFile = QDir::fromNativeSeparators( tempFile );

        // expand for full path
        QFileInfo tempPath( tempFile );
        tempFile = tempPath.canonicalFilePath();

        if ( tempFile.isEmpty() )
        {
            // do nothing

        }
        else if ( Overlord::getInstance()->openedFilesContains( tempFile ) )
        {
            // file is already open

        }
        else if ( QFile::exists( tempFile ) )
        {
            if ( loadFile( tempFile, true, true ) )
            {
                Overlord::getInstance()->openedFilesAppend( tempFile );
            }
        }
    }
}

void MainWindow::autoLoad()
{
    QString fileName;
    int count = Overlord::getInstance()->openedFilesCount();

    if ( count == 0 )
    {
        tabNew();

    }
    else
    {

        for ( int k = 0; k < count; k++ )
        {
            fileName = Overlord::getInstance()->openedFiles( k );

            // load existing files
            loadFile( fileName, true, true );
        }

        QString lastFile = Overlord::getInstance()->lastActiveFile();

        if ( !lastFile.isEmpty() )
        {
            bool found = false;

            for ( int x=0; x < m_tabWidget->count() && !found; x++ )
            {
                DiamondTextEdit *ed = dynamic_cast<DiamondTextEdit *>( m_tabWidget->widget( x ) );

                if ( ed && lastFile == m_tabWidget->tabText( x ) )
                {
                    QTextCursor cursor( ed->document()->findBlockByNumber( Overlord::getInstance()->lastActiveRow() ) );
                    cursor.movePosition( QTextCursor::StartOfLine );
                    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, Overlord::getInstance()->lastActiveColumn() - 1 );
                    ed->setTextCursor( cursor );
                    m_tabWidget->setCurrentIndex( x );
                    found = true;
                }
            }
        }
    }
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    QWidget *topWidget = m_tabWidget->currentWidget();

    if ( topWidget != nullptr )
    {
        DiamondTextEdit *ed = dynamic_cast<DiamondTextEdit *>( topWidget );

        if ( ed != nullptr )
        {
            QTextCursor c = ed->textCursor();
            Overlord::getInstance()->set_lastActiveFile( m_tabWidget->tabText( m_tabWidget->currentIndex() ) );
            Overlord::getInstance()->set_lastActiveRow( c.blockNumber() );
            Overlord::getInstance()->set_lastActiveColumn( c.positionInBlock() );
        }
    }

    bool exit = closeAll_Doc( true );

    if ( exit )
    {
        Overlord::getInstance()->set_lastPosition( pos() );
        Overlord::getInstance()->set_lastSize( size() );
        Overlord::getInstance()->close();
        event->accept();

    }
    else
    {
        event->ignore();

    }
}

void MainWindow::documentWasModified()
{
    bool isModified;

    if ( m_isSplit )
    {
        isModified = m_noSplit_textEdit->document()->isModified();
        update_splitCombo( m_curFile, isModified );

    }
    else
    {
        isModified = m_textEdit->document()->isModified();

    }

    setWindowModified( isModified );

    int index = Overlord::getInstance()->openedFilesFind( m_curFile );

    if ( index != -1 )
    {
        bool wasModified = Overlord::getInstance()->openedModified( index );

        if ( wasModified != isModified )
        {
            Overlord::getInstance()->openedModifiedReplace( index,isModified );
            openTab_UpdateOneAction( index,isModified );
        }
    }
}

QString MainWindow::get_curFileName( int whichTab )
{
    QString name = m_tabWidget->tabWhatsThis( whichTab );

    if ( name == "untitled.txt" )
    {
        name = "";
    }

    return name;
}



bool MainWindow::loadFile( QString fileName, bool addNewTab, bool isAuto, bool isReload, bool isReadOnly )
{
#if defined (Q_OS_WIN)
    // change forward to backslash
    fileName.replace( '/', '\\' );
#endif

    // part 1
    if ( addNewTab && ( m_tabWidget->count() > 0 ) )
    {
        // test if fileName is open in another tab
        QFSFileEngine engine( fileName );

        int count = m_tabWidget->count();

        QWidget *temp;
        DiamondTextEdit *textEdit;

        for ( int k = 0; k < count; ++k )
        {

            temp = m_tabWidget->widget( k );
            textEdit = dynamic_cast<DiamondTextEdit *>( temp );

            if ( textEdit )
            {
                QString t_Fname = m_tabWidget->tabWhatsThis( k );
                bool found      = false;

                if ( engine.caseSensitive() )
                {
                    found = ( fileName == t_Fname );

                }
                else
                {
                    // usually only windows
                    found = fileName.compare( t_Fname, Qt::CaseInsensitive ) == 0;

                }

                if ( found )
                {
                    // file is alredy open, select the tab
                    m_textEdit = textEdit;
                    m_tabWidget->setCurrentIndex( k );
                    return true;
                }
            }
        }
    }

    QFile file( fileName );

    if ( ! file.open( QFile::ReadOnly | QFile::Text ) )
    {

        if ( ! isAuto )
        {
            // do not show this message

            QString tmp = fileName;

            if ( tmp.isEmpty() )
            {
                tmp = "(No file name available)";
            }

            QString error = tr( "Unable to open/read file:  %1\n%2." ).formatArgs( tmp, file.errorString() );
            csError( tr( "Open/Read File" ), error );
            return false;
        }
    }

    setStatusBar( tr( "Loading File..." ), 0 );
    QApplication::setOverrideCursor( Qt::WaitCursor );

    if ( addNewTab )
    {
        tabNew();

        Overlord::getInstance()->set_priorPath( pathName( fileName ) );
    }


    /*;;;;;
     *  We have a chicken and egg problem here.
     *  Autoload happens before MainWindow is fully created.
     *  The ideal solution would be to create the syntax highlighter
     *  _before_ setting data. When you do that syntax highlighting happens auto-magically.
     *  We aren't far enough up during autoload to allow this. You spastically crash
     *  once the last line has been processed by highlightBlock() because the highlighter
     *  wants to update a tab not yet fully created for a not yet fully created MainWindow.
     *
     *  Need to find a way of decoupling menu creations from having all of the files already
     *  loaded. Once that is done the core of MainWindow could be created followed by the
     *  autoloading followed by the menu creation.
     *;;;;;
     */

    file.seek( 0 );
    QByteArray temp = file.readAll();

    QString fileData = QString::fromUtf8( temp );

    m_textEdit->setPlainText( fileData );
    //  Need to drain the swamp here. Really big files have a massive drag
    //  with all of the highlighting.
    QApplication::processEvents();

    if ( m_textEdit->m_owner == "tab" )
    {
        setCurrentTitle( fileName, false, isReload, isReadOnly );
    }

    QApplication::restoreOverrideCursor();

    if ( m_isSplit )
    {
        // update split combo box
        add_splitCombo( fileName );
    }

    if ( ! addNewTab )
    {
        // recent folders
        rfolder_Add();
    }

    if ( addNewTab && ( ! isAuto ) )
    {
        // update open tab list
        openTab_Add();

        int index = Overlord::getInstance()->openedFilesFind( fileName );

        if ( index != -1 )
        {
            Overlord::getInstance()->openedModifiedReplace( index,false );
        }
    }

    setStatusBar( tr( "File loaded" ), 1500 );

    return true;
}


bool MainWindow::querySave()
{
    if ( m_textEdit->document()->isModified() )
    {

        QString fileName = m_curFile;

        if ( m_curFile.isEmpty() )
        {
            fileName = "(Unknown Filename)";
        }

        QMessageBox quest;
        quest.setWindowTitle( tr( "Diamond Editor" ) );
        quest.setText( fileName + tr( " has been modified. Save changes?" ) );
        quest.setStandardButtons( QMessageBox::Save | QMessageBox::Discard  | QMessageBox::Cancel );
        quest.setDefaultButton( QMessageBox::Cancel );

        int retval = quest.exec();

        if ( retval == QMessageBox::Save )
        {

            if ( fileName == "untitled.txt" )
            {
                return saveAs( Overlord::SAVE_ONE );
            }
            else
            {
                return save();
            }

        }
        else if ( retval == QMessageBox::Cancel )
        {
            return false;

        }
    }

    return true;
}

bool MainWindow::saveFile( QString fileName, Overlord::SaveFiles saveType )
{
#if defined (Q_OS_WIN)
    // change forward to backslash
    fileName.replace( '/', '\\' );
#endif

    if ( QFile::exists( fileName ) )
    {
        if ( Overlord::getInstance()->makeBackups() )
        {
            backupAndTrim( fileName );
        }
    }

    QFile file( fileName );

    if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
    {
        QString tmp = fileName;

        if ( tmp.isEmpty() )
        {
            tmp = tr( "(No file name available)" );
        }

        QString error = tr( "Unable to save/write file %1:\n%2." ).formatArgs( tmp, file.errorString() );
        csError( tr( "Save/Write File" ), error );
        return false;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    if ( Overlord::getInstance()->removeSpaces() )
    {
        deleteEOL_Spaces();
    }

    if ( Overlord::getInstance()->astyleOnSave() )
    {
        QString suffix = suffixName( fileName ).toLower();

        QTextCursor lastCursorPos( m_textEdit->textCursor() );

        int row = lastCursorPos.blockNumber();
        int column = lastCursorPos.positionInBlock();

        if ( validAstyleSuffix( suffix ) )
        {
            QByteArray styledText = m_textEdit->astyleNewBuffer();

            if ( styledText.size() > 0 )
            {
                file.write( styledText );
                QString txt = QString::fromUtf8( styledText );
                m_textEdit->setPlainText( txt );

                QTextCursor cursor( m_textEdit->document()->findBlockByNumber( row ) );
                cursor.movePosition( QTextCursor::StartOfLine );
                cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, column - 1 );
                m_textEdit->setTextCursor( cursor );
                QApplication::processEvents();
            }
        }
        else
        {
            // just write the files that cannot be styled
            setStatusBar( tr( "Styling produced a zero length file" ), 2000 );
            file.write( m_textEdit->toPlainText().toUtf8() );
        }


    }
    else
    {
        file.write( m_textEdit->toPlainText().toUtf8() );
    }

    m_textEdit->document()->setModified( false );
    int index = Overlord::getInstance()->openedFilesFind( fileName );

    if ( index != -1 )
    {
        Overlord::getInstance()->openedModifiedReplace( index,false );
        openTab_UpdateOneAction( index,false );
    }


    if ( m_isSplit )
    {
        update_splitCombo( fileName, false );
    }

    if ( saveType == Overlord::SAVE_ONE )
    {
        setWindowModified( false );
        setDiamondTitle( fileName );

        setStatusBar( tr( "File saved" ), 2000 );
    }

    QApplication::restoreOverrideCursor();
    return true;
}


// title & status bar
void MainWindow::setCurrentTitle( const QString &fileName, bool tabChange, bool isReload, bool isReadOnly )
{
    QString showName;

    // adjusts the * in the title bar
    setWindowModified( m_textEdit->document()->isModified() );

    if ( fileName.isEmpty() )
    {

        m_curFile = "";
        showName  = "untitled.txt";

        setStatus_FName( showName );
        setStatus_ReadWrite( isReadOnly );

        // change the name on the tab to "untitled.txt"
        int index = m_tabWidget->currentIndex();

        m_tabWidget->setTabText( index, showName );
        m_tabWidget->setTabWhatsThis( index, showName );

        if ( Overlord::getInstance()->isComplete() )
        {
            m_textEdit->forceSyntax( SYN_TEXT );
        }

    }
    else
    {
        // loading existing file

        m_curFile = fileName;
        showName  = m_curFile;

        setStatus_FName( m_curFile );
        setStatus_ReadWrite( isReadOnly );

        // change the name on the tab to m_curFile
        int index = m_tabWidget->currentIndex();

        m_textEdit->setCurrentFile( m_curFile );
        m_tabWidget->setTabText( index, strippedName( m_curFile ) );
        m_tabWidget->setTabWhatsThis( index, m_curFile );

        if ( ! Overlord::getInstance()->recentFilesListContains( m_curFile ) )
        {
            rf_Update();
        }

        if ( ! tabChange && ! isReload )
        {
            m_textEdit->setSyntax();
        }
    }

    m_textEdit->setReadOnly( isReadOnly );

    setDiamondTitle( showName );
}

void MainWindow::setDiamondTitle( const QString title )
{
    // displays as: Diamond Editor --  File Name[*]
    QString temp = QChar( 0x02014 );
    setWindowTitle( "Diamond Editor " + temp + " " + title + " [*]" );
}

void MainWindow::setStatus_LineCol()
{
    QTextCursor cursor( m_textEdit->textCursor() );

    // emerald - adjust value when tabs are used instead of spaces
    int adjColNum = cursor.columnNumber()+1;

    m_statusLine->setText( " Line: "  + QString::number( cursor.blockNumber()+1 ) +
                           "  Col: "  + QString::number( adjColNum ) + "  " );
}

void MainWindow::setStatus_ColMode()
{
    if ( Overlord::getInstance()->isColumnMode() )
    {
        m_statusMode->setText( " Column Mode  " );

    }
    else
    {
        m_statusMode->setText( " Line Mode  " );
    }

    m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
}

void MainWindow::setStatus_FName( QString fullName )
{
    m_statusName->setText( " " + fullName + "  " );
}

void MainWindow::setStatus_ReadWrite( bool yesNo )
{
    if ( yesNo )
    {
        m_statusReadWrite->setText( "ReadOnly" );
    }
    else
    {
        m_statusReadWrite->setText( "Write" );
    }
}


// copy buffer
void MainWindow::showCopyBuffer()
{
    QList<QString> copyBuffer = m_textEdit->copyBuffer();

    Dialog_Buffer *dw = new Dialog_Buffer( copyBuffer );
    int result = dw->exec();

    if ( result == QDialog::Accepted )
    {
        int index = dw->get_Index();

        QString text = copyBuffer.at( index );
        m_textEdit->textCursor().insertText( text );
    }

    delete dw;
}


// drag & drop
void MainWindow::dragEnterEvent( QDragEnterEvent *event )
{
    if ( event->mimeData()->hasFormat( "text/uri-list" ) )
    {
        event->acceptProposedAction();
    }
    else if ( event->mimeData()->hasFormat( "text/plain" ) )
    {
        event->acceptProposedAction();

    }
}

void MainWindow::dropEvent( QDropEvent *event )
{
    const QMimeData *mimeData = event->mimeData();

    if ( mimeData->hasUrls() )
    {

        QList<QUrl> urls = mimeData->urls();

        if ( urls.isEmpty() )
        {
            return;
        }

        QString fileName = urls.first().toLocalFile();

        if ( ! fileName.isEmpty() )
        {
            loadFile( fileName, true, false );
        }

    }
    else if ( mimeData->hasText() )
    {
        QTextCursor cursor( m_textEdit->textCursor() );

        // set for undo stack
        cursor.beginEditBlock();

        cursor.insertText( mimeData->text() );

        // set for undo stack
        cursor.endEditBlock();
    }

}

// target for lambda
void MainWindow::forceSyntax( SyntaxTypes data )
{
    m_textEdit->forceSyntax( data );
}

void MainWindow::backupAndTrim( QString fileName )
{
    if ( !QFile::exists( fileName ) )
    {
        return;
    }

    QDir backupDir( Overlord::getInstance()->backupDirectory() );
    QStringList filters;
    QString wild = QString( "\'%1.b*\'" ).formatArg( fileName );
    wild.replace( ":", "!" );
    wild.replace( "\\", "!" );
    wild.replace( "/", "!" );

    QString w4 = wild;
    w4.replace( "\'", "*" );
    filters << w4;

    QStringList backupFiles = backupDir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

    // Looks weird but we are deleting the lowest backup numbers
    // which will be at the beginning of the list.
    bool okFlag = true;

    while ( ( backupFiles.size() >= Overlord::getInstance()->maxVersions() ) && okFlag )
    {
        QString fName = strippedName( backupFiles.takeFirst() );
        okFlag = backupDir.remove( fName );

        if ( !okFlag )
        {
            QString msg = QString( "Failed to remove " )+  fName;
            csError( tr( "Purging Backups" ), msg );
        }
    }

    int versionNumber = -1;

    if ( backupFiles.size() > 0 )
    {
        QString lastFile = backupFiles.last();
        QString tmp = suffixName( lastFile );
        QRegularExpression rx( "[0-9]+" );
        QRegularExpressionMatch match = rx.match( tmp );

        if ( match.hasMatch() )
        {
            versionNumber = match.captured( 0 ).toInteger<int>();
        }
    }

    versionNumber++;
    QString newSuffix = QString( ".b%1\'" ).formatArg( versionNumber, 5, 10, '0' );
    QString destName = wild;
    destName = destName.replace( ".b*\'", newSuffix );
    destName = backupDir.absoluteFilePath( destName );

    // TODO:: QFile::copy() is busted.
    //        Need to see how long before QFile::copy() will be fixed.
    //
    //        Ubuntu 18 g++7 doesn't have full -std=c++17 support.
    //        at some point g++8 moved filesystem from experimental into
    //        main library so could just #include <filesystem> and use the code below
    //
    //        Don't want to drag g++8 experimental lib around or have convoluted
    //        build to detect when building on partial C++17 support.
    //
    //        Besides, QFile::copy() should get fixed.

#if 0
    std::error_code ec;

    fs::copy( fileName.toStdString(), destName.toStdString(), ec );

    QString ecStr = QString::fromStdString( ec.message() );

#else
    QString cpyCmd;
    qDebug() << "took else path of #if 0";
#ifdef Q_OS_WIN
    cpyCmd = QString( "copy \"%1\" \"%2\"" ).formatArgs( fileName, destName );
#else
    cpyCmd = QString( "cp \"%1\" \"%2\"" ).formatArgs( fileName, destName );
#endif
    qDebug() << "About to execute: " << cpyCmd;
    system( cpyCmd.toStdString().c_str() );

#endif

    //
    qDebug() << "backup copy should exist";

    // NOTE: maxVersions from overlord is the maximum number of backup versions
    //       to keep around. If set to 12 we will dutifully keep up to 12,
    //       deleting the lowest versioned one to make room for a new one.
    //
    //       The version NUMBER will continue to increase. Your 12 backup versions
    //       could have version numbers 20000 through 20012.
    //
    //       At some point it is theoretically possible you save so many versions of one
    //       file that you bump into our magic maximum version number. This is when
    //       we start renaming files so the oldest is version zero.
    //
    if ( versionNumber >= DiamondLimits::BACKUP_VERSION_MAX )
    {
        versionNumber = 0;
        // refresh the list
        //
        backupFiles = backupDir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

        while ( backupFiles.size() > 0 )
        {
            QString firstFile = backupFiles.takeFirst();
            QString suffix = QString( ".%1" ).formatArg( suffixName( firstFile ) );
            QString newSuffix = QString( ".b%1" ).formatArg( versionNumber, 5, 10, '0' );
            QString newName = strippedName( firstFile );
            newName.replace( suffix, newSuffix );

            backupDir.rename( strippedName( firstFile ), newName );

            versionNumber++;
        }
    }
}
