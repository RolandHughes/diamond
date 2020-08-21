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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "diamond_edit.h"
#include "overlord.h"
#include "syntax.h"
#include "ui_mainwindow.h"
#include "util.h"
#include "diamondlimits.h"
#include "macrostruct.h"

#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QList>
#include <QMenu>
#include <QMainWindow>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QPrinter>
#include <QRectF>
#include <QShortcut>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QStackedWidget>
#include <QResizeEvent>

class Dialog_AdvFind;


struct advFindStruct
{
    QString fileName;
    int lineNumber;
    QString text;
};

class MainWindow : public QMainWindow
{
    CS_OBJECT( MainWindow )

public:
    MainWindow( QStringList fileList, QStringList flagList );


    // indent
    void indentIncr( QString route );
    void indentDecr( QString route );

    // spell
    QStringList spell_getMaybe( QString word );

    // support
    bool loadFile( QString fileName, bool newTab, bool isAuto, bool isReload = false );

    CS_SLOT_1( Public, void Move( QPoint pos ) )
    CS_SLOT_2( Move )

    CS_SLOT_1( Public, void Resize( QSize size ) )
    CS_SLOT_2( Resize )

    CS_SLOT_1( Public, void setSynType( SyntaxTypes data ) )
    CS_SLOT_2( setSynType )

    CS_SLOT_1( Public, void afterVisible() )
    CS_SLOT_2( afterVisible )

    CS_SLOT_1( Public, void showEdtHelp() )
    CS_SLOT_2( showEdtHelp )

    CS_SLOT_1( Public, void split_Horizontal() )
    CS_SLOT_2( split_Horizontal )

    CS_SLOT_1( Public, void split_Vertical() )
    CS_SLOT_2( split_Vertical )

protected:
    void closeEvent( QCloseEvent *event );
    void dragEnterEvent( QDragEnterEvent *event );
    void dropEvent( QDropEvent *event );
    void resizeEvent( QResizeEvent *event );

private:
    Ui::MainWindow *m_ui;

    QString m_appPath;

    QString m_configFileName;
    void getConfigFileName();

    QStringList m_fileList;
    QStringList m_flagList;

    // textEdit
    DiamondTextEdit *m_textEdit;
    QTabWidget *m_tabWidget;
    QString m_curFile;


    // split
    DiamondTextEdit *m_split_textEdit;
    DiamondTextEdit *m_noSplit_textEdit;
    QSplitter *m_splitter;

    bool m_isSplit;
    QFrame *m_splitWidget;
    QString m_splitFileName;
    QComboBox *m_splitName_CB;
    QPushButton *m_splitClose_PB;

    void add_splitCombo( QString fullName );
    void rm_splitCombo( QString fullName );
    void update_splitCombo( QString fullName, bool isModified );

    // copy buffer
    QShortcut *m_actionCopyBuffer;

    // advanced find
    Dialog_AdvFind *m_dwAdvFind;

    QStringList m_recursiveList;
    QFrame *m_findWidget;
    QStandardItemModel *m_model;
    QList<advFindStruct> advFind_getResults( bool &aborted );
    void findRecursive( const QString &path, bool isFirstLoop = true );
    void advFind_ShowFiles( QList<advFindStruct> foundList );

    // replace
    int getReply();

    // macros
    bool m_record;
    QList<QKeyEvent *> m_macroList;

    void replaceQuery();
    void replaceAll();

    // passed parms
    void autoLoad();
    void argLoad( QList<QString> argList );

    // preset folders
    QAction *prefolder_Actions[DiamondLimits::PRESET_FOLDERS_MAX];

    void prefolder_CreateMenus();
    void prefolder_RedoList();
    //void prefolder_UpdateActions();

    // recent folders
    QAction *rfolder_Actions[DiamondLimits::RECENT_FOLDERS_MAX];

    void rfolder_CreateMenus();
    void rfolder_Add();
    void rfolder_UpdateActions();

    // recent files
    QAction *rf_Actions[DiamondLimits::RECENT_FILES_MAX];

    void rf_CreateMenus();
    void rf_Update();
    void rf_UpdateActions();


    // open tabs
    QAction *openTab_Actions[DiamondLimits::OPENTABS_MAX];

    void openTab_CreateMenus();
    void openTab_Add();
    void openTab_Delete();
    void openTab_UpdateActions();

    void openTab_Select( int index );
    void openTab_UpdateOneAction( int index, bool isModified );

    // menu bar
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *searchToolBar;
    QToolBar *viewToolBar;
    QToolBar *toolsToolBar;
    QToolBar *windowToolBar;

    // status bar
    QLabel *m_statusLine;
    QLabel *m_statusMode;
    QLabel *m_statusName;

    void openDoc( QString path );
    bool closeAll_Doc( bool isExit );
    void save_ConfigFile();

    // create shortcuts, menus, status bar
    void createShortCuts( bool setupAll );
    void createToolBars();
    void createStatusBar();
    void createConnections();
    void createToggles();

    QString adjustKey( QString sequence );

    void setStatusBar( QString msg, int timeOut );
    void setStatus_ColMode();
    void setStatus_FName( QString name );
    CS_SLOT_1( Public, void showNotDone( QString item ) )
    CS_SLOT_2( showNotDone )


    QString get_SyntaxPath( QString syntaxPath );
    QString get_xxFile( QString title, QString fname, QString filter );


    // printing
    int m_pageNo;
    int m_pageCount;

    QRectF m_printArea;
    double m_resolution;

    int get_HeaderSize( QPainter *painter );
    int get_FooterSize( QPainter *painter );

    void doHeader( QPainter *painter );
    void doFooter( QPainter *painter );
    QString macroExpand( QString macro );
    QString convertBlockToHTML( const QString &plain, int tabSpacing ) const;

    // support
    bool querySave();
    CS_SLOT_1( Public, bool saveFile( QString fileName, Overlord::SaveFiles saveType ) )
    CS_SLOT_2( saveFile )

    bool saveAs( Overlord::SaveFiles saveType );

    void setCurrentTitle( const QString &fileName, bool tabChange = false, bool isReload = false );
    void setDiamondTitle( const QString title );
    void forceSyntax( SyntaxTypes data );
    QString get_curFileName( int whichTab );

    void focusChanged( QWidget *prior, QWidget *current );

    void newFile();
    void open_RelatedFile();

    bool close_Doc();
    void reload();
    bool save();
    void saveAll();
    void print();

    void printOut( QPrinter * );
    void printPdf();
    void printPreview();

    void mw_undo();
    void mw_redo();
    void mw_cut();
    void mw_copy();
    void mw_paste();

    CS_SLOT_1( Private, void selectAll() )
    CS_SLOT_2( selectAll )

    void selectBlock();
    void selectLine();
    void selectWord();

    CS_SLOT_1( Private, void caseUpper() )
    CS_SLOT_2( caseUpper )

    CS_SLOT_1( Private, void caseLower() )
    CS_SLOT_2( caseLower )

    void caseCap();

    CS_SLOT_1( Private, void deleteLine() )
    CS_SLOT_2( deleteLine )

    CS_SLOT_1( Private, void deleteEOL() )
    CS_SLOT_2( deleteEOL )

    CS_SLOT_1( Private, void deleteThroughEOL() )
    CS_SLOT_2( deleteThroughEOL )

    CS_SLOT_1( Private, void insertDate() )
    CS_SLOT_2( insertDate )

    CS_SLOT_1( Private, void insertTime() )
    CS_SLOT_2( insertTime )

    CS_SLOT_1( Private, void rewrapParagraph() )
    CS_SLOT_2( rewrapParagraph )




    void insertSymbol();
    void columnMode();

    void find();
    void replace();
    void findNext();
    void findPrevious();
    void advFind();

    void goLine();
    void goColumn();
    void goTop();
    void goBottom();

    void lineHighlight();
    void moveBar();
    void lineNumbers();
    void wordWrap();

    void show_Spaces();
    void show_Breaks();
    void displayHTML();

    // document
    void formatUnix();
    void formatWin();
    void fixTab_Spaces();
    void fixSpaces_Tab();
    void deleteEOL_Spaces();

    // macro
    void mw_macroStart();
    void mw_macroStop();
    void macroPlay();
    void macroLoad();
    void macroEditNames();
    void spellCheck();

    // options
    void setColors();
    void setFont();
    void setOptions();
    void setPresetFolders();
    void setPrintOptions();
    void move_ConfigFile();
    void tabNew();
    void tabClose( int index );

    // help
    void diamondHelp();
    void about();

    //
    void documentWasModified();
    void setStatus_LineCol();
    void mw_tabClose();
    void tabChanged( int index );

    // adv find
    void advFind_View( const QModelIndex &index );
    void advFind_Close();

    // copy buffer
    void showCopyBuffer();

    // recent folders
    void showContext_RecentFolder( const QPoint &pt );

    CS_SLOT_1( Private, void rfolder_Open() )
    CS_SLOT_2( rfolder_Open )

    CS_SLOT_1( Private, void rfolder_ClearList() )
    CS_SLOT_2( rfolder_ClearList )

    CS_SLOT_1( Private, void rfolder_RemoveFName() )
    CS_SLOT_2( rfolder_RemoveFName )

    // preset folders
    CS_SLOT_1( Private, void prefolder_Open() )
    CS_SLOT_2( prefolder_Open )

    // recent files
    void showContext_Files( const QPoint &pt );

    CS_SLOT_1( Private, void rf_Open() )
    CS_SLOT_2( rf_Open )

    CS_SLOT_1( Private, void rf_ClearList() )
    CS_SLOT_2( rf_ClearList )

    CS_SLOT_1( Private, void rf_RemoveFName() )
    CS_SLOT_2( rf_RemoveFName )

    // open (tab) files
    void showContext_Tabs( const QPoint &pt );

    CS_SLOT_1( Private, void openTab_redo() )
    CS_SLOT_2( openTab_redo )

    // split
    void set_splitCombo();
    void split_NameChanged( int data );
    void split_CloseButton();
};

#endif
