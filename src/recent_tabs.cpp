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
#include "util.h"

#include <QStringList>
#include <QVariant>

void MainWindow::openTab_CreateMenus()
{
    // re-populate m_openedFiles
    QString fullName;
    QString tName;

    int cnt = m_tabWidget->count();
    Overlord::getInstance()->openedFilesClear();
    Overlord::getInstance()->openedModifiedClear();

    for ( int k = 0; k < cnt; ++k )
    {
        fullName = this->get_curFileName( k );

        if ( fullName.isEmpty() )
        {
            --cnt;

        }
        else
        {
            Overlord::getInstance()->openedFilesAppend( fullName );
            Overlord::getInstance()->openedModifiedAppend( false );
        }
    }

    //
    QMenu *windowMenu = m_ui->menuWindow;
    windowMenu->addSeparator();

    for ( int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k )
    {

        if ( k < cnt )
        {
            fullName = Overlord::getInstance()->openedFiles( k );
            tName    = fullName;

            if ( Overlord::getInstance()->openedModified( k ) )
            {
                tName += " *";
            }

        }
        else
        {
            tName = "file"+QString::number( k );

        }

        openTab_Actions[k] = new QAction( tName, this );

//    openTab_Actions[k]->setData("select-tab");
        openTab_Actions[k]->setData( QString( "select-tab" ) );

        windowMenu->addAction( openTab_Actions[k] );

        if ( k >= cnt )
        {
            openTab_Actions[k]->setVisible( false );
        }

        connect( openTab_Actions[k], &QAction::triggered, this, [this, k]( bool ) { openTab_Select( k ); } );
    }
}

void MainWindow::openTab_Select( int index )
{
    bool match = false;
    QString fullName = Overlord::getInstance()->openedFiles( index );

    if ( fullName.isEmpty() )
    {
        // something is pretty bogus

    }
    else
    {
        int cnt   = m_tabWidget->count();
        int index = m_tabWidget->currentIndex();

        for ( int k = 0; k < cnt; ++k )
        {
            QString tcurFile = this->get_curFileName( k );

            if ( tcurFile == fullName )
            {
                match = true;
                index = k;
                break;
            }
        }

        if ( match )
        {
            // select new tab
            m_tabWidget->setCurrentIndex( index );

        }
        else
        {
            // delete entry from list since it did not exist
            Overlord::getInstance()->openedFilesRemove( fullName );

            // update actions
            openTab_UpdateActions();
        }
    }
}

void MainWindow::showContext_Tabs( const QPoint &pt )
{
    QAction *action = m_ui->menuWindow->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "select-tab" )
        {
            QMenu *menu = new QMenu( this );
            menu->addAction( "Reorder Tab file list", this, SLOT( openTab_redo() ) );

            menu->exec( m_ui->menuWindow->mapToGlobal( pt ) );
            delete menu;
        }
    }
}

void MainWindow::openTab_redo()
{
    QWidget *temp;
    DiamondTextEdit *textEdit;

    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // re-populate m_openedFiles and m_openedModified
        QString tName;
        bool isModified;

        Overlord::getInstance()->openedFilesClear();
        Overlord::getInstance()->openedModifiedClear();

        int cnt = m_tabWidget->count();

        for ( int k = 0; k < cnt; ++k )
        {
            tName = this->get_curFileName( k );
            Overlord::getInstance()->openedFilesAppend( tName );

            temp = m_tabWidget->widget( k );
            textEdit = dynamic_cast<DiamondTextEdit *>( temp );

            if ( textEdit )
            {
                isModified = textEdit->document()->isModified();
                Overlord::getInstance()->openedModifiedAppend( isModified );
            }
        }

        for ( int i = 0; i < DiamondLimits::OPENTABS_MAX; ++i )
        {

            if ( i < cnt )
            {
                tName = Overlord::getInstance()->openedFiles( i );

                if ( Overlord::getInstance()->openedModified( i ) )
                {
                    tName += " *";
                }

            }
            else
            {
                tName = "file"+QString::number( i );
            }

            openTab_Actions[i]->setText( tName );

            if ( i >= cnt )
            {
                openTab_Actions[i]->setVisible( false );
            }

        }
    }
}

void MainWindow::openTab_Add()
{
    if ( m_curFile.isEmpty() )
    {
        return;
    }

    Overlord::getInstance()->openedFilesAppend( m_curFile );
    Overlord::getInstance()->openedModifiedAppend( false );

    // update actions
    openTab_UpdateActions();
}

void MainWindow::openTab_Delete()
{
    Overlord::getInstance()->openedFilesRemove( m_curFile );

    // update actions
    openTab_UpdateActions();
}

void MainWindow::openTab_UpdateActions()
{
    int cnt = Overlord::getInstance()->openedFilesCount();

    for ( int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k )
    {

        if ( k < cnt )
        {
            QString modified;

            if ( Overlord::getInstance()->openedModified( k ) )
            {
                modified += " *";
            }

            openTab_Actions[k]->setText( Overlord::getInstance()->openedFiles( k ) + modified );
            openTab_Actions[k]->setVisible( true );

        }
        else
        {
            openTab_Actions[k]->setVisible( false );
        }
    }
}

void MainWindow::openTab_UpdateOneAction( int index, bool isModified )
{
    if ( index < DiamondLimits::OPENTABS_MAX )
    {
        QString modified;

        if ( isModified )
        {
            modified += " *";
        }

        openTab_Actions[index]->setText( Overlord::getInstance()->openedFiles( index ) + modified );
    }
}
