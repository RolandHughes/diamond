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

#include "dialog_advfind.h"
#include "dialog_find.h"
#include "dialog_replace.h"
#include "mainwindow.h"
#include "search.h"

#include <QBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTextStream>
#include <QTableView>

// * find
void MainWindow::find()
{
    QString saveText = m_settings.findText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        m_settings.set_findText( selectedText );
    }

    Dialog_Find *dw = new Dialog_Find( this, m_settings.findText(), m_settings.findList() );
    int result = dw->exec();

    if ( result == QDialog::Accepted )
    {

        m_settings.set_findText( dw->get_findText() );
        m_settings.set_findList( dw->get_findList() );

        // add to combo list if not already there
        int index = m_settings.findListFind( m_settings.findText() );

        if ( index == -1 )
        {
            m_settings.findList().prepend( m_settings.findText() );
        }
        else
        {
            m_settings.findList().move( index,0 );
        }

        saveAndBroadcastSettings();

        // get the flags
        m_settings.set_findFlags( 0 );

        m_settings.set_findDirection( dw->get_Direction() );

        if ( ! m_settings.findDirection() )
        {
            m_settings.set_findFlagsBackward();
        }

        m_settings.set_findCase( dw->get_Case() );

        if ( m_settings.findCase() )
        {
            m_settings.set_findFlagsCaseSensitive();
        }

        m_settings.set_findWholeWords( dw->get_WholeWords() );

        if ( m_settings.findWholeWords() )
        {
            m_settings.set_findFlagsWholeWords();
        }

        if ( ! m_settings.findText().isEmpty() )
        {
            bool found = m_textEdit->find( m_settings.findText(), m_settings.findFlags() );

            if ( ! found )
            {
                // text not found, query if the user wants to search from top of file
                findNext();
            }
        }

    }
    else
    {
        m_settings.set_findText( saveText );

        bool upd_Find = dw->get_Upd_Find();

        if ( upd_Find )
        {
            m_settings.set_findList( dw->get_findList() );
            saveAndBroadcastSettings();
        }
    }

    delete dw;
}

void MainWindow::findNext()
{
    // emerald - may want to modify m_FindText when text contains html

    QTextDocument::FindFlags flags = QTextDocument::FindFlags( ~QTextDocument::FindBackward
                                     & m_settings.findFlags() );
    bool found = m_textEdit->find( m_settings.findText(), flags );

    if ( ! found )
    {
        QString msg = "Not found: " + m_settings.findText() + "\n\n";
        msg += "Search from the beginning of this document?\n";

        QMessageBox msgFindNext( this );
        msgFindNext.setWindowTitle( "Find" );
        msgFindNext.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        msgFindNext.setDefaultButton( QMessageBox::Yes );
        msgFindNext.setText( msg );

        int result = msgFindNext.exec();

        if ( result == QMessageBox::Yes )
        {
            // reset to the beginning of the document
            QTextCursor cursor( m_textEdit->textCursor() );
            cursor.movePosition( QTextCursor::Start );
            m_textEdit->setTextCursor( cursor );

            // search again
            this->findNext();
        }
    }
}

void MainWindow::findPrevious()
{
    bool found = m_textEdit->find( m_settings.findText(),
                                   QTextDocument::FindBackward | m_settings.findFlags() );

    if ( ! found )
    {
        csError( "Find", "Not found: " + m_settings.findText() );
    }
}


// * advanced find
void MainWindow::advFind()
{
    QString saveText = m_settings.advancedFindText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        m_settings.set_advancedFindText( selectedText );
    }

    m_dwAdvFind = new Dialog_AdvFind( this,
                                      m_settings.advancedFindText(),
                                      m_settings.advancedFindFileType(),
                                      m_settings.advancedFindFolder(),
                                      m_settings.advancedFSearchFolders() );

    while ( true )
    {
        int result = m_dwAdvFind->exec();

        if ( result == QDialog::Accepted )
        {

            m_settings.set_advancedFindText( m_dwAdvFind->get_findText() );
            m_settings.set_advancedFindFileType( m_dwAdvFind->get_findType() );
            m_settings.set_advancedFindFolder( m_dwAdvFind->get_findFolder() );

            // get the flags
            m_settings.set_advancedFCase( m_dwAdvFind->get_Case() );
            m_settings.set_advancedFWholeWords( m_dwAdvFind->get_WholeWords() );
            m_settings.set_advancedFSearchFolders( m_dwAdvFind->get_SearchSubFolders() );

            saveAndBroadcastSettings();

            if ( ! m_settings.advancedFindText().isEmpty() )
            {

                if ( m_settings.advancedFindFileType().isEmpty() )
                {
                    m_settings.set_advancedFindFileType( "*" );
                }

                if ( m_settings.advancedFindFolder().isEmpty() )
                {
                    m_settings.set_advancedFindFolder( QDir::currentPath() );
                }

                //
                bool aborted = false;
                QList<advFindStruct> foundList = this->advFind_getResults( aborted );

                if ( aborted )
                {
                    // do nothing

                }
                else if ( foundList.isEmpty() )
                {
                    csError( "Advanced Find", "Not found: " + m_settings.advancedFindText() );

                    // allow user to search again
                    m_dwAdvFind->showNotBusyMsg();
                    continue;

                }
                else
                {
                    this->advFind_ShowFiles( foundList );

                }
            }

        }
        else
        {
            m_settings.set_advancedFindText( saveText );

        }

        // exit while loop
        break;
    }

    delete m_dwAdvFind;
}

QList<advFindStruct> MainWindow::advFind_getResults( bool &aborted )
{
    aborted = false;

    // part 1
    QStringList searchList;
    QDir currentDir;

    if ( m_settings.advancedFSearchFolders() )
    {
        m_recursiveList.clear();

        this->findRecursive( m_settings.advancedFindFolder() );
        searchList = m_recursiveList;

    }
    else
    {
        currentDir = QDir( m_settings.advancedFindFolder() );
        searchList = currentDir.entryList( QStringList( m_settings.advancedFindFileType() ),
                                           QDir::Files | QDir::NoSymLinks );
    }

    QProgressDialog progressDialog( this );

    progressDialog.setMinimumDuration( 1500 );
    progressDialog.setMinimumWidth( 275 );
    progressDialog.setRange( 0, searchList.size() );
    progressDialog.setWindowTitle( tr( "Advanced File Search" ) );

    progressDialog.setCancelButtonText( tr( "&Cancel" ) );
    progressDialog.setCancelButtonCentered( true );

    QLabel *label = new QLabel;

    QFont font = label->font();
    font.setPointSize( 11 );

    label->setFont( font );
    label->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    progressDialog.setLabel( label );

    // part 2
    QList<advFindStruct> foundList;
    QString name;

    enum Qt::CaseSensitivity caseFlag;
    QRegularExpression regExp = QRegularExpression( "\\b" + m_settings.advancedFindText() + "\\b" );

    if ( m_settings.advancedFCase() )
    {
        caseFlag = Qt::CaseSensitive;

    }
    else
    {
        caseFlag = Qt::CaseInsensitive;
        regExp.setPatternOptions( QPatternOption::CaseInsensitiveOption );

    }

    // process each file
    for ( int k = 0; k < searchList.size(); ++k )
    {

        progressDialog.setValue( k );
        progressDialog.setLabelText( tr( "Searching file %1 of %2" ).formatArg( k ).formatArg( searchList.size() ) );
        qApp->processEvents();

        if ( progressDialog.wasCanceled() )
        {
            aborted = true;
            break;
        }

        if ( m_settings.advancedFSearchFolders() )
        {
            name = searchList[k];

        }
        else
        {
            name = currentDir.absoluteFilePath( searchList[k] );

        }

#if defined (Q_OS_WIN)
        // change forward to backslash
        name.replace( '/', '\\' );
#endif

        QFile file( name );

        if ( file.open( QIODevice::ReadOnly ) )
        {
            QString line;
            QTextStream in( &file );

            int lineNumber = 0;
            int position   = 0;

            while ( ! in.atEnd() )
            {

                line = in.readLine();
                lineNumber++;

                if ( m_settings.advancedFWholeWords() )
                {
                    position = line.indexOf( regExp );

                }
                else
                {
                    position = line.indexOf( m_settings.advancedFindText(), 0, caseFlag );

                }

                // store the results
                if ( position != -1 )
                {
                    advFindStruct temp;

                    temp.fileName   = name;
                    temp.lineNumber = lineNumber;
                    temp.text       = line.trimmed();

                    foundList.append( temp );
                }
            }
        }

        file.close();
    }

    return foundList;
}

void MainWindow::findRecursive( const QString &path, bool isFirstLoop )
{
    QDir dir( path );
    dir.setFilter( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    QFileInfoList list = dir.entryInfoList( QStringList( m_settings.advancedFindFileType() ) );
    int cnt = list.count();

    if ( isFirstLoop && cnt > 0 )
    {
        m_dwAdvFind->showBusyMsg();
    }

    if ( ! list.isEmpty() )
    {
        for ( int k = 0; k != cnt; ++k )
        {

            QString filePath = list[k].filePath();

            if ( list[k].isDir() )
            {
                // recursive
                findRecursive( filePath, false );

            }
            else
            {
                m_recursiveList.append( filePath );

            }
        }
    }
}

void MainWindow::advFind_ShowFiles( QList<advFindStruct> foundList )
{
    int index = m_splitter->indexOf( m_findWidget );

    if ( index > 0 )
    {
        m_findWidget->deleteLater();
    }

    // create the find window
    m_findWidget = new QFrame( this );
    m_findWidget->setFrameShape( QFrame::Panel );

    QTableView *view = new QTableView( this );

    m_model = new QStandardItemModel;
    m_model->setColumnCount( 3 );
    m_model->setHeaderData( 0, Qt::Horizontal, tr( "File Name" ) );
    m_model->setHeaderData( 1, Qt::Horizontal, tr( "Line #" ) );
    m_model->setHeaderData( 2, Qt::Horizontal, tr( "Text" ) );

    view->setModel( m_model );

    view->setSelectionMode( QAbstractItemView::SingleSelection );
    view->setSelectionBehavior( QAbstractItemView::SelectRows );
    view->setEditTriggers( QAbstractItemView::NoEditTriggers );

    view->setColumnWidth( 0, 300 );
    view->setColumnWidth( 1, 75 );

    view->horizontalHeader()->setStretchLastSection( true );

    // use main window font and size, add feature to allow user to change font
    // following code out for now since the font was too large

//  QFont font = view->font();
//  font.setPointSize(12);
//  view->setFont(font);

    // background color
    view->setAlternatingRowColors( true );
    view->setStyleSheet( "alternate-background-color: lightyellow" );

    int row = 0;

    for ( const auto &entry : foundList )
    {

        QStandardItem *item0  = new QStandardItem( entry.fileName );
        QStandardItem *item1  = new QStandardItem( QString::number( entry.lineNumber ) );
        QStandardItem *item2  = new QStandardItem( entry.text );

        if ( entry.fileName.endsWith( ".wpd" ) )
        {
            item2->setText( "** WordPerfect file, text format incompatible" );
        }

        m_model->insertRow( row );
        m_model->setItem( row, 0, item0 );
        m_model->setItem( row, 1, item1 );
        m_model->setItem( row, 2, item2 );

        ++row;
    }

    //
    QPushButton *closeButton = new QPushButton();
    closeButton->setText( "Close" );

    QBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget( closeButton );
    buttonLayout->addStretch();

    QBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( view );
    layout->addLayout( buttonLayout );

    m_findWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Vertical );
    m_splitter->addWidget( m_findWidget );

    // must call after addWidget
    view->resizeRowsToContents();

    connect( view,        &QTableView::clicked,  this, &MainWindow::advFind_View );
    connect( closeButton, &QPushButton::clicked, this, &MainWindow::advFind_Close );
}

void MainWindow::advFind_Close()
{
    m_findWidget->deleteLater();
}

void MainWindow::advFind_View( const QModelIndex &index )
{
    int row = index.row();

    if ( row < 0 )
    {
        return;
    }

    QString fileName = m_model->item( row,0 )->data( Qt::DisplayRole ).toString();
    int lineNumber   = m_model->item( row,1 )->data( Qt::DisplayRole ).toInt();

    if ( fileName.endsWith( ".wpd" ) )
    {
        csError( "Open File", "WordPerfect file, text format incompatible with Diamond" );
        return;
    }

    // is the file already open?
    bool open = false;
    int max   = m_tabWidget->count();

    for ( int index = 0; index < max; ++index )
    {
        QString tcurFile = this->get_curFileName( index );

        if ( tcurFile == fileName )
        {
            m_tabWidget->setCurrentIndex( index );

            open = true;
            break;
        }
    }

    //
    if ( ! open )
    {
        open = loadFile( fileName, true, false );
    }

    if ( open )
    {
        QTextCursor cursor( m_textEdit->textCursor() );
        cursor.movePosition( QTextCursor::Start );
        cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineNumber-1 );
        m_textEdit->setTextCursor( cursor );
    }
}


// * replace
void MainWindow::replace()
{
    QString saveText = m_settings.findText();

    QTextCursor cursor( m_textEdit->textCursor() );
    QString selectedText = cursor.selectedText();

    if ( ! selectedText.isEmpty() )
    {
        m_settings.set_findText( selectedText );
    }

    Dialog_Replace *dw = new Dialog_Replace( this,
            m_settings.findText(),
            m_settings.findList(),
            m_settings.replaceText(),
            m_settings.replaceList() );
    int result = dw->exec();

    if ( result >= QDialog::Accepted )
    {
        m_settings.set_findText( dw->get_findText() );
        m_settings.set_findList( dw->get_findList() );

        // add to list if not found
        int index = m_settings.findListFind( m_settings.findText() );

        if ( index == -1 )
        {
            m_settings.findListPrepend( m_settings.findText() );
        }
        else
        {
            m_settings.findListMove( index,0 );
        }

        m_settings.set_replaceText( dw->get_replaceText() );
        m_settings.set_replaceList( dw->get_replaceList() );

        // add to list if not found
        index = m_settings.replaceListFind( m_settings.replaceText() );

        if ( index == -1 )
        {
            m_settings.replaceListPrepend( m_settings.replaceText() );

            saveAndBroadcastSettings();

        }
        else
        {
            m_settings.replaceListMove( index,0 );

        }

        // get the flags
        m_settings.set_findFlags( 0 );

        m_settings.set_findCase( dw->get_Case() );

        if ( m_settings.findCase() )
        {
            m_settings.set_findFlagsCaseSensitive();
        }

        m_settings.set_findWholeWords( dw->get_WholeWords() );

        if ( m_settings.findWholeWords() )
        {
            m_settings.set_findFlagsWholeWords();
        }

        if ( ! m_settings.findText().isEmpty() && ! m_settings.replaceText().isEmpty() )
        {

            if ( result == 1 )
            {
                replaceQuery();

            }
            else if ( result == 2 )
            {
                replaceAll();

            }
        }

    }
    else
    {
        m_settings.set_findText( saveText );

        bool upd_Find    = dw->get_Upd_Find();
        bool upd_Replace = dw->get_Upd_Replace();

        if ( upd_Find && ! upd_Replace )
        {
            m_settings.set_findList( dw->get_findList() );

            saveAndBroadcastSettings();

        }
        else if ( upd_Replace )
        {
            m_settings.set_findList( dw->get_findList() );
            m_settings.set_replaceList( dw->get_replaceList() );

            saveAndBroadcastSettings();
        }
    }

    delete dw;
}

void MainWindow::replaceQuery()
{
    bool isFirst = true;
    bool found;

    // begin undo block
    QTextCursor cursor( m_textEdit->textCursor() );
    cursor.beginEditBlock();

    ReplaceReply *dw = nullptr;

    while ( true )
    {
        found = m_textEdit->find( m_settings.findText(), m_settings.findFlags() );

        if ( found )
        {

            if ( isFirst )
            {
                isFirst = false;
                dw = new ReplaceReply( this );
            }

            // aling near text
            QRect rect  = m_textEdit->cursorRect();
            QPoint point = rect.bottomLeft();
            point.rx() += 25;
            point.ry() += 10;
            dw->move( m_textEdit->mapToGlobal( point ) );

            int result = dw->exec();

            if ( result == QDialog::Rejected )
            {
                break;
            }

            int key = dw->getKey();

            if ( key == Qt::Key_unknown )
            {
                continue;

            }
            else if ( key == Qt::Key_A )
            {
                replaceAll();

            }
            else if ( key == Qt::Key_N )
            {
                continue;

            }
            else if ( key == Qt::Key_O )
            {
                cursor  = m_textEdit->textCursor();
                cursor.insertText( m_settings.replaceText() );

                break;

            }
            else if ( key == Qt::Key_S )
            {
                break;

            }
            else if ( key == Qt::Key_Y )
            {
                cursor  = m_textEdit->textCursor();
                cursor.insertText( m_settings.replaceText() );

            }

        }
        else
        {
            break;

        }
    }

    delete dw;

    cursor.clearSelection();
    m_textEdit->setTextCursor( cursor );

    // end of undo
    cursor.endEditBlock();

    if ( isFirst )
    {
        csError( "Replace", "Not found: " + m_settings.findText() );
    }
}

void MainWindow::replaceAll()
{
    bool isFirst = true;
    bool found;

    // begin undo block
    QTextCursor cursor( m_textEdit->textCursor() );
    cursor.beginEditBlock();

    while ( true )
    {
        found = m_textEdit->find( m_settings.findText(), m_settings.findFlags() );

        if ( found )
        {
            isFirst = false;

            cursor  = m_textEdit->textCursor();
            cursor.insertText( m_settings.replaceText() );

        }
        else
        {
            break;

        }
    }

    cursor.clearSelection();
    m_textEdit->setTextCursor( cursor );

    // end of undo
    cursor.endEditBlock();

    if ( isFirst )
    {
        csError( "Replace All", "Not found: " + m_settings.findText() );
    }
}


// *
ReplaceReply::ReplaceReply( MainWindow *parent )
    : QDialog( parent )
{
    QLabel *label_1= new QLabel();
    label_1->setText( "Replace?" );

    QFont font = label_1->font();
    font.setPointSize( 10 );
    label_1->setFont( font );

    QLabel *label_2= new QLabel();
    label_2->setText( "Yes / No / All / Once / Stop" );

    font = label_2->font();
    font.setPointSize( 10 );
    label_2->setFont( font );

    QBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( label_1 );
    layout->addWidget( label_2 );

    setLayout( layout );

    //
    setWindowTitle( "Search & Replace" );

    QSize size = QSize( 235, 60 );
    resize( size );
}

ReplaceReply::~ReplaceReply()
{
}

void ReplaceReply::keyPressEvent( QKeyEvent *event )
{
    m_replaceReply = event->key();

    if ( m_replaceReply == Qt::Key_Escape )
    {
        this->done( 0 );

    }
    else
    {
        this->done( 1 );

    }
}

int ReplaceReply::getKey()
{
    return m_replaceReply;
}
