/**************************************************************************
*
* Copyright (c) 2020 Roland Hughes
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
#include "overlord.h"
#include <qtconcurrentrun.h>
#include <QThread>
#include <QTime>

Overlord *Overlord::m_instance = nullptr;

Overlord::Overlord() :
    m_isComplete( false )
    , m_changed( false )
    , m_needsBroadcast( false )
    , m_edtDirection( false )
{
    connect( &m_settings, &Settings::Move, this, &Overlord::Move );
    connect( &m_settings, &Settings::Resize, this, &Overlord::Resize );
}

Overlord::~Overlord()
{
    if ( m_flushTimer.isActive() )
    {
        m_flushTimer.stop();
    }

    if ( m_broadcastTimer.isActive() )
    {
        m_broadcastTimer.stop();
    }

    // TODO:: see if we need to delete SyntaxPatterns in QMAP
    //        as they were dynamically allocated
    //

}

void Overlord::close()
{
    if ( m_flushTimer.isActive() )
    {
        m_flushTimer.stop();
    }

    if ( m_broadcastTimer.isActive() )
    {
        m_broadcastTimer.stop();
    }

    if ( m_changed )
    {
        m_settings.save();
    }
}

void Overlord::checkForChange()
{
    if ( m_changed )
    {
        m_changed = false;
        // launch without waiting
        QFuture<void> t1 = QtConcurrent::run( &m_settings, &Settings::save );
    }
}

void Overlord::checkForBroadcast()
{
    if ( m_needsBroadcast )
    {
        m_needsBroadcast = false;
        settingsChanged( &m_settings );
    }
}

// configFileName is neither loaded nor stored, so no need to set flag
bool Overlord::set_configFileName( QString name )
{
    m_configFileName = name;
    m_settings.m_configFileName = name;
    bool retVal = m_settings.load();

    if ( retVal )
    {
        m_isComplete = true;
        m_flushTimer.start( 30000 );   // flush to disk at most every 30 seconds
        m_broadcastTimer.start( 3000 ); // broadcast up to once every 3 seconds

        preloadSyntax();

        connect( &m_broadcastTimer, &QTimer::timeout, this, &Overlord::checkForBroadcast );
        connect( &m_flushTimer, &QTimer::timeout, this, &Overlord::checkForChange );
    }

    return retVal;
}

void Overlord::set_newConfigFileName( QString name )
{
    m_configFileName = name;
    m_settings.m_configFileName = name;
    m_changed = true;
    checkForChange();
}

// appPath not stored in file so no need to set changed flag
void Overlord::set_appPath( QString path )
{
    m_appPath = path;
    m_settings.m_appPath = path;
}

Overlord *Overlord::getInstance()
{
    if ( !m_instance )
    {
        m_instance = new Overlord();
    }

    return m_instance;
}

void Overlord::set_options( const Options &opt )
{
    m_settings.m_options = opt;
    markToNotify();
}


void Overlord::set_printSettings( const PrintSettings &prt )
{
    m_settings.m_printSettings = prt;
    markToNotify();
}

void Overlord::set_preFolderList( QStringList lst )
{
    m_settings.m_preFolderList = lst;
    markToNotify();
}

void Overlord::set_showLineHighlight( bool yesNo )
{
    m_settings.m_showLineHighlight = yesNo;
    markToNotify();
}

void Overlord::set_preloadClipper( bool yesNo )
{
    m_settings.m_options.set_preloadClipper( yesNo );
    markToNotify();
}

void Overlord::set_preloadCmake( bool yesNo )
{
    m_settings.m_options.set_preloadCmake( yesNo );
    markToNotify();
}

void Overlord::set_preloadCpp( bool yesNo )
{
    m_settings.m_options.set_preloadCpp( yesNo );
    markToNotify();
}

void Overlord::set_preloadCss( bool yesNo )
{
    m_settings.m_options.set_preloadCss( yesNo );
    markToNotify();
}

void Overlord::set_preloadDoxy( bool yesNo )
{
    m_settings.m_options.set_preloadDoxy( yesNo );
    markToNotify();
}

void Overlord::set_preloadErrLog( bool yesNo )
{
    m_settings.m_options.set_preloadErrLog( yesNo );
    markToNotify();
}

void Overlord::set_preloadJava( bool yesNo )
{
    m_settings.m_options.set_preloadJava( yesNo );
    markToNotify();
}

void Overlord::set_preloadJs( bool yesNo )
{
    m_settings.m_options.set_preloadJs( yesNo );
    markToNotify();
}


void Overlord::set_preloadHtml( bool yesNo )
{
    m_settings.m_options.set_preloadHtml( yesNo );
    markToNotify();
}

void Overlord::set_preloadJson( bool yesNo )
{
    m_settings.m_options.set_preloadJson( yesNo );
    markToNotify();
}

void Overlord::set_preloadMake( bool yesNo )
{
    m_settings.m_options.set_preloadMake( yesNo );
    markToNotify();
}

void Overlord::set_preloadNone( bool yesNo )
{
    m_settings.m_options.set_preloadNone( yesNo );
    markToNotify();
}

void Overlord::set_preloadNSI( bool yesNo )
{
    m_settings.m_options.set_preloadNSI( yesNo );
    markToNotify();
}

void Overlord::set_preloadPhp( bool yesNo )
{
    m_settings.m_options.set_preloadPhp( yesNo );
    markToNotify();
}

void Overlord::set_preloadPl( bool yesNo )
{
    m_settings.m_options.set_preloadPl( yesNo );
    markToNotify();
}


void Overlord::set_preloadPy( bool yesNo )
{
    m_settings.m_options.set_preloadPy( yesNo );
    markToNotify();
}

void Overlord::set_preloadSh( bool yesNo )
{
    m_settings.m_options.set_preloadSh( yesNo );
    markToNotify();
}

void Overlord::set_preloadTxt( bool yesNo )
{
    m_settings.m_options.set_preloadTxt( yesNo );
    markToNotify();
}

void Overlord::set_preloadXml( bool yesNo )
{
    m_settings.m_options.set_preloadXml( yesNo );
    markToNotify();
}

void Overlord::set_showLineNumbers( bool yesNo )
{
    m_settings.m_showLineNumbers = yesNo;
    markToNotify();
}

void Overlord::set_isColumnMode( bool yesNo )
{
    m_settings.m_isColumnMode = yesNo;
    markToNotify();
}

void Overlord::set_isSpellCheck( bool yesNo )
{
    m_settings.m_isSpellCheck = yesNo;
    markToNotify();
}

void Overlord::set_isWordWrap( bool yesNo )
{
    m_settings.m_isWordWrap = yesNo;
    markToNotify();
}

void Overlord::set_showSpaces( bool yesNo )
{
    m_settings.m_showSpaces = yesNo;
    markToNotify();
}

void Overlord::set_showBreaks( bool yesNo )
{
    m_settings.m_showBreaks = yesNo;
    markToNotify();
}


void Overlord::set_priorPath( const QString &priorPath )
{
    m_settings.m_priorPath = priorPath;
    markToNotify();
}

void Overlord::set_activeTheme( const QString &name )
{
    m_settings.m_activeTheme = name;
    markToNotify();
}

void Overlord::set_fontNormal( const QFont &font )
{
    m_settings.m_fontNormal = font;
    markToNotify();
}

void Overlord::set_fontColumn( const QFont &font )
{
    m_settings.m_fontColumn = font;
    markToNotify();
}

void Overlord::set_tabSpacing( int spacing )
{
    m_settings.m_options.set_tabSpacing( spacing );
    markToNotify();
}

void Overlord::set_rewrapColumn( int col )
{
    m_settings.m_options.set_rewrapColumn( col );
    markToNotify();
}

void Overlord::set_formatDate( const QString &fmt )
{
    m_settings.m_options.set_formatDate( fmt );
    markToNotify();
}

void Overlord::set_formatTime( const QString &fmt )
{
    m_settings.m_options.set_formatTime( fmt );
    markToNotify();
}

void Overlord::set_useSpaces( bool yesNo )
{
    m_settings.m_options.set_useSpaces( yesNo );
    markToNotify();
}

// flag members are not stored so don't set changed flag
void Overlord::set_flagNoAutoLoad( bool yesNo )
{
    m_settings.m_flagNoAutoLoad = yesNo;
}

// flag members are not stored so don't set changed flag
void Overlord::set_flagNoSaveConfig( bool yesNo )
{
    m_settings.m_flagNoSaveConfig = yesNo;
}

void Overlord::set_findList( QStringList list )
{
    m_settings.m_findList = list;
    markToNotify();
}

void Overlord::set_advancedFindText( QString text )
{
    m_settings.m_advancedFindText = text;
    markToNotify();
}

void Overlord::set_advancedFindFileType( QString text )
{
    m_settings.m_advancedFindFileType = text;
    markToNotify();
}

void Overlord::set_advancedFindFolder( QString text )
{
    m_settings.m_advancedFindFolder = text;
    markToNotify();
}

void Overlord::set_advancedFCase( bool yesNo )
{
    m_settings.m_advancedFCase = yesNo;
    markToNotify();
}

void Overlord::set_advancedFWholeWords( bool yesNo )
{
    m_settings.m_advancedFWholeWords = yesNo;
    markToNotify();
}

void Overlord::set_advancedFSearchFolders( bool yesNo )
{
    m_settings.m_advancedFSearchFolders = yesNo;
    markToNotify();
}

void Overlord::set_findText( QString text )
{
    m_settings.m_findText = text;
    markToNotify();
}

void Overlord::set_findFlags( QTextDocument::FindFlags flags )
{
    m_settings.m_findFlags = flags;
    markToNotify();
}

void Overlord::set_replaceText( QString text )
{
    m_settings.m_replaceText = text;
    markToNotify();
}

void Overlord::set_findWholeWords( bool yesNo )
{
    m_settings.m_findWholeWords = yesNo;
    markToNotify();
}

void Overlord::set_findDirection( bool yesNo )
{
    m_settings.m_findDirection = yesNo;
    markToNotify();
}

void Overlord::set_findFlagsBackward()
{
    m_settings.m_findFlags |= QTextDocument::FindBackward;
    markToNotify();
}

void Overlord::set_findFlagsCaseSensitive()
{
    m_settings.m_findFlags |= QTextDocument::FindCaseSensitively;
    markToNotify();
}

void Overlord::set_findFlagsWholeWords()
{
    m_settings.m_findFlags |= QTextDocument::FindWholeWords;
    markToNotify();
}

void Overlord::set_findCase( bool yesNo )
{
    m_settings.m_findCase = yesNo;
    markToNotify();
}

void Overlord::set_replaceList( QStringList lst )
{
    m_settings.m_replaceList = lst;
    markToNotify();
}


void Overlord::updateSettingsFromLocalCopy( Settings &settings )
{
    m_settings = settings;
    markToNotify();
    checkForChange();
}

Settings &Overlord::pullLocalSettingsCopy()
{
    return m_settings;
}

Settings *Overlord::pointerToSettings()
{
    return &m_settings;
}

void Overlord::set_macroNames( const QStringList &macroNames )
{
    m_settings.json_Save_MacroNames( macroNames );
    markToNotify();  // force flush to disk
    checkForChange();
}

QList<macroStruct> Overlord::viewMacro( QString macroName )
{
    return m_settings.json_View_Macro( macroName );
}

PrintSettings &Overlord::pullLocalCopyOfPrintSettings()
{
    return m_settings.m_printSettings;
}

void Overlord::updatePrintSettingsFromLocalCopy( PrintSettings &pSettings )
{
    m_settings.m_printSettings = pSettings;
    markToNotify();
}

Options &Overlord::pullLocalCopyOfOptions()
{
    return m_settings.m_options;
}

void Overlord::updateOptionsFromLocalCopy( Options &options )
{
    m_settings.m_options = options;
    markToNotify();
}

QStringList Overlord::pullCopyOfPreFolderList()
{
    return m_settings.m_preFolderList;
}

void Overlord::updatePreFolderListFromLocalCopy( QStringList &lst )
{
    m_settings.m_preFolderList = lst;
    markToNotify();
}

void Overlord::openedFilesClear()
{
    m_settings.openedFilesClear();
    markToNotify();
}

void Overlord::openedModifiedClear()
{
    m_settings.openedModifiedClear();
    markToNotify();
}

void Overlord::recentFileListClear()
{
    m_settings.recentFileListClear();
    markToNotify();
}

void Overlord::openedFilesAppend( QString fullName )
{
    m_settings.openedFilesAppend( fullName );
    markToNotify();
}

void Overlord::openedModifiedAppend( bool yesNo )
{
    m_settings.openedModifiedAppend( yesNo );
    markToNotify();
}

void Overlord::markToNotify()
{
    m_changed = true;
    m_needsBroadcast = true;
}

void Overlord::openedFilesRemove( QString name )
{
    m_settings.openedFilesRemove( name );
    markToNotify();
}

QStringList Overlord::loadMacroIds()
{
    return m_settings.json_Load_MacroIds();
}

bool Overlord::loadMacro( QString macroName )
{
    return m_settings.json_Load_Macro( macroName );
}

void Overlord::findListPrepend( QString text )
{
    m_settings.findListPrepend( text );
    markToNotify();
}

void Overlord::set_lastSize( QSize size )
{
    m_settings.set_lastSize( size );
    markToNotify();
}

void Overlord::set_lastPosition( QPoint pos )
{
    m_settings.set_lastPosition( pos );
    markToNotify();
}

void Overlord::set_lastActiveFile( QString fileName )
{
    m_settings.set_lastActiveFile( fileName );
    markToNotify();
}

void Overlord::set_lastActiveRow( int row )
{
    m_settings.set_lastActiveRow( row );
    markToNotify();
}

void Overlord::set_lastActiveColumn( int column )
{
    m_settings.set_lastActiveColumn( column );
    markToNotify();
}

void Overlord::openedModifiedReplace( int sub, bool yesNo )
{
    m_settings.openedModifiedReplace( sub, yesNo );
    markToNotify();
}

bool Overlord::openedFilesContains( QString name )
{
    return m_settings.openedFilesContains( name );
}

bool Overlord::recentFilesListContains( QString text )
{
    return m_settings.recentFilesListContains( text );
}

void Overlord::findListMove( int index, int dest )
{
    m_settings.findListMove( index, dest );
    markToNotify();
}

void Overlord::replaceListPrepend( QString text )
{
    m_settings.replaceListPrepend( text );
    markToNotify();
}

void Overlord::replaceListMove( int index, int dest )
{
    m_settings.replaceListMove( index, dest );
    markToNotify();
}

SyntaxPatterns *Overlord::getSyntaxPatterns( QString fileName )
{
    if ( !m_syntaxPatterns.contains( fileName ) )
    {
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    return m_syntaxPatterns[fileName];
}

void Overlord::preloadSyntax()
{
    QString fileName;

    if ( m_settings.m_options.preloadClipper() )
    {
        fileName = m_settings.syntaxPath() + "syn_clipper.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadCmake() )
    {
        fileName = m_settings.syntaxPath() + "syn_cmake.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadCpp() )
    {
        qDebug() << "preloading CPP syntax";
        fileName = m_settings.syntaxPath() + "syn_cpp.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
        qDebug() << "finished preloading CPP syntax";
    }

    if ( m_settings.m_options.preloadCss() )
    {
        fileName = m_settings.syntaxPath() + "syn_css.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadDoxy() )
    {
        fileName = m_settings.syntaxPath() + "syn_doxy.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadErrLog() )
    {
        fileName = m_settings.syntaxPath() + "syn_errlog.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadHtml() )
    {
        fileName = m_settings.syntaxPath() + "syn_html.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJava() )
    {
        fileName = m_settings.syntaxPath() + "syn_java.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJs() )
    {
        fileName = m_settings.syntaxPath() + "syn_js.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadJson() )
    {
        fileName = m_settings.syntaxPath() + "syn_json.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadMake() )
    {
        fileName = m_settings.syntaxPath() + "syn_make.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadNone() )
    {
        fileName = m_settings.syntaxPath() + "syn_none.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadNSI() )
    {
        fileName = m_settings.syntaxPath() + "syn_nsi.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPhp() )
    {
        fileName = m_settings.syntaxPath() + "syn_php.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPl() )
    {
        fileName = m_settings.syntaxPath() + "syn_pl.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadPy() )
    {
        fileName = m_settings.syntaxPath() + "syn_py.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadSh() )
    {
        fileName = m_settings.syntaxPath() + "syn_sh.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadTxt() )
    {
        fileName = m_settings.syntaxPath() + "syn_txt.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }

    if ( m_settings.m_options.preloadXml() )
    {
        fileName = m_settings.syntaxPath() + "syn_xml.json";
        m_syntaxPatterns[fileName] = new SyntaxPatterns( fileName );
    }
}

void Overlord::set_edtDirection( bool forwardBackward )
{
    m_edtDirection = forwardBackward;
    markToNotify();
}

void Overlord::set_edtLastDeletedWord( QString word )
{
    m_settings.m_edtLastDeletedWord = word;
    markToNotify();
    checkForBroadcast();        // needs to go out immediately in case cut and paste between buffers
}

void Overlord::set_edtLastDeletedLine( QString line )
{
    m_settings.m_edtLastDeletedLine = line;
    markToNotify();
    checkForBroadcast();        // needs to go out immediately in case cut and past between buffers
}

void Overlord::set_edtLastDeletedChar( QString c )
{
    m_settings.m_edtLastDeletedChar = c;
    markToNotify();
    checkForBroadcast();        // needs to go out immediately in case cut and paste between buffers
}
