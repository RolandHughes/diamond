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

#include "mainwindow.h"
#include "non_gui_functions.h"

#include <QBoxLayout>
#include <QPalette>
#include <QPushButton>

void MainWindow::split_Vertical()
{
    // only allow one for now
    if ( m_isSplit )
    {
        deleteOldSplit();
    }

    m_split_textEdit = new DiamondTextEdit( this, "split" );
    m_splitFileName  = m_curFile;

    // sync documents
    m_split_textEdit->setDocument( m_textEdit->document() );

    if ( m_split_textEdit->get_ColumnMode() )
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
    }
    else
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
    }

    QPalette temp = m_split_textEdit->palette();
    temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme().colorText() );
    temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme().colorBack() );
    m_split_textEdit->setPalette( temp );

    // position on same line
    QTextCursor cursor( m_textEdit->textCursor() );
    m_split_textEdit->setTextCursor( cursor );

    m_isSplit  = true;
    m_textEdit = m_split_textEdit;

    //
    m_splitWidget = new QFrame( this );
    m_splitWidget->setFrameShape( QFrame::Panel );
    m_splitWidget->setWhatsThis( "split_widget" );

    //
    m_splitName_CB = new QComboBox();
    m_splitName_CB->setMinimumWidth( 175 );

    QFont font2 = m_splitName_CB->font();
    font2.setPointSize( 11 );
    m_splitName_CB->setFont( font2 );

    for ( int k = 0; k < Overlord::getInstance()->openedFilesCount(); ++k )
    {

        QString fullName = Overlord::getInstance()->openedFiles( k );
        add_splitCombo( fullName );

        if ( Overlord::getInstance()->openedModified( k ) )
        {
            update_splitCombo( fullName, true );
        }
    }

    //
    m_splitClose_PB = new QPushButton();
    m_splitClose_PB->setText( "Close" );

    QBoxLayout *topbar_Layout = new QHBoxLayout();
    topbar_Layout->addWidget( m_splitName_CB, 1 );
    topbar_Layout->addSpacing( 25 );
    topbar_Layout->addWidget( m_splitClose_PB );
    topbar_Layout->addStretch( 2 );

    //
    QBoxLayout *layout = new QVBoxLayout();
    layout->addLayout( topbar_Layout );
    layout->addWidget( m_split_textEdit );

    m_splitWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Horizontal );      // difference Here
    m_splitter->addWidget( m_splitWidget );

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );
    m_splitName_CB->setCurrentIndex( splitIndex );

    moveBar();

    connect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this, &MainWindow::split_NameChanged );

    connect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    connect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_split_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );
    connect( Overlord::getInstance(), &Overlord::settingsChanged,
             m_split_textEdit, &DiamondTextEdit::changeSettings );

}

void MainWindow::split_Horizontal()
{
    // only allow one for now
    if ( m_isSplit )
    {
        deleteOldSplit();
    }

    m_split_textEdit = new DiamondTextEdit( this, "split" );
    m_splitFileName  = m_curFile;

    // sync documents
    m_split_textEdit->setDocument( m_textEdit->document() );

    if ( m_split_textEdit->get_ColumnMode() )
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
    }
    else
    {
        m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
    }

    QPalette temp = m_split_textEdit->palette();
    temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme().colorText() );
    temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme().colorBack() );
    m_split_textEdit->setPalette( temp );

    // position on same line
    QTextCursor cursor( m_textEdit->textCursor() );
    m_split_textEdit->setTextCursor( cursor );

    m_isSplit  = true;
    m_textEdit = m_split_textEdit;

    //
    m_splitWidget = new QFrame( this );
    m_splitWidget->setFrameShape( QFrame::Panel );
    m_splitWidget->setWhatsThis( "split_widget" );

    //
    m_splitName_CB = new QComboBox();
    m_splitName_CB->setMinimumWidth( 175 );

    QFont font2 = m_splitName_CB->font();
    font2.setPointSize( 11 );
    m_splitName_CB->setFont( font2 );

    for ( int k = 0; k < Overlord::getInstance()->openedFilesCount(); ++k )
    {
        QString fullName = Overlord::getInstance()->openedFiles( k );
        add_splitCombo( fullName );

        if ( Overlord::getInstance()->openedModified( k ) )
        {
            update_splitCombo( fullName, true );
        }
    }

    //
    m_splitClose_PB = new QPushButton();
    m_splitClose_PB->setText( tr( "Close" ) );

    QBoxLayout *topbar_Layout = new QHBoxLayout();
    topbar_Layout->addWidget( m_splitName_CB, 1 );
    topbar_Layout->addSpacing( 25 );
    topbar_Layout->addWidget( m_splitClose_PB );
    topbar_Layout->addStretch( 2 );

    QBoxLayout *layout = new QVBoxLayout();
    layout->addLayout( topbar_Layout );
    layout->addWidget( m_split_textEdit );

    m_splitWidget->setLayout( layout );

    m_splitter->setOrientation( Qt::Vertical );        // Difference is here
    m_splitter->addWidget( m_splitWidget );

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );
    m_splitName_CB->setCurrentIndex( splitIndex );

    moveBar();

    connect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
             this, &MainWindow::split_NameChanged );

    connect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    connect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_split_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );
    connect( Overlord::getInstance(), &Overlord::settingsChanged,
             m_split_textEdit, &DiamondTextEdit::changeSettings );

}

void MainWindow::set_splitCombo()
{
    QString shortName = strippedName( m_splitFileName );

    bool isModified = m_split_textEdit->document()->isModified();

    if ( isModified )
    {
        shortName += " *";
    }

    int index = Overlord::getInstance()->openedFilesFind( m_splitFileName );

    if ( index != -1 )
    {
        Overlord::getInstance()->openedModifiedReplace( index,isModified );
    }

    //
    int splitIndex = m_splitName_CB->findData( m_splitFileName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->setItemText( splitIndex, shortName );
        m_splitName_CB->setItemData( splitIndex, m_splitFileName, Qt::ToolTipRole );
    }
}

void MainWindow::update_splitCombo( QString fullName, bool isModified )
{
    QString shortName = strippedName( fullName );

    if ( isModified )
    {
        shortName += " *";
    }

    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->setItemText( splitIndex, shortName );
        m_splitName_CB->setItemData( splitIndex,fullName, Qt::ToolTipRole );
    }
}

void MainWindow::add_splitCombo( QString fullName )
{
    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex == -1 )
    {
        QString shortName = strippedName( fullName );
        m_splitName_CB->addItem( shortName, fullName );

        splitIndex = m_splitName_CB->count() - 1;
        m_splitName_CB->setItemData( splitIndex,fullName, Qt::ToolTipRole );

    }
    else
    {
        set_splitCombo();

    }
}

void MainWindow::rm_splitCombo( QString fullName )
{
    int splitIndex = m_splitName_CB->findData( fullName );

    if ( splitIndex != -1 )
    {
        m_splitName_CB->removeItem( splitIndex );
    }
}

void MainWindow::split_NameChanged( int data )
{
    QString newName = m_splitName_CB->itemData( data ).toString();

    if ( m_splitFileName != newName )
    {

        // old doc
        disconnect( m_split_textEdit->document(), &QTextDocument::contentsChanged,
                    this, &MainWindow::set_splitCombo );

        // new doc
        m_splitFileName = newName;

        int whichTab = -1;

        for ( int k = 0; k < m_tabWidget->count(); ++k )
        {

            if ( newName == this->get_curFileName( k ) )
            {
                whichTab = k;
                break;
            }
        }

        if ( whichTab == -1 )
        {
            csError( tr( "Split Window Selection" ), tr( "Unable to locate selected document" ) );

            deleteOldSplit();
            return;
        }

        QWidget *temp = m_tabWidget->widget( whichTab );
        DiamondTextEdit *textEdit = dynamic_cast<DiamondTextEdit *>( temp );

        if ( textEdit )
        {
            // get document matching the file name
            m_split_textEdit->setDocument( textEdit->document() );

            m_split_textEdit->setReadOnly( textEdit->isReadOnly() );

            if ( m_split_textEdit->get_ColumnMode() )
            {
                m_split_textEdit->setFont( Overlord::getInstance()->fontColumn() );
            }
            else
            {
                m_split_textEdit->setFont( Overlord::getInstance()->fontNormal() );
            }

            QPalette temp = m_split_textEdit->palette();
            temp.setColor( QPalette::Text, Overlord::getInstance()->currentTheme().colorText() );
            temp.setColor( QPalette::Base, Overlord::getInstance()->currentTheme().colorBack() );
            m_split_textEdit->setPalette( temp );

            m_textEdit = m_split_textEdit;

            set_splitCombo();

            //
            m_curFile = m_splitFileName;
            setStatus_FName( m_curFile );
            setStatus_ReadWrite( m_textEdit->isReadOnly() );

            // ** retrieve slected syntax type
//       m_syntaxParser = m_textEdit->get_SyntaxParser();

            // retrieve the menu enum
//       m_syntaxEnum = m_textEdit->get_SyntaxEnum();

            // check the menu item
//       setSynType(m_syntaxEnum);

            moveBar();
            show_Spaces();
            show_Breaks();

            connect( m_split_textEdit->document(), &QTextDocument::contentsChanged,
                     this, &MainWindow::set_splitCombo );

        }
        else
        {
            // close the split
            csError( tr( "Split Window Selection" ), tr( "Selected document invalid" ) );
            deleteOldSplit();
        }
    }
}

void MainWindow::deleteOldSplit()
{
    // set focus to the current tab widget
    QWidget *temp = m_tabWidget->currentWidget();
    temp->setFocus();
    m_isSplit = false;


    disconnect( m_splitName_CB,   static_cast<void ( QComboBox::* )( int )>( &QComboBox::currentIndexChanged ),
                this, &MainWindow::split_NameChanged );

    disconnect( m_splitClose_PB,  &QPushButton::clicked, this, &MainWindow::splitCloseClicked );

    disconnect( m_split_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::set_splitCombo );
    disconnect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    disconnect( m_split_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    disconnect( m_split_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    disconnect( m_split_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    m_splitName_CB->clear();
    m_split_textEdit = nullptr;

    m_splitWidget->deleteLater();

}

void MainWindow::splitCloseClicked()
{
    deleteOldSplit();
    // There is a jiggling and juggling of focus with this close button
    // It gets focus back after we leave here.
    //
    m_refocusTimer->start();

}

void MainWindow::refocusTab()
{
    m_refocusTimer->stop();

    // set focus to the current tab widget
    // need to do this in a slot so it can be called by a timer
    // when closing non-modal dialogs we cannot do this within the close
    // of the dialog because it will be ignored.
    if ( m_textEdit )
    {
        m_textEdit->setFocus();
    }
    else
    {
        qWarning() << tr( "m_textEdit was null in refocusTab()" );
    }
}
