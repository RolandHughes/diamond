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
#include "non_gui_functions.h"

#include <QCursor>
#include <QStringList>
#include <QVariant>

// ****  recent files
void MainWindow::rf_CreateMenus()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    QString tName;

    QMenu   *fileMenu = m_ui->menuFile;
    QAction *action   = fileMenu->insertSeparator( m_ui->actionExit );

    for ( int i = 0; i < DiamondLimits::RECENT_FILES_MAX; ++i )
    {

        if ( i < cnt )
        {
            tName = Overlord::getInstance()->recentFiles()[i];
        }
        else
        {
            tName = "file"+QString::number( i );
        }

        rf_Actions[i] = new QAction( tName, this );
        rf_Actions[i]->setData( QString( "recent-file" ) );

        fileMenu->insertAction( action, rf_Actions[i] );

        if ( i >= cnt )
        {
            rf_Actions[i]->setVisible( false );
        }

        connect( rf_Actions[i], SIGNAL( triggered() ), this, SLOT( rf_Open() ) );
    }
}

void MainWindow::rf_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        bool ok = loadFile( action->text(), true, false );

        if ( ! ok )
        {
            int index = Overlord::getInstance()->recentFiles().indexOf( action->text() );

            if ( index >= 0 )
            {
                Overlord::getInstance()->recentFiles().removeAt( index );
                Overlord::getInstance()->markToNotify();

                // update actions
                rf_UpdateActions();
            }
        }
    }
}

void MainWindow::showContext_Files( const QPoint &pt )
{
    QAction *action = m_ui->menuFile->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "recent-file" )
        {
            QString fName = action->text();

            QMenu *menu = new QMenu( this );
            menu->addAction( "Clear Recent file list", this, SLOT( rf_ClearList() ) );

            QAction *rfAction = menu->addAction( "Remove file:  " + fName, this, SLOT( rf_RemoveFName() ) );
            rfAction->setData( fName );

            menu->exec( m_ui->menuFile->mapToGlobal( pt ) );
            delete menu;
        }
    }
}

void MainWindow::rf_ClearList()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        Overlord::getInstance()->recentFileListClear();

        // update actions
        rf_UpdateActions();
    }
}

void MainWindow::rf_RemoveFName()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        QString fName = action->data().toString();

        int index = Overlord::getInstance()->recentFiles().indexOf( fName );

        if ( index >= 0 )
        {
            Overlord::getInstance()->recentFiles().removeAt( index );
            Overlord::getInstance()->markToNotify();

            // update actions
            rf_UpdateActions();
        }
    }
}

void MainWindow::rf_Update()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    if ( cnt >= DiamondLimits::RECENT_FILES_MAX )
    {
        Overlord::getInstance()->recentFiles().removeFirst();
    }

    Overlord::getInstance()->recentFiles().append( m_curFile );

    // update actions
    rf_UpdateActions();
}

void MainWindow::rf_UpdateActions()
{
    int cnt = Overlord::getInstance()->recentFiles().count();

    for ( int i = 0; i < DiamondLimits::RECENT_FILES_MAX; ++i )
    {

        if ( i < cnt )
        {
            rf_Actions[i]->setText( Overlord::getInstance()->recentFiles()[i] );
            rf_Actions[i]->setVisible( true );

        }
        else
        {
            rf_Actions[i]->setVisible( false );
        }

    }
}




// ****  recent folders
void MainWindow::rfolder_CreateMenus()
{
    int cnt = Overlord::getInstance()->recentFolders().count();

    QString tName;
    QMenu *menu = new QMenu( this );

    for ( int i = 0; i < DiamondLimits::RECENT_FOLDERS_MAX; ++i )
    {

        if ( i < cnt )
        {
            tName = Overlord::getInstance()->recentFolders()[i];
        }
        else
        {
            tName = "folder"+QString::number( i );
        }

        rfolder_Actions[i] = new QAction( tName, this );
        rfolder_Actions[i]->setData( QString( "recent-folder" ) );

        menu->addAction( rfolder_Actions[i] );

        if ( i >= cnt )
        {
            rfolder_Actions[i]->setVisible( false );
        }

        connect( rfolder_Actions[i], SIGNAL( triggered() ), this, SLOT( rfolder_Open() ) );
    }

    m_ui->actionOpen_RecentFolder->setMenu( menu );

}

void MainWindow::rfolder_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // pass the path
        openDoc( action->text() );
    }
}

void MainWindow::showContext_RecentFolder( const QPoint &pt )
{
    QAction *action = m_ui->actionOpen_RecentFolder->menu()->actionAt( pt );

    if ( action )
    {
        QString data = action->data().toString();

        if ( data == "recent-folder" )
        {
            QString fName = action->text();

            QMenu *menu = new QMenu( this );
            menu->addAction( "Clear Recent folder list",  this, SLOT( rfolder_ClearList() ) );

            QAction *rfAction = menu->addAction( "Remove folder:  " + fName, this,  SLOT( rfolder_RemoveFName() ) );
            rfAction->setData( fName );

            menu->exec( QCursor::pos() );
            delete menu;
        }
    }
}

void MainWindow::rfolder_ClearList()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        Overlord::getInstance()->recentFolders().clear();

        // update actions
        rfolder_UpdateActions();
    }
}

void MainWindow::rfolder_RemoveFName()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        QString fName = action->data().toString();

        int index = Overlord::getInstance()->recentFolders().indexOf( fName );

        if ( index >= 0 )
        {
            Overlord::getInstance()->recentFolders().removeAt( index );

            // update actions
            rfolder_UpdateActions();
        }
    }
}

void MainWindow::rfolder_Add()
{
    if ( m_curFile.isEmpty() )
    {
        return;
    }

    int cnt = Overlord::getInstance()->recentFolders().count();

    if ( cnt >= DiamondLimits::RECENT_FOLDERS_MAX )
    {
        Overlord::getInstance()->recentFolders().removeFirst();
    }

    QString fileName = pathName( m_curFile );

    if ( ! Overlord::getInstance()->recentFolders().contains( fileName ) )
    {
        Overlord::getInstance()->recentFolders().append( fileName );
    }

    // update actions
    rfolder_UpdateActions();
}

void MainWindow::rfolder_UpdateActions()
{
    int cnt = Overlord::getInstance()->recentFolders().count();

    for ( int i = 0; i < DiamondLimits::RECENT_FOLDERS_MAX; ++i )
    {

        if ( i < cnt )
        {
            rfolder_Actions[i]->setText( Overlord::getInstance()->recentFolders()[i] );
            rfolder_Actions[i]->setVisible( true );

        }
        else
        {
            rfolder_Actions[i]->setVisible( false );
        }

    }
}



// ****  preset folders
void MainWindow::prefolder_CreateMenus()
{
    QString tName;
    QMenu *menu = new QMenu( this );

    for ( int i = 0; i < DiamondLimits::PRESET_FOLDERS_MAX; ++i )
    {

        tName = Overlord::getInstance()->presetFolders()[i];

        if ( tName.isEmpty() )
        {
            continue;
        }

        prefolder_Actions[i] = new QAction( tName, this );
        prefolder_Actions[i]->setData( QString( "preset-folder" ) );

        menu->addAction( prefolder_Actions[i] );
        connect( prefolder_Actions[i], SIGNAL( triggered() ), this, SLOT( prefolder_Open() ) );
    }

    m_ui->actionOpen_PresetFolder->setMenu( menu );

}

void MainWindow::prefolder_Open()
{
    QAction *action;
    action = ( QAction * )sender();

    if ( action )
    {
        // pass the path
        openDoc( action->text() );
    }
}

void MainWindow::prefolder_RedoList()
{
    QMenu *menu = m_ui->actionOpen_PresetFolder->menu();
    menu->deleteLater();

    prefolder_CreateMenus();
}

