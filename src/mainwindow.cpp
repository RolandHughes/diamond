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

#include "diamond_build_info.h"
#include "mainwindow.h"
#include "non_gui_functions.h"

#include <stdexcept>

#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QToolBar>
#include "dialog_colors.h"
#include "dialog_fonts.h"
#include "dialog_options.h"
#include "dialog_preset.h"
#include "dialog_print_opt.h"
#include "mainwindow.h"
#include "dialog_config.h"
#include "overlord.h"
#include "dialog_edt_help.h"


#include <QBoxLayout>
#include <QFontDialog>
#include <QLabel>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include <QClipboard>

MainWindow::MainWindow( QStringList fileList, QStringList flagList )
    : m_ui( new Ui::MainWindow )
    , m_fileList( fileList )
    , m_flagList( flagList )
{
    m_ui->setupUi( this );
    setDiamondTitle( "untitled.txt" );

    m_ui->actionAstyle->setDisabled( true );

    m_appPath = QApplication::applicationDirPath();
    Overlord::getInstance()->set_appPath( m_appPath );

    setIconSize( QSize( 32,32 ) );
    setWindowIcon( QIcon( "://resources/diamond.png" ) );


    // drag & drop
    setAcceptDrops( true );

    // remaining methods must be done after json_Read for config
    m_tabWidget = new QTabWidget( this );
    m_tabWidget->setTabsClosable( true );
    m_tabWidget->setMovable( true );
    m_tabWidget->setWhatsThis( "tab_widget" );

    // set up the splitter, only display the tabWidget
    m_splitter = new QSplitter( Qt::Vertical );
    m_splitter->addWidget( m_tabWidget );
    setCentralWidget( m_splitter );

    connect( qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged );

    m_split_textEdit = 0;
    m_isSplit = false;

    // macros
    m_record = false;

    // copy buffer
    m_actionCopyBuffer = new QShortcut( this );
    connect( m_actionCopyBuffer, &QShortcut::activated, this, &MainWindow::showCopyBuffer );

    // set flags after reading config and before autoload
    if ( m_flagList.contains( "--no_autoload", Qt::CaseInsensitive ) )
    {
        Overlord::getInstance()->set_flagNoAutoLoad( true );
    }

    if ( m_flagList.contains( "--no_saveconfig", Qt::CaseInsensitive ) )
    {
        Overlord::getInstance()->set_flagNoSaveConfig( true );
    }

    connect( this, &MainWindow::queueNextPart, this, &MainWindow::onceEventLoopStarts );

    queueNextPart();

}

void MainWindow::onceEventLoopStarts()
{
    // Because the user "could" be prompted to choose a file or location
    // this critical part of Settings has to be done here.
    //
    getConfigFileName();

    connect( Overlord::getInstance(), &Overlord::preloadComplete, this, &MainWindow::startupStep2, Qt::QueuedConnection );

    if ( ! Overlord::getInstance()->set_configFileName( m_configFileName ) )
    {
        // do not start program
        csError( tr( "Configuration File Missing" ), tr( "Unable to locate or open the Diamond Configuration file." ) );
        throw std::runtime_error( "abort_no_message" );
    }

    QString syntaxPath = Overlord::getInstance()->syntaxPath();

    if ( ! QFile::exists( syntaxPath + "syn_txt.json" ) )
    {
        qDebug() << "syntax path not set in Overlord";
        Overlord::getInstance()->set_syntaxPath( get_SyntaxPath( syntaxPath ) );
    }


    if ( ! QFile::exists( Overlord::getInstance()->mainDictionary() ) )
    {
        Overlord::getInstance()->set_mainDictionary( get_xxFile( "Dictionary File (*.dic)", "en_US.dic", "Dictionary Files (*.dic)" ) );
    }

    Overlord::getInstance()->set_userDictionary( pathName( Overlord::getInstance()->mainDictionary() ) + "/userDict.txt" );

    if ( ! QFile::exists( Overlord::getInstance()->mainDictionary() ) )
    {
        // add missing file
        QFile temp( Overlord::getInstance()->mainDictionary() );
        temp.open( QIODevice::WriteOnly );
        temp.close();
    }


    m_refocusTimer = new QTimer( this );
    m_refocusTimer->setInterval( 500 );

    connect( m_refocusTimer, &QTimer::timeout, this, &MainWindow::refocusTab );

}

void MainWindow::startupStep2()
{
    // screen setup
    createShortCuts( true );
    createToolBars();
    createStatusBar();
    createToggles();
    createConnections();

    QProcess astyleCheck( this );

    astyleCheck.start( "astyle --help" );

    bool retVal = astyleCheck.waitForFinished();

    if ( retVal )
    {
        QString helpText( astyleCheck.readAllStandardOutput() );

        if ( helpText.length() > 100 )
        {
            m_ui->actionAstyle->setDisabled( false );
        }
    }


    // recent folders
    rfolder_CreateMenus();

    // preset  folders
    prefolder_CreateMenus();

    // recent files
    rf_CreateMenus();

    // recent folders, context menu
    QMenu *menuFolder_R = m_ui->actionOpen_RecentFolder->menu();
    menuFolder_R->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( menuFolder_R, &QMenu::customContextMenuRequested,     this, &MainWindow::showContext_RecentFolder );

    // recent files, context menu
    m_ui->menuFile->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->menuFile, &QMenu::customContextMenuRequested,   this, &MainWindow::showContext_Files );

    // window tab, context menu
    m_ui->menuWindow->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( m_ui->menuWindow, &QMenu::customContextMenuRequested, this, &MainWindow::showContext_Tabs );

    this->move( Overlord::getInstance()->lastPosition() );
    this->resize( Overlord::getInstance()->lastSize() );

    connect( Overlord::getInstance(), &Overlord::Move, this, &MainWindow::Move );
    connect( Overlord::getInstance(), &Overlord::Resize, this, &MainWindow::Resize );

    // Let the queue drain before we autoload any files
    connect( this, &MainWindow::nextStartupStep, this, &MainWindow::startupStep3, Qt::QueuedConnection );

    nextStartupStep();  // use of queued connection allows us to drain the event queue
}

void MainWindow::startupStep3()
{
    if ( Overlord::getInstance()->autoLoad() && ! Overlord::getInstance()->flagNoAutoLoad() )
    {
        autoLoad();
    }

    // user requested files on the command line
    if ( m_fileList.count() > 1 )
    {
        argLoad( m_fileList );
    }

    // no files were open, open a blank tab
    if ( m_tabWidget->count() == 0 )
    {
        tabNew();
    }

    // currently open tabs, must occur after autoLoad & argLoad
    openTab_CreateMenus();

    // find
    if ( ! Overlord::getInstance()->findList().isEmpty() )
    {
        Overlord::getInstance()->set_findText( Overlord::getInstance()->findList().first() );
    }

    // replace
    if ( ! Overlord::getInstance()->replaceList().isEmpty() )
    {
        Overlord::getInstance()->set_replaceText( Overlord::getInstance()->replaceList().first() );
    }

    setStatus_ColMode();
    setStatusBar( tr( "Ready" ), 0 );
    setUnifiedTitleAndToolBarOnMac( true );

}

// **window, tabs
void MainWindow::tabNew()
{
    m_textEdit = new DiamondTextEdit( this, "tab" );

    // keep reference
    m_noSplit_textEdit = m_textEdit;

    // triggers a call to tabChanged
    int index = m_tabWidget->addTab( m_textEdit, "untitled.txt" );
    m_tabWidget->setCurrentIndex( index );

//  TODO:: tabs are basically wrong.
//         A tab isn't a set of spaces it is a tab stop.
//         When a tab is 8 that means the cursor STOPS every 8 spaces, not lands after the next 8 space boundary
//         COBOL AREA-B starts in column 8 because that is where the tab stopped the typewriter.
//
    int tmp = m_textEdit->fontMetrics().width( " " );
    m_textEdit->setTabStopWidth( tmp * Overlord::getInstance()->tabSpacing() );

    m_textEdit->setFocus();

    // connect(m_textEdit, SIGNAL(fileDropped(const QString &)), this, SLOT(fileDropped(const QString &)));
    connect( m_textEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::documentWasModified );

    connect( m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::moveBar );
    connect( m_textEdit, &DiamondTextEdit::cursorPositionChanged,     this, &MainWindow::setStatus_LineCol );

    connect( m_textEdit, &DiamondTextEdit::undoAvailable, m_ui->actionUndo, &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::redoAvailable, m_ui->actionRedo, &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCut,  &QAction::setEnabled );
    connect( m_textEdit, &DiamondTextEdit::copyAvailable, m_ui->actionCopy, &QAction::setEnabled );

    connect( m_textEdit, &DiamondTextEdit::setSynType, this, &MainWindow::setSynType );

    // connect EDT signals here
    //
    connect( m_textEdit, &DiamondTextEdit::showEdtHelp, this, &MainWindow::showEdtHelp );
    connect( m_textEdit, &DiamondTextEdit::edtTop, this, &MainWindow::goTop );
    connect( m_textEdit, &DiamondTextEdit::edtBottom, this, &MainWindow::goBottom );
    connect( m_textEdit, &DiamondTextEdit::not_done, this, &MainWindow::showNotDone );
    connect( m_textEdit, &DiamondTextEdit::edtCut, this, &MainWindow::mw_cut );
    connect( m_textEdit, &DiamondTextEdit::edtUpcase, this, &MainWindow::caseUpper );
    connect( m_textEdit, &DiamondTextEdit::edtLowerCase, this, &MainWindow::caseLower );
    connect( m_textEdit, &DiamondTextEdit::edtSplitH, this, &MainWindow::split_Horizontal );
    connect( m_textEdit, &DiamondTextEdit::edtSplitV, this, &MainWindow::split_Vertical );
    connect( m_textEdit, &DiamondTextEdit::edtSaveFile, this, &MainWindow::saveFile );
    connect( m_textEdit, &DiamondTextEdit::edtSaveFileAs, this, &MainWindow::saveAs );
    connect( m_textEdit, &DiamondTextEdit::edtPaste, this, &MainWindow::mw_paste );
    connect( m_textEdit, &DiamondTextEdit::edtCopy, this, &MainWindow::mw_copy );
    connect( m_textEdit, &DiamondTextEdit::edtSubs, this, &MainWindow::replace );
    connect( m_textEdit, &DiamondTextEdit::timedMessage, this, &MainWindow::setStatusBar );
}

void MainWindow::mw_tabClose()
{
    qDebug() << "mw_tabClose()";
    int index = m_tabWidget->currentIndex();
    tabClose( index );
}

void MainWindow::tabClose( int index )
{
    m_tabWidget->setCurrentIndex( index );
    QWidget *tmp = m_tabWidget->widget( index );

    DiamondTextEdit *t_textEdit;
    t_textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

    if ( t_textEdit )
    {
        m_textEdit = t_textEdit;
        m_curFile  = this->get_curFileName( index );

        if ( close_Doc() )
        {

            if ( m_tabWidget->count() > 1 )
            {

                // hold textEdit and delete after tab is removed
                DiamondTextEdit *hold = m_textEdit;

                m_tabWidget->removeTab( index );
                delete hold;
            }
        }
    }
}

void MainWindow::tabChanged( int index )
{
    QWidget *tmp = m_tabWidget->widget( index );

    DiamondTextEdit *textEdit;
    textEdit = dynamic_cast<DiamondTextEdit *>( tmp );

    if ( textEdit )
    {
        m_textEdit = textEdit;

        // keep reference
        m_noSplit_textEdit = m_textEdit;

        m_curFile = this->get_curFileName( index );
        this->setCurrentTitle( m_curFile, true, false, m_textEdit->isReadOnly() );

        // **
        setStatus_LineCol();
        m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
        m_textEdit->set_ShowLineNum( Overlord::getInstance()->showLineNumbers() );

        moveBar();
        show_Spaces();
        show_Breaks();
    }
}

void MainWindow::focusChanged( QWidget *prior, QWidget *current )
{
    Q_UNUSED( prior )

    if ( ! current )
    {
        return;
    }

    DiamondTextEdit *t_textEdit;
    t_textEdit = dynamic_cast<DiamondTextEdit *>( current );

    if ( t_textEdit )
    {

        if ( m_textEdit->m_owner == t_textEdit->m_owner )
        {
            // do nothing

        }
        else
        {
            m_textEdit = t_textEdit;

            if ( m_textEdit->m_owner == "tab" )
            {
                // focus changed to the tabWidet

                // keep reference
                m_noSplit_textEdit = m_textEdit;

                int index = m_tabWidget->currentIndex();
                m_curFile = get_curFileName( index );

                setStatus_FName( m_curFile );
                setStatus_ReadWrite( m_textEdit->isReadOnly() );


            }
            else if ( m_textEdit->m_owner == "split" )
            {
                // focus changed to the splitWidget

                m_curFile = m_splitFileName;
                setStatus_FName( m_curFile );
                setStatus_ReadWrite( m_textEdit->isReadOnly() );
            }

            setStatus_LineCol();
            m_textEdit->set_ColumnMode( Overlord::getInstance()->isColumnMode() );
            m_textEdit->set_ShowLineNum( Overlord::getInstance()->showLineNumbers() );

            moveBar();
            show_Spaces();
            show_Breaks();

        }

        refocusTab();
    }
}


// **help
void MainWindow::diamondHelp()
{
    try
    {
        showHtml( "docs", "https://www.copperspice.com/docs/diamond/index.html" );

    }
    catch ( std::exception &e )
    {
        // do nothing for now
    }
}

void MainWindow::about()
{
    // change mainwindow.cpp & main.cpp

    QString icon = ":/resources/diamond.png";

    QString textBody =
        "<font color='#000080'><table style=margin-right:25>"

        "<tr><td><img width='96' height='96'src='" + icon + "'></td>"

        "    <td>"
        "      <table style='margin-left:25; margin-top:15px' >"
        "        <tr><td><nobr>Developed by Barbara Geller</nobr></td>"
        "            <td>barbara@copperspice.com</td></tr>"
        "        <tr><td style=padding-right:25><nobr>Developed by Ansel Sermersheim</nobr></td>"
        "            <td>ansel@copperspice.com</td></tr>"
        "       </table>"
        "    </td>"

        "</tr>"
        "</table></font>"

        "<p><small>Copyright 2012-2020 BG Consulting, released under the terms of the GNU GPL version 2<br>"
        "This program is provided AS IS with no warranty of any kind.<br></small></p>";

    //
    QMessageBox msgB;
    msgB.setIcon( QMessageBox::NoIcon );
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );

    msgB.setWindowTitle( tr( "About Diamond" ) );
    msgB.setText( tr( "<p style=margin-right:25><center><h5>Version: %1<br>Build # %2</h5></center></p>" )
                  .formatArgs( QString::fromLatin1( versionString ), QString::fromLatin1( buildDate ) ) );
    msgB.setInformativeText( textBody );

    msgB.setStandardButtons( QMessageBox::Ok );
    msgB.setDefaultButton( QMessageBox::Ok );

    msgB.exec();
}

void MainWindow::createConnections()
{
    // file
    connect( m_ui->actionNew,               &QAction::triggered, this, &MainWindow::newFile );
    connect( m_ui->actionOpen,              &QAction::triggered, this, [this]( bool ) { openDoc( Overlord::getInstance()->priorPath() ); } );
    connect( m_ui->actionOpen_RelatedFile,  &QAction::triggered, this, &MainWindow::open_RelatedFile );
    connect( m_ui->actionClose,             &QAction::triggered, this, [this]( bool ) { close_Doc(); } );
    connect( m_ui->actionClose_All,         &QAction::triggered, this, [this]( bool ) { closeAll_Doc( false ); } );
    connect( m_ui->actionReload,            &QAction::triggered, this, &MainWindow::reload );

    connect( m_ui->actionSave,              &QAction::triggered, this, [this]( bool ) { save(); } );
    connect( m_ui->actionSave_As,           &QAction::triggered, this, [this]( bool ) { saveAs( Overlord::SAVE_ONE ); } );
    connect( m_ui->actionSave_All,          &QAction::triggered, this, &MainWindow::saveAll );

    connect( m_ui->actionPrint,             &QAction::triggered, this, &MainWindow::print );
    connect( m_ui->actionPrint_Preview,     &QAction::triggered, this, &MainWindow::printPreview );
    connect( m_ui->actionPrint_Pdf,         &QAction::triggered, this, &MainWindow::printPdf );
    connect( m_ui->actionExit,              &QAction::triggered, this, [this]( bool ) { close(); } );

    // edit
    connect( m_ui->actionUndo,              &QAction::triggered, this, &MainWindow::mw_undo );
    connect( m_ui->actionRedo,              &QAction::triggered, this, &MainWindow::mw_redo );
    connect( m_ui->actionCut,               &QAction::triggered, this, &MainWindow::mw_cut );
    connect( m_ui->actionCopy,              &QAction::triggered, this, &MainWindow::mw_copy );
    connect( m_ui->actionPaste,             &QAction::triggered, this, &MainWindow::mw_paste );

    connect( m_ui->actionSelect_All,        &QAction::triggered, this, &MainWindow::selectAll );
    connect( m_ui->actionSelect_Block,      &QAction::triggered, this, &MainWindow::selectBlock );
    connect( m_ui->actionSelect_Line,       &QAction::triggered, this, &MainWindow::selectLine );
    connect( m_ui->actionSelect_Word,       &QAction::triggered, this, &MainWindow::selectWord );
    connect( m_ui->actionCase_Upper,        &QAction::triggered, this, &MainWindow::caseUpper );
    connect( m_ui->actionCase_Lower,        &QAction::triggered, this, &MainWindow::caseLower );
    connect( m_ui->actionCase_Cap,          &QAction::triggered, this, &MainWindow::caseCap );

    connect( m_ui->actionIndent_Incr,       &QAction::triggered, this, [this]( bool ) { indentIncr( "indent" );   } );
    connect( m_ui->actionIndent_Decr,       &QAction::triggered, this, [this]( bool ) { indentDecr( "unindent" ); } );
    connect( m_ui->actionDelete_Line,       &QAction::triggered, this, &MainWindow::deleteLine );
    connect( m_ui->actionDelete_EOL,        &QAction::triggered, this, &MainWindow::deleteEOL );
    connect( m_ui->actionDelete_ThroughEOL, &QAction::triggered, this, &MainWindow::deleteThroughEOL );

    connect( m_ui->actionInsert_Date,       &QAction::triggered, this, &MainWindow::insertDate );
    connect( m_ui->actionInsert_Time,       &QAction::triggered, this, &MainWindow::insertTime );
    connect( m_ui->actionInsert_Symbol,     &QAction::triggered, this, &MainWindow::insertSymbol );
    connect( m_ui->actionColumn_Mode,       &QAction::triggered, this, &MainWindow::columnMode );

    // search
    connect( m_ui->actionFind,              &QAction::triggered, this, &MainWindow::find );
    connect( m_ui->actionReplace,           &QAction::triggered, this, &MainWindow::replace );
    connect( m_ui->actionFind_Next,         &QAction::triggered, this, &MainWindow::findNext );
    connect( m_ui->actionFind_Prev,         &QAction::triggered, this, &MainWindow::findPrevious );
    connect( m_ui->actionAdv_Find,          &QAction::triggered, this, &MainWindow::advFind );
    connect( m_ui->actionGo_Line,           &QAction::triggered, this, &MainWindow::goLine );
    connect( m_ui->actionGo_Column,         &QAction::triggered, this, &MainWindow::goColumn );
    connect( m_ui->actionGo_Top,            &QAction::triggered, this, &MainWindow::goTop );
    connect( m_ui->actionGo_Bottom,         &QAction::triggered, this, &MainWindow::goBottom );

    // view
    connect( m_ui->actionLine_Highlight,    &QAction::triggered, this, &MainWindow::lineHighlight );
    connect( m_ui->actionLine_Numbers,      &QAction::triggered, this, &MainWindow::lineNumbers );
    connect( m_ui->actionWord_Wrap,         &QAction::triggered, this, &MainWindow::wordWrap );
    connect( m_ui->actionShow_Spaces,       &QAction::triggered, this, &MainWindow::show_Spaces );
    connect( m_ui->actionShow_Breaks,       &QAction::triggered, this, &MainWindow::show_Breaks );
    connect( m_ui->actionDisplay_HTML,      &QAction::triggered, this, &MainWindow::displayHTML );
    connect( m_ui->actionClipboard,         &QAction::triggered, this, &MainWindow::showClipboard );
    connect( m_ui->actionBackups,           &QAction::triggered, this, &MainWindow::showBackups );

    // document
    connect( m_ui->actionSyn_C,             &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_C );       } );
    connect( m_ui->actionSyn_Clipper,       &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_CLIPPER ); } );
    connect( m_ui->actionSyn_CMake,         &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_CMAKE );   } );
    connect( m_ui->actionSyn_Css,           &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_CSS );     } );
    connect( m_ui->actionSyn_Doxy,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_DOXY );    } );
    connect( m_ui->actionSyn_ErrorLog,      &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_ERRLOG );  } );
    connect( m_ui->actionSyn_Html,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_HTML );    } );
    connect( m_ui->actionSyn_Java,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_JAVA );    } );
    connect( m_ui->actionSyn_Javascript,    &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_JS );      } );
    connect( m_ui->actionSyn_Json,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_JSON );    } );
    connect( m_ui->actionSyn_Makefile,      &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_MAKE );    } );
    connect( m_ui->actionSyn_Nsis,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_NSIS );    } );
    connect( m_ui->actionSyn_Text,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_TEXT );    } );
    connect( m_ui->actionSyn_Shell,         &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_SHELL );   } );
    connect( m_ui->actionSyn_Perl,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_PERL );    } );
    connect( m_ui->actionSyn_PHP,           &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_PHP );     } );
    connect( m_ui->actionSyn_Python,        &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_PYTHON );  } );
    connect( m_ui->actionSyn_Xml,           &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_XML );     } );
    connect( m_ui->actionSyn_None,          &QAction::triggered, this, [this]( bool ) { forceSyntax( SYN_NONE );    } );

    // connect(m_ui->actionSyn_UNUSED1,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED1); } );
    // connect(m_ui->actionSyn_UNUSED2,    &QAction::triggered, this, [this](bool){ forceSyntax(SYN_UNUSED2); } );

    connect( m_ui->actionFormat_Unix,       &QAction::triggered, this, &MainWindow::formatUnix );
    connect( m_ui->actionFormat_Win,        &QAction::triggered, this, &MainWindow::formatWin );

    connect( m_ui->actionFix_Tab_Spaces,    &QAction::triggered, this, &MainWindow::fixTab_Spaces );
    connect( m_ui->actionFix_Spaces_Tab,    &QAction::triggered, this, &MainWindow::fixSpaces_Tab );
    connect( m_ui->actionDeleteEOL_Spaces,  &QAction::triggered, this, &MainWindow::deleteEOL_Spaces );

    // tools
    connect( m_ui->actionMacro_Start,       &QAction::triggered, this, &MainWindow::mw_macroStart );
    connect( m_ui->actionMacro_Stop,        &QAction::triggered, this, &MainWindow::mw_macroStop );
    connect( m_ui->actionMacro_Play,        &QAction::triggered, this, &MainWindow::macroPlay );
    connect( m_ui->actionMacro_Load,        &QAction::triggered, this, &MainWindow::macroLoad );
    connect( m_ui->actionMacro_EditNames,   &QAction::triggered, this, &MainWindow::macroEditNames );
    connect( m_ui->actionSpell_Check,       &QAction::triggered, this, &MainWindow::spellCheck );
    connect( m_ui->actionAstyle,            &QAction::triggered, this, &MainWindow::astyle );

    // settings
    connect( m_ui->actionColors,            &QAction::triggered, this, &MainWindow::setColors );
    connect( m_ui->actionFonts,             &QAction::triggered, this, &MainWindow::setFont );
    connect( m_ui->actionOptions,           &QAction::triggered, this, &MainWindow::setOptions );
    connect( m_ui->actionPresetFolders,     &QAction::triggered, this, &MainWindow::setPresetFolders );
    connect( m_ui->actionPrintOptions,      &QAction::triggered, this, &MainWindow::setPrintOptions );
    connect( m_ui->actionMove_ConfigFile,   &QAction::triggered, this, &MainWindow::move_ConfigFile );

    // window
    connect( m_ui->actionTab_New,           &QAction::triggered, this, &MainWindow::tabNew );
    connect( m_ui->actionTab_Close,         &QAction::triggered, this, &MainWindow::mw_tabClose );
    connect( m_ui->actionSplit_Horizontal,  &QAction::triggered, this, &MainWindow::split_Horizontal );
    connect( m_ui->actionSplit_Vertical,    &QAction::triggered, this, &MainWindow::split_Vertical );

    // help menu
    connect( m_ui->actionDiamond_Help,      &QAction::triggered, this, &MainWindow::diamondHelp );
    connect( m_ui->actionAbout,             &QAction::triggered, this, &MainWindow::about );
}

void MainWindow::createToggles()
{
    m_ui->actionSyn_C->setCheckable( true );
    m_ui->actionSyn_Clipper->setCheckable( true );
    m_ui->actionSyn_CMake->setCheckable( true );
    m_ui->actionSyn_Css->setCheckable( true );
    m_ui->actionSyn_Doxy->setCheckable( true );
    m_ui->actionSyn_ErrorLog->setCheckable( true );
    m_ui->actionSyn_Html->setCheckable( true );
    m_ui->actionSyn_Java->setCheckable( true );
    m_ui->actionSyn_Javascript->setCheckable( true );
    m_ui->actionSyn_Json->setCheckable( true );
    m_ui->actionSyn_Makefile->setCheckable( true );
    m_ui->actionSyn_Nsis->setCheckable( true );
    m_ui->actionSyn_Text->setCheckable( true );
    m_ui->actionSyn_Shell->setCheckable( true );
    m_ui->actionSyn_Perl->setCheckable( true );
    m_ui->actionSyn_PHP->setCheckable( true );
    m_ui->actionSyn_Python->setCheckable( true );
    m_ui->actionSyn_Xml->setCheckable( true );
    m_ui->actionSyn_None->setCheckable( true );

    // m_ui->actionSyn_Usused1->setCheckable(true);
    // m_ui->actionSyn_Unused2->setCheckable(true);

    m_ui->actionLine_Highlight->setCheckable( true );
    m_ui->actionLine_Highlight->setChecked( Overlord::getInstance()->showLineHighlight() );

    m_ui->actionLine_Numbers->setCheckable( true );
    m_ui->actionLine_Numbers->setChecked( Overlord::getInstance()->showLineNumbers() );

    m_ui->actionWord_Wrap->setCheckable( true );
    m_ui->actionWord_Wrap->setChecked( Overlord::getInstance()->isWordWrap() );

    m_ui->actionShow_Spaces->setCheckable( true );
    m_ui->actionShow_Spaces->setChecked( Overlord::getInstance()->showSpaces() );

    m_ui->actionShow_Breaks->setCheckable( true );
    m_ui->actionShow_Breaks->setChecked( Overlord::getInstance()->showBreaks() );

    m_ui->actionColumn_Mode->setCheckable( true );
    m_ui->actionColumn_Mode->setChecked( Overlord::getInstance()->isColumnMode() );

    m_ui->actionSpell_Check->setCheckable( true );
    m_ui->actionSpell_Check->setChecked( Overlord::getInstance()->isSpellCheck() );

    m_ui->actionUndo->setEnabled( false );
    m_ui->actionRedo->setEnabled( false );
    m_ui->actionCut->setEnabled( false );
    m_ui->actionCopy->setEnabled( false );

    m_ui->actionBackups->setEnabled( Overlord::getInstance()->backupDirectory().length() > 1 );

    connect( m_tabWidget, &QTabWidget::currentChanged,    this, &MainWindow::tabChanged );
    connect( m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::tabClose );
}

void MainWindow::createShortCuts( bool setupAll )
{
    // ** standard definded shortcuts

    if ( setupAll )
    {
        // file
        m_ui->actionOpen->setShortcut( QKeySequence( Overlord::getInstance()->keys().open() ) );
        m_ui->actionClose->setShortcut( QKeySequence( Overlord::getInstance()->keys().close() ) );
        m_ui->actionSave->setShortcut( QKeySequence( Overlord::getInstance()->keys().save() ) );
        m_ui->actionSave_As->setShortcut( QKeySequence( Overlord::getInstance()->keys().saveAs() ) );
        m_ui->actionPrint->setShortcut( QKeySequence( Overlord::getInstance()->keys().print() ) );
        m_ui->actionExit->setShortcuts( QKeySequence::Quit );

        // edit
        m_ui->actionUndo->setShortcut( QKeySequence( Overlord::getInstance()->keys().undo() ) );
        m_ui->actionRedo->setShortcut( QKeySequence( Overlord::getInstance()->keys().redo() ) );
        m_ui->actionCut->setShortcut( QKeySequence( Overlord::getInstance()->keys().cut() ) );
        m_ui->actionCopy->setShortcut( QKeySequence( Overlord::getInstance()->keys().copy() ) );
        m_ui->actionPaste->setShortcut( QKeySequence( Overlord::getInstance()->keys().paste() ) );

        m_ui->actionSelect_All->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectAll() ) );
        m_ui->actionGo_Top->setShortcut( QKeySequence( Overlord::getInstance()->keys().goTop() ) );
        m_ui->actionGo_Bottom->setShortcut( QKeySequence( Overlord::getInstance()->keys().goBottom() ) );

        //search
        m_ui->actionFind->setShortcut( QKeySequence( Overlord::getInstance()->keys().find() ) );
        m_ui->actionReplace->setShortcut( QKeySequence( Overlord::getInstance()->keys().replace() ) );
        m_ui->actionFind_Next->setShortcut( QKeySequence( Overlord::getInstance()->keys().findNext() ) );;
        m_ui->actionFind_Prev->setShortcut( QKeySequence( Overlord::getInstance()->keys().findPrev() ) );

        // tab
        m_ui->actionTab_New->setShortcut( QKeySequence( Overlord::getInstance()->keys().newTab() ) );

        // help
        m_ui->actionDiamond_Help->setShortcuts( QKeySequence::HelpContents );
    }

    // ** user definded

    // edit
    m_ui->actionPrint_Preview->setShortcut( QKeySequence( Overlord::getInstance()->keys().printPreview() ) );
    m_ui->actionReload->setShortcut( QKeySequence( Overlord::getInstance()->keys().reload() ) );

    m_ui->actionSelect_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectLine() )   );
    m_ui->actionSelect_Word->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectWord() )   );
    m_ui->actionSelect_Block->setShortcut( QKeySequence( Overlord::getInstance()->keys().selectBlock() ) );
    m_ui->actionCase_Upper->setShortcut( QKeySequence( Overlord::getInstance()->keys().upper() ) );
    m_ui->actionCase_Lower->setShortcut( QKeySequence( Overlord::getInstance()->keys().lower() ) );

    m_ui->actionIndent_Incr->setShortcut( QKeySequence( Overlord::getInstance()->keys().indentIncrement() ) );
    m_ui->actionIndent_Decr->setShortcut( QKeySequence( Overlord::getInstance()->keys().indentDecrement() ) );
    m_ui->actionDelete_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteLine() ) );
    m_ui->actionDelete_EOL->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteToEOL() )   );
    m_ui->actionDelete_ThroughEOL->setShortcut( QKeySequence( Overlord::getInstance()->keys().deleteThroughEOL() ) );
    m_ui->actionColumn_Mode->setShortcut( QKeySequence( Overlord::getInstance()->keys().columnMode() ) );

    // search
    m_ui->actionGo_Line->setShortcut( QKeySequence( Overlord::getInstance()->keys().gotoLine() ) );

    // view
    m_ui->actionShow_Spaces->setShortcut( QKeySequence( Overlord::getInstance()->keys().showSpaces() ) );
    m_ui->actionShow_Breaks->setShortcut( QKeySequence( Overlord::getInstance()->keys().showBreaks() ) );

    // tools
    m_ui->actionMacro_Play->setShortcut( QKeySequence( Overlord::getInstance()->keys().macroPlay() )   );
    m_ui->actionSpell_Check->setShortcut( QKeySequence( Overlord::getInstance()->keys().spellCheck() ) );

    // copy buffer
    m_actionCopyBuffer->setKey( QKeySequence( Overlord::getInstance()->keys().copyBuffer() ) );
}


void MainWindow::createToolBars()
{
    m_ui->actionTab_New->setIcon( QIcon( ":/resources/tab_new.png" ) );
    m_ui->actionTab_Close->setIcon( QIcon( ":/resources/tab_remove.png" ) );

    m_ui->actionOpen->setIcon( QIcon( ":/resources/file_open.png" ) );
    m_ui->actionClose->setIcon( QIcon( ":/resources/file_close.png" ) );
    m_ui->actionSave->setIcon( QIcon( ":/resources/save.png" ) );

    m_ui->actionPrint->setIcon( QIcon( ":/resources/print.png" ) );
    m_ui->actionPrint_Pdf->setIcon( QIcon( ":/resources/print_pdf.png" ) );

    m_ui->actionUndo->setIcon( QIcon( ":/resources/undo.png" ) );
    m_ui->actionRedo->setIcon( QIcon( ":/resources/redo.png" ) );
    m_ui->actionCut->setIcon( QIcon( ":/resources/cut.png" ) );
    m_ui->actionCopy->setIcon( QIcon( ":/resources/copy.png" ) );
    m_ui->actionPaste->setIcon( QIcon( ":/resources/paste.png" ) );

    m_ui->actionFind->setIcon( QIcon( ":/resources/find.png" ) );
    m_ui->actionReplace->setIcon( QIcon( ":/resources/find_replace.png" ) );

    m_ui->actionShow_Spaces->setIcon( QIcon( ":/resources/show_spaces.png" ) );
    m_ui->actionShow_Breaks->setIcon( QIcon( ":/resources/show_eol.png" ) );

    m_ui->actionMacro_Start->setIcon( QIcon( ":/resources/macro_rec.png" ) );
    m_ui->actionMacro_Stop->setIcon( QIcon( ":/resources/macro_stop.png" ) );
    m_ui->actionMacro_Play->setIcon( QIcon( ":/resources/macro_play.png" ) );
    m_ui->actionSpell_Check->setIcon( QIcon( ":/resources/spellcheck.png" ) );

    //
    fileToolBar = addToolBar( tr( "File" ) );
    fileToolBar->addAction( m_ui->actionOpen );
    fileToolBar->addAction( m_ui->actionClose );
    fileToolBar->addAction( m_ui->actionSave );
    fileToolBar->addSeparator();
    fileToolBar->addAction( m_ui->actionPrint );
    fileToolBar->addAction( m_ui->actionPrint_Pdf );

#ifdef Q_OS_MAC
    // os x does not support moving tool bars
    fileToolBar->addSeparator();
#endif

    windowToolBar = addToolBar( tr( "Window" ) );
    windowToolBar->addAction( m_ui->actionTab_New );
    windowToolBar->addAction( m_ui->actionTab_Close );

#ifdef Q_OS_MAC
    windowToolBar->addSeparator();
#endif

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( m_ui->actionUndo );
    editToolBar->addAction( m_ui->actionRedo );
    editToolBar->addAction( m_ui->actionCut );
    editToolBar->addAction( m_ui->actionCopy );
    editToolBar->addAction( m_ui->actionPaste );

#ifdef Q_OS_MAC
    editToolBar->addSeparator();
#endif

    searchToolBar = addToolBar( tr( "Search" ) );
    searchToolBar->addAction( m_ui->actionFind );
    searchToolBar->addAction( m_ui->actionReplace );

#ifdef Q_OS_MAC
    searchToolBar->addSeparator();
#endif

    viewToolBar = addToolBar( tr( "View" ) );
    viewToolBar->addAction( m_ui->actionShow_Spaces );
    viewToolBar->addAction( m_ui->actionShow_Breaks );

#ifdef Q_OS_MAC
    viewToolBar->addSeparator();
#endif

    toolsToolBar = addToolBar( tr( "Tools" ) );
    toolsToolBar->addAction( m_ui->actionMacro_Start );
    toolsToolBar->addAction( m_ui->actionMacro_Stop );
    toolsToolBar->addAction( m_ui->actionMacro_Play );
    toolsToolBar->addSeparator();
    toolsToolBar->addAction( m_ui->actionSpell_Check );
}

void MainWindow::createStatusBar()
{
    m_statusLine = new QLabel( "", this );

    m_statusMode = new QLabel( "", this );

    m_statusName = new QLabel( "", this );

    m_statusReadWrite = new QLabel( "", this );

    statusBar()->addPermanentWidget( m_statusLine, 0 );
    statusBar()->addPermanentWidget( m_statusMode, 0 );
    statusBar()->addPermanentWidget( m_statusReadWrite, 0 );
    statusBar()->addPermanentWidget( m_statusName, 0 );
}

void MainWindow::setStatusBar( QString msg, int timeOut )
{
    statusBar()->showMessage( msg, timeOut );
}

void MainWindow::showNotDone( QString item )
{
    csMsg( item + " - this feature has not been implemented." );
}


// **settings
void MainWindow::setColors()
{
    Dialog_Colors dw( this );

    dw.exec();
}

void MainWindow::setFont()
{
    Dialog_Fonts dw( Overlord::getInstance()->fontNormal(), Overlord::getInstance()->fontColumn() );
    dw.exec();
}

void MainWindow::setOptions()
{
    Options options = Overlord::getInstance()->pullLocalCopyOfOptions();


    Dialog_Options dw( this );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        // false will redisplay only user defined shortcuts
        this->createShortCuts( true );
    }
}

void MainWindow::setPresetFolders()
{
    Dialog_Preset dw( this );
    int result = dw.exec();

    if ( result == QDialog::Accepted )
    {
        prefolder_RedoList();
    }
}

void MainWindow::setPrintOptions()
{
    Dialog_PrintOptions dw( this );
    dw.exec();
}

QString MainWindow::get_SyntaxPath( QString syntaxPath )
{
    QString msg  = tr( "Select Diamond Syntax Folder" );
    QString path = get_DirPath( this, msg, syntaxPath );

    return path;
}

QString MainWindow::get_xxFile( QString title, QString fname, QString filter )
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    fname = m_appPath + "/" + fname;

    QString file = QFileDialog::getOpenFileName( this, "Select " + title,
                   fname, filter, &selectedFilter, options );

    return file;
}

void MainWindow::Move( QPoint pos )
{
    move( pos );

    Overlord::getInstance()->set_lastPosition( this->pos() );
    Overlord::getInstance()->set_lastSize( this->size() );
}

void MainWindow::Resize( QSize size )
{
    resize( size );
}

void MainWindow::resizeEvent( QResizeEvent *e )
{
    QMainWindow::resizeEvent( e );

    Overlord::getInstance()->set_lastPosition( this->pos() );
    Overlord::getInstance()->set_lastSize( this->size() );
}

void MainWindow::getConfigFileName()
{

#if defined(Q_OS_UNIX) && ! defined(Q_OS_MAC)

    QString homePath = QDir::homePath();
    m_configFileName = homePath + "/.config/Diamond/config.json";

    QDir( homePath ).mkdir( ".config" );
    QDir( homePath + "/.config" ).mkdir( "Diamond" );
    QDir( homePath + "/.config/Diamond" ).mkdir( "dictionary" );

    return;

#elif defined(Q_OS_MAC)

    if ( m_appPath.contains( ".app/Contents/MacOS" ) )
    {
        QString homePath = QDir::homePath();
        m_configFileName = homePath + "/Library/Diamond/config.json";

        QDir( homePath + "/Library" ).mkdir( "Diamond" );
        QDir( homePath + "/Library/Diamond" ).mkdir( "dictionary" );

        return;
    }

#endif

    QString selectedFilter;
    QFileDialog::Options options;

    QMessageBox quest;
    quest.setWindowTitle( tr( "Diamond Editor" ) );
    quest.setText( tr( "Diamond configuration file is missing.\n\n"
                       "Selet an option to (a) create the configuration file in the system default location, "
                       "(b) pick a folder location, or (c) select an existing Diamond Configuration file.\n" ) );

    QPushButton *createDefault  = quest.addButton( "Default Location",     QMessageBox::AcceptRole );
    QPushButton *createNew      = quest.addButton( "Pick Folder Location", QMessageBox::AcceptRole );
    QPushButton *selectExist    = quest.addButton( "Select Existing File", QMessageBox::AcceptRole );

    quest.setStandardButtons( QMessageBox::Cancel );
    quest.setDefaultButton( QMessageBox::Cancel );

    quest.exec();

    if ( quest.clickedButton() == createDefault )
    {
        m_configFileName = m_appPath + "/config.json";

#ifdef Q_OS_WIN
        QString path = QStandardPaths::writableLocation( QStandardPaths::ConfigLocation );
        m_configFileName  = path + "/config.json";
#endif

    }
    else if ( quest.clickedButton() == createNew )
    {
        QString fname = m_appPath + "/config.json";

        // force windows 7 and 8 to honor initial path
        options = QFileDialog::ForceInitialDir_Win7;

        m_configFileName = QFileDialog::getSaveFileName( this, tr( "Create new Configuration File" ),
                           fname, tr( "Json Files (*.json)" ), &selectedFilter, options );

    }
    else if ( quest.clickedButton() == selectExist )
    {

        m_configFileName = QFileDialog::getOpenFileName( this, tr( "Select Existing Diamond Configuration File" ),
                           "", tr( "Json Files (*.json)" ), &selectedFilter, options );

    }
    else
    {
        // user aborted
        m_configFileName = "";

    }
}


void MainWindow::move_ConfigFile()
{
    QSettings settings( "Diamond Editor", "Settings" );
    m_configFileName = settings.value( "configName" ).toString();

    //
    Dialog_Config *dw = new Dialog_Config( m_configFileName );
    int result = dw->exec();

    switch ( result )
    {
        case QDialog::Rejected:
            break;

        case 1:
            // create
        {
            QString selectedFilter;
            QFileDialog::Options options;

            // force windows 7 and 8 to honor initial path
            options = QFileDialog::ForceInitialDir_Win7;

            QString newName = QFileDialog::getSaveFileName( this, tr( "Create New Configuration File" ),
                              m_appPath + "/config.json", tr( "Json Files (*.json)" ),
                              &selectedFilter, options );

            if ( newName.isEmpty() )
            {
                // do nothing

            }
            else if ( QFile::exists( newName ) )
            {
                // can this happen?
                csError( "Diamond Configuration",
                         "Configuration file already exists, unable to create new file." );

            }
            else
            {
                m_configFileName = newName;
                settings.setValue( "configName", m_configFileName );
                Overlord::getInstance()->set_newConfigFileName( m_configFileName );
            }

            break;
        }

        case 2:
            // select
        {
            QString selectedFilter;
            QFileDialog::Options options;

            QString newName = QFileDialog::getOpenFileName( this,
                              tr( "Select Diamond Configuration File" ),
                              "*.json",
                              tr( "Json Files (*.json)" ),
                              &selectedFilter, options );

            if ( newName.isEmpty() )
            {
                // do nothing

            }
            else if ( QFile::exists( newName ) )
            {
                m_configFileName = newName;
                settings.setValue( "configName", m_configFileName );

                Overlord::getInstance()->set_configFileName( m_configFileName );

                csError( "Diamond Configuration", "New configuration file selected."
                         " Restart Diamond to utilize the new configuration file settings." );
            }

            break;
        }

        case 3:
            // rename
            QString newName = dw->get_newName();

            if ( newName.isEmpty() )
            {
                csError( "Diamond Configuration", "No configuration file name specified, unable to rename." );

            }

            if ( QFile::exists( newName ) )
            {
                csError( "Diamond Configuration", "New configuration file already exists, unable to rename." );

            }
            else
            {

                QString path = pathName( newName );
                QDir directory( path );

                if ( ! directory.exists() )
                {
                    directory.mkpath( path );
                }

                if ( QFile::rename( m_configFileName, newName ) )
                {
                    m_configFileName = newName;
                    settings.setValue( "configName", m_configFileName );

                }
                else
                {
                    csError( "Diamond Configuration", "Configuration file rename failed." );

                }
            }

            break;
    }
}

void MainWindow::showEdtHelp()
{
    Dialog_Edt_Help dw( this );
    dw.exec();
}

void MainWindow::showClipboard()
{
    QDialog dw( this );
    dw.setMinimumSize( 400, 400 );
    dw.setWindowTitle( "Clipboard contents" );
    dw.setFont( Overlord::getInstance()->fontNormal() );

    QHBoxLayout *layout = new QHBoxLayout;

    QTextEdit *ed = new QTextEdit();
    ed->setReadOnly( true );
    ed->setText( QApplication::clipboard()->text() );

    layout->addWidget( ed );
    dw.setLayout( layout );

    dw.exec();
}

void MainWindow::astyle()
{
    if ( m_textEdit )
    {
        m_textEdit->astyleBuffer();
    }
}
