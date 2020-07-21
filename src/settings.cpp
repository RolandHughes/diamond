/**************************************************************************
*
* Copyright (c) 2012-2020 Roland Hughes
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

#include "settings.h"
#include "util.h"
#include "diamondlimits.h"
#include "non_gui_functions.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFlags>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QKeySequence>
#include <QPoint>
#include <QPushButton>
#include <QSettings>
#include <qglobal.h>
#include <QKeyEvent>
#include <QStringParser>
#include <QDebug>

Settings::Settings() :
    m_showLineHighlight( true )
    , m_showLineNumbers( true )
    , m_isColumnMode( false )
    , m_isComplete( false )
    , m_isSpellCheck( false )
    , m_isWordWrap( false )
    , m_showSpaces( false )
    , m_showBreaks( false )
    , m_flagNoAutoLoad( false )
    , m_flagNoSaveConfig( false )
    , m_advancedFCase( false )
    , m_advancedFWholeWords( false )
    , m_advancedFSearchFolders( false )
    , m_findDirection( false )
    , m_findWholeWords( false )
    , m_findCase( false )
    , m_lastPosition( QPoint( 400, 200 ) )
    , m_lastSize( QSize( 800, 600 ) )
    , m_advancedFindText( "diamond" )
    , m_advancedFindFileType( "." )
{
    m_fontNormal            = QFont( "Monospace", 10 );
    m_fontColumn            = QFont( "Monospace", 10 );
    m_appPath               = QCoreApplication::applicationDirPath();
    m_priorPath             = m_appPath;
    m_advancedFindFolder    = m_appPath;

    generateDefaultThemes();

    m_isComplete = true;
}

Settings::Settings( const Settings &other ) :
    m_activeTheme( other.m_activeTheme )
    , m_advancedFCase( other.m_advancedFCase )
    , m_advancedFSearchFolders( other.m_advancedFSearchFolders )
    , m_advancedFWholeWords( other.m_advancedFWholeWords )
    , m_advancedFindFileType( other.m_advancedFindFileType )
    , m_advancedFindFolder( other.m_advancedFindFolder )
    , m_advancedFindText( other.m_advancedFindText )
    , m_appPath( other.m_appPath )
    , m_configFileName( other.m_configFileName )
    , m_findCase( other.m_findCase )
    , m_findDirection( other.m_findDirection )
    , m_findFlags( other.m_findFlags )
    , m_findList( other.m_findList )
    , m_findText( other.m_findText )
    , m_findWholeWords( other.m_findWholeWords )
    , m_flagNoAutoLoad( other.m_flagNoAutoLoad )
    , m_flagNoSaveConfig( other.m_flagNoSaveConfig )
    , m_fontColumn( other.m_fontColumn )
    , m_fontNormal( other.m_fontNormal )
    , m_isColumnMode( other.m_isColumnMode )
    , m_isComplete( other.m_isComplete )
    , m_isSpellCheck( other.m_isSpellCheck )
    , m_isWordWrap( other.m_isWordWrap )
    , m_lastPosition( other.m_lastPosition )
    , m_lastSize( other.m_lastSize )
    , m_macroNames( other.m_macroNames )
    , m_openedFiles( other.m_openedFiles )
    , m_openedModified( other.m_openedModified )
    , m_options( other.m_options )
    , m_preFolderList( other.m_preFolderList )
    , m_printSettings( other.m_printSettings )
    , m_priorPath( other.m_priorPath )
    , m_rFolderList( other.m_rFolderList )
    , m_replaceList( other.m_replaceList )
    , m_replaceText( other.m_replaceText )
    , m_recentFilesList( other.m_recentFilesList )
    , m_showBreaks( other.m_showBreaks )
    , m_showLineHighlight( other.m_showLineHighlight )
    , m_showLineNumbers( other.m_showLineNumbers )
    , m_showSpaces( other.m_showSpaces )
    , m_themes( other.m_themes )
{
}

Settings &Settings::operator =( const Settings &other )
{
    if ( this != &other )
    {
        m_activeTheme               = other.m_activeTheme;
        m_advancedFCase             = other.m_advancedFCase;
        m_advancedFSearchFolders    = other.m_advancedFSearchFolders;
        m_advancedFWholeWords       = other.m_advancedFWholeWords;
        m_advancedFindFileType      = other.m_advancedFindFileType;
        m_advancedFindFolder        = other.m_advancedFindFolder;
        m_advancedFindText          = other.m_advancedFindText;
        m_appPath                   = other.m_appPath;
        m_configFileName            = other.m_configFileName;
        m_findCase                  = other.m_findCase;
        m_findDirection             = other.m_findDirection;
        m_findFlags                 = other.m_findFlags;
        m_findList                  = other.m_findList;
        m_findText                  = other.m_findText;
        m_findWholeWords            = other.m_findWholeWords;
        m_flagNoAutoLoad            = other.m_flagNoAutoLoad;
        m_flagNoSaveConfig          = other.m_flagNoSaveConfig;
        m_fontColumn                = other.m_fontColumn;
        m_fontNormal                = other.m_fontNormal;
        m_isColumnMode              = other.m_isColumnMode;
        m_isComplete                = other.m_isComplete;
        m_isSpellCheck              = other.m_isSpellCheck;
        m_isWordWrap                = other.m_isWordWrap;
        m_lastPosition              = other.m_lastPosition;
        m_lastSize                  = other.m_lastSize;
        m_macroNames                = other.m_macroNames;
        m_openedFiles               = other.m_openedFiles;
        m_openedModified            = other.m_openedModified;
        m_options                   = other.m_options;
        m_preFolderList             = other.m_preFolderList;
        m_printSettings             = other.m_printSettings;
        m_priorPath                 = other.m_priorPath;
        m_rFolderList               = other.m_rFolderList;
        m_replaceList               = other.m_replaceList;
        m_replaceText               = other.m_replaceText;
        m_recentFilesList           = other.m_recentFilesList;
        m_showBreaks                = other.m_showBreaks;
        m_showLineHighlight         = other.m_showLineHighlight;
        m_showLineNumbers           = other.m_showLineNumbers;
        m_showSpaces                = other.m_showSpaces;
        m_themes                    = other.m_themes;
    }

    return *this;
}

bool operator ==( const Settings &left, const Settings &right )
{
    bool retVal = true;

    if ( left.m_advancedFCase != right.m_advancedFCase )
    {
        retVal = false;
    }

    if ( left.m_advancedFSearchFolders != right.m_advancedFSearchFolders )
    {
        retVal = false;
    }
    
    if (left.m_advancedFWholeWords != right.m_advancedFWholeWords)
    {
        retVal = false;
    }

    if (left.m_findCase != right.m_findCase)
    {
        retVal = false;
    }

    if (left.m_findDirection != right.m_findDirection)
    {
        retVal = false;
    }

    if (left.m_findWholeWords != right.m_findWholeWords)
    {
        retVal = false;
    }

    // skip testing "flag" members as they are command line only
    //
    if (left.m_isColumnMode != right.m_isColumnMode)
    {
        retVal = false;
    }

    if (left.m_isComplete != right.m_isComplete)
    {
        retVal = false;
    }

    if (left.m_isSpellCheck != right.m_isSpellCheck)
    {
        retVal = false;
    }

    if (left.m_isWordWrap != right.m_isWordWrap)
    {
        retVal = false;
    }

    if (left.m_showBreaks != right.m_showBreaks)
    {
        retVal = false;
    }

    if (left.m_showLineHighlight != right.m_showLineHighlight)
    {
        retVal = false;
    }

    if (left.m_showLineNumbers != right.m_showLineNumbers)
    {
        retVal = false;
    }

    if (left.m_showSpaces != right.m_showSpaces)
    {
        retVal = false;
    }

    if (left.m_activeTheme != right.m_activeTheme)
    {
        retVal = false;
    }

    if (left.m_advancedFindFileType != right.m_advancedFindFileType)
    {
        retVal = false;
    }

    if (left.m_advancedFindFolder != right.m_advancedFindFolder)
    {
        retVal = false;
    }

    if (left.m_advancedFindText != right.m_advancedFindText)
    {
        retVal = false;
    }

    if (left.m_appPath != right.m_appPath)
    {
        retVal = false;
    }

    if (left.m_configFileName != right.m_configFileName)
    {
        retVal = false;
    }

    if (left.m_findText != right.m_findText)
    {
        retVal = false;
    }

    if (left.m_priorPath != right.m_priorPath)
    {
        retVal = false;
    }

    if (left.m_replaceText != right.m_replaceText)
    {
        retVal = false;
    }

    if (left.m_findList != right.m_findList)
    {
        retVal = false;
    }

    if (left.m_macroNames != right.m_macroNames)
    {
        retVal = false;
    }

    if (left.m_openedFiles != right.m_openedFiles)
    {
        retVal = false;
    }

    if (left.m_preFolderList != right.m_preFolderList)
    {
        retVal = false;
    }

    if (left.m_rFolderList != right.m_rFolderList)
    {
        retVal = false;
    }

    if (left.m_replaceList != right.m_replaceList)
    {
        retVal = false;
    }

    if (left.m_recentFilesList != right.m_recentFilesList)
    {
        retVal = false;
    }

    if (left.m_fontColumn != right.m_fontColumn)
    {
        retVal = false;
    }

    if (left.m_fontNormal != right.m_fontNormal)
    {
        retVal = false;
    }

    if (left.m_lastPosition != right.m_lastPosition)
    {
        retVal = false;
    }

    if (left.m_lastPosition != right.m_lastPosition)
    {
        retVal = false;
    }

    if (left.m_lastSize != right.m_lastSize)
    {
        retVal = false;
    }

    if (left.m_openedModified != right.m_openedModified)
    {
        retVal = false;
    }

    if (left.m_findFlags != right.m_findFlags)
    {
        retVal = false;
    }

    if (left.m_options != right.m_options)
    {
        retVal = false;
    }

    if (left.m_themes != right.m_themes)
    {
        retVal = false;
    }

    if (left.m_printSettings != right.m_printSettings)
    {
        retVal = false;
    }
    
    return retVal;
}

bool operator !=( const Settings &left, const Settings &right )
{
    bool retVal = true;

    if (left == right)
    {
        retVal = false;
    }

    return retVal;
}

void Settings::copyTheme( QString name, QString dest )
{
    if ( themeNameExists( name ) )
    {
        if ( !themeNameExists( dest ) )
        {
            Themes tempTheme = m_themes[name];
            m_themes[dest] = tempTheme;
            m_themes[dest].set_protection( false );
            m_activeTheme = dest;       // TODO:: Need to refresh display
        }
        else
        {
            csError( tr( "Copy Theme" ), tr( "Destination name " ) + dest + tr( "  already exists" ) );
        }
    }
    else
    {
        csError( tr( "Copy Theme" ), tr( "Source name " ) + name + tr( " does not exist" ) );
    }
}

bool Settings::themeNameExists( QString name )
{
    return m_themes.contains( name );
}

void Settings::deleteTheme( QString name )
{
    if ( themeNameExists( name ) )
    {
        m_themes.remove( name );
    }
    else
    {
        csError( tr( "Delete Theme" ), tr( "Theme name " ) + name + tr( " does not exist" ) );
    }
}

void Settings::set_options( const Options &opt )
{
    m_options = opt;
    save();
}

void Settings::set_printSettings( const PrintSettings &prt )
{
    m_printSettings = prt;
    save();
}

bool Settings::load( Config trail )
{
    bool ok = true;

    if ( m_configFileName.isEmpty() )
    {
        return false;
    }


    if ( ! QFile::exists( m_configFileName ) )
    {
        save();
    }

    if ( ok )
    {

        // get existing json data
        QByteArray data = json_ReadFile();

        QJsonDocument doc = QJsonDocument::fromJson( data );

        QJsonObject object = doc.object();
        QJsonValue value;
        QJsonArray list;

        int cnt;

        //
        value = object.value( "pos-x" );
        int x = value.toDouble();

        value = object.value( "pos-y" );
        int y = value.toDouble();

        m_lastPosition = QPoint( x, y );
        Move( m_lastPosition );

        //
        value = object.value( "size-width" );
        int width = value.toDouble();

        value = object.value( "size-height" );
        int height = value.toDouble();

        m_lastSize = QSize( width, height );
        Resize( m_lastSize );

        // options
        m_options.set_rewrapColumn( object.value( "rewrapColumn" ).toInt() );
        m_options.set_tabSpacing( object.value( "tabSpacing" ).toInt() );
        m_options.set_useSpaces( object.value( "useSpaces" ).toBool() );
        m_options.set_removeSpaces( object.value( "removeSpace" ).toBool() );
        m_options.set_autoLoad( object.value( "autoLoad" ).toBool() );
        m_options.set_mainDictionary( object.value( "dictMain" ).toString() );
        m_options.set_userDictionary( object.value( "dictUser" ).toString() );
        m_options.set_formatDate( object.value( "formatDate" ).toString() );
        m_options.set_formatTime( object.value( "formatTime" ).toString() );
        m_options.set_syntaxPath( object.value( "pathSyntax" ).toString() );

        // settings
        m_showLineHighlight   = object.value( "showLineHighlight" ).toBool();
        m_showLineNumbers     = object.value( "showLineNumbers" ).toBool();
        m_isWordWrap          = object.value( "word-wrap" ).toBool();
        m_showSpaces          = object.value( "showSpaces" ).toBool();
        m_showBreaks          = object.value( "showBreaks" ).toBool();
        m_isColumnMode        = object.value( "column-mode" ).toBool();
        m_isSpellCheck        = object.value( "spellcheck" ).toBool();
        m_priorPath           = object.value( "pathPrior" ).toString();
        // screen fonts
        m_fontNormal          = fontFromString( object.value( "font-normal" ).toString() );
        m_fontColumn          = fontFromString( object.value( "font-column" ).toString() );


        // printer options
        m_printSettings.set_lineNumbers( object.value( "prt-lineNumbers" ).toBool() );
        m_printSettings.set_printHeader( object.value( "prt-pritnHeader" ).toBool() );
        m_printSettings.set_printFooter( object.value( "prt-printFooter" ).toBool() );
        m_printSettings.set_headerLeft( object.value( "prt-headerLeft" ).toString() );
        m_printSettings.set_headerCenter( object.value( "prt-headerCenter" ).toString() );
        m_printSettings.set_headerRight( object.value( "prt-headerRight" ).toString() );
        m_printSettings.set_headerLine2( object.value( "prt-headerLine2" ).toString() );
        m_printSettings.set_footerLeft( object.value( "prt-footerLeft" ).toString() );
        m_printSettings.set_footerCenter( object.value( "prt-footerCenter" ).toString() );
        m_printSettings.set_footerRight( object.value( "prt-footerRight" ).toString() );
        m_printSettings.set_footerLine2( object.value( "prt-footerLine2" ).toString() );
        m_printSettings.set_marginTop( object.value( "prt-marginTop" ).toDouble() );
        m_printSettings.set_marginBottom( object.value( "prt-marginBottom" ).toDouble() );
        m_printSettings.set_marginLeft( object.value( "prt-marginLeft" ).toDouble() );
        m_printSettings.set_marginRight( object.value( "prt-marginRight" ).toDouble() );
        m_printSettings.set_headerGap( object.value( "prt-hdrGap" ).toDouble() );
        m_printSettings.set_fontHeader( fontFromString( object.value( "prt-fontHeader" ).toString() ) );
        m_printSettings.set_fontFooter( fontFromString( object.value( "prt-fontFooter" ).toString() ) );
        m_printSettings.set_fontText( fontFromString( object.value( "prt-fontText" ).toString() ) );

        // TODO:: Have to handle map of keys here
#if 0
        // standard keys
        m_settings.key_open        = object.value( "key-open" ).toString();
        m_settings.key_close       = object.value( "key-close" ).toString();
        m_settings.key_save        = object.value( "key-save" ).toString();
        m_settings.key_saveAs      = object.value( "key-saveAs" ).toString();
        m_settings.key_print       = object.value( "key-print" ).toString();
//    m_settings.key_exit        = object.value("key-exit").toString();

        m_settings.key_undo        = object.value( "key-undo" ).toString();
        m_settings.key_redo        = object.value( "key-redo" ).toString();
        m_settings.key_cut         = object.value( "key-cut" ).toString();
        m_settings.key_copy        = object.value( "key-copy" ).toString();
        m_settings.key_paste       = object.value( "key-paste" ).toString();
        m_settings.key_selectAll   = object.value( "key-selectAll" ).toString();
        m_settings.key_find        = object.value( "key-find" ).toString();
        m_settings.key_replace     = object.value( "key-replace" ).toString();
        m_settings.key_findNext    = object.value( "key-findNext" ).toString();
        m_settings.key_findPrev    = object.value( "key-findPrev" ).toString();
        m_settings.key_goTop       = object.value( "key-goTop" ).toString();
        m_settings.key_goBottom    = object.value( "key-goBottom" ).toString();
        m_settings.key_newTab      = object.value( "key-newTab" ).toString();
//    m_settings.key_help        = object.value("key-help").toString();

        // user keys
        m_settings.key_printPreview = object.value( "key-printPreview" ).toString();
        m_settings.key_reload       = object.value( "key-reload" ).toString();
        m_settings.key_selectLine   = object.value( "key-selectLine" ).toString();
        m_settings.key_selectWord   = object.value( "key-selectWord" ).toString();
        m_settings.key_selectBlock  = object.value( "key-selectBlock" ).toString();
        m_settings.key_upper        = object.value( "key-upper" ).toString();
        m_settings.key_lower        = object.value( "key-lower" ).toString();
        m_settings.key_indentIncr   = object.value( "key-indentIncr" ).toString();
        m_settings.key_indentDecr   = object.value( "key-indentDecr" ).toString();
        m_settings.key_deleteLine   = object.value( "key-deleteLine" ).toString();
        m_settings.key_deleteEOL    = object.value( "key-deleteEOL" ).toString();
        m_settings.key_columnMode   = object.value( "key-columnMode" ).toString();
        m_settings.key_goLine       = object.value( "key-goLine" ).toString();
        m_settings.key_show_Spaces  = object.value( "key-showShow" ).toString();
        m_settings.key_show_Breaks  = object.value( "key-showBreaks" ).toString();
        m_settings.key_macroPlay    = object.value( "key-macroPlay" ).toString();
        m_settings.key_spellCheck   = object.value( "key-spellCheck" ).toString();
        m_settings.key_copyBuffer   = object.value( "key-copyBuffer" ).toString();
#endif

        // TODO:: Need to load multiple themes here.
        //        Need to handle legacy conversion as well
        //
#if 0
        // colors
        m_settings.colorText      = colorFromValueString( object.value( "color-text" ).toString()    );
        m_settings.colorBack      = colorFromValueString( object.value( "color-back" ).toString()    );
        m_settings.colorHighText  = colorFromValueString( object.value( "color-highText" ).toString() );
        m_settings.colorHighBack  = colorFromValueString( object.value( "color-highBack" ).toString() );

        // test added to resolve color conflicts
        if ( m_settings.colorBack == m_settings.colorText )
        {
            csError( "Diamond Configuration", "Background and Text colors are the same.\n\n"
                     "To change the colors select 'Settings' from the main Menu, then select 'Colors'.\n" );
        }

        list = object.value( "syntax-key" ).toArray();
        m_settings.syn_KeyWeight  = list.at( 0 ).toDouble();
        m_settings.syn_KeyItalic  = list.at( 1 ).toBool();
        m_settings.syn_KeyText    = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-type" ).toArray();
        m_settings.syn_TypeWeight = list.at( 0 ).toDouble();
        m_settings.syn_TypeItalic = list.at( 1 ).toBool();
        m_settings.syn_TypeText   = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-class" ).toArray();
        m_settings.syn_ClassWeight = list.at( 0 ).toDouble();
        m_settings.syn_ClassItalic = list.at( 1 ).toBool();
        m_settings.syn_ClassText   = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-func" ).toArray();
        m_settings.syn_FuncWeight = list.at( 0 ).toDouble();
        m_settings.syn_FuncItalic = list.at( 1 ).toBool();
        m_settings.syn_FuncText   = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-quote" ).toArray();
        m_settings.syn_QuoteWeight = list.at( 0 ).toDouble();
        m_settings.syn_QuoteItalic = list.at( 1 ).toBool();
        m_settings.syn_QuoteText   = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-comment" ).toArray();
        m_settings.syn_CommentWeight = list.at( 0 ).toDouble();
        m_settings.syn_CommentItalic = list.at( 1 ).toBool();
        m_settings.syn_CommentText   = colorFromValueString( list.at( 2 ).toString() );

        list = object.value( "syntax-mline" ).toArray();
        m_settings.syn_MLineWeight = list.at( 0 ).toDouble();
        m_settings.syn_MLineItalic = list.at( 1 ).toBool();
        m_settings.syn_MLineText   = colorFromValueString( list.at( 2 ).toString() );
#endif
        // adv find
        m_advancedFindText       = object.value( "advFile-text" ).toString();
        m_advancedFindFileType   = object.value( "advFile-filetype" ).toString();
        m_advancedFindFolder     = object.value( "advFile-folder" ).toString();
        m_advancedFSearchFolders = object.value( "advFile-searchFolders" ).toBool();

        // find list
        list = object.value( "find-list" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_findList.append( list.at( k ).toString() );
        }

        // replace list
        list = object.value( "replace-list" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_replaceList.append( list.at( k ).toString() );
        }

        // macro names
        list = object.value( "macro-names" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_macroNames.append( list.at( k ).toString() );
        }

        // ensure a macro name exists for each macro-id
        QStringList macroIds = json_Load_MacroIds();
        bool modified = false;

        for ( int k = 0; k < macroIds.count() ; ++k )
        {

            if ( ( m_macroNames.count() <= k ) || ( m_macroNames.at( k ).isEmpty() ) )
            {
                QString temp = "Macro Name " + QString::number( k+1 );
                m_macroNames.append( temp );

                modified  = true;
            }
        }

        if ( modified )
        {
            save();
        }

        // preset folders
        list = object.value( "preset-folders" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_preFolderList.append( list.at( k ).toString() );
        }

        // silly way to pad the list
        for ( int k = cnt; k < DiamondLimits::PRESET_FOLDERS_MAX; k++ )
        {
            m_preFolderList.append( "" );
        }

        // recent folders
        list = object.value( "recent-folders" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_rFolderList.append( list.at( k ).toString() );
        }

        // recent files
        list = object.value( "recent-files" ).toArray();
        cnt = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            m_recentFilesList.append( list.at( k ).toString() );
        }

        // opened files
        list = object.value( "opened-files" ).toArray();
        cnt  = list.count();

        for ( int k = 0; k < cnt; k++ )
        {
            QString fname = list.at( k ).toString();

            if ( ! fname.isEmpty() )
            {
                m_openedFiles.append( fname );
                m_openedModified.append( false );
            }
        }
    }

    return ok;
}


QByteArray Settings::json_ReadFile()
{
    QByteArray data;

    QFile file( m_configFileName );

    if ( ! file.open( QFile::ReadWrite | QFile::Text ) )
    {
        const QString msg = tr( "Unable to open Configuration File: " ) +  m_configFileName + " : " + file.errorString();
        csError( tr( "Read Json" ), msg );
        return data;
    }

    file.seek( 0 );
    data = file.readAll();
    file.close();

    return data;
}


void Settings::createThemeArray( QJsonObject &object )
{
    QJsonArray list;
    QJsonArray themeArray;

    // dummy
    list.append( 0 );
    list.append( true );
    list.append( 0 );

    for ( Themes theme : m_themes )
    {
        QJsonObject arrayElement;

        arrayElement.insert( "theme-name", theme.name() );
        arrayElement.insert( "theme-protected", theme.isProtected() );
        arrayElement.insert( "theme-color-text", getRGBString( theme.colorText() ) );
        arrayElement.insert( "theme-color-back", getRGBString( theme.colorBack() ) );
        arrayElement.insert( "theme-color-gutterText", getRGBString( theme.gutterText() ) );
        arrayElement.insert( "theme-color-gutterBack", getRGBString( theme.gutterBack() ) );
        arrayElement.insert( "theme-color-currentLineBack", getRGBString( theme.currentLineBack() ) );

        list.replace( 0, theme.syntaxKey().weight() );
        list.replace( 1, theme.syntaxKey().italic() );
        list.replace( 2, getRGBString( theme.syntaxKey().color() ) );
        arrayElement.insert( "theme-syntax-key", list );

        list.replace( 0, theme.syntaxType().weight() );
        list.replace( 1, theme.syntaxType().italic() );
        list.replace( 2, getRGBString( theme.syntaxType().color() ) );
        arrayElement.insert( "theme-syntax-type", list );

        list.replace( 0, theme.syntaxClass().weight() );
        list.replace( 1, theme.syntaxClass().italic() );
        list.replace( 2, getRGBString( theme.syntaxClass().color() ) );
        arrayElement.insert( "theme-syntax-class", list );

        list.replace( 0, theme.syntaxFunc().weight() );
        list.replace( 1, theme.syntaxFunc().italic() );
        list.replace( 2, getRGBString( theme.syntaxFunc().color() ) );
        arrayElement.insert( "theme-syntax-func", list );

        list.replace( 0, theme.syntaxQuote().weight() );
        list.replace( 1, theme.syntaxQuote().italic() );
        list.replace( 2, getRGBString( theme.syntaxQuote().color() ) );
        arrayElement.insert( "theme-syntax-quote", list );

        list.replace( 0, theme.syntaxComment().weight() );
        list.replace( 1, theme.syntaxComment().italic() );
        list.replace( 2, getRGBString( theme.syntaxComment().color() ) );
        arrayElement.insert( "theme-syntax-comment", list );

        list.replace( 0, theme.syntaxMLine().weight() );
        list.replace( 1, theme.syntaxMLine().italic() );
        list.replace( 2, getRGBString( theme.syntaxMLine().color() ) );
        arrayElement.insert( "theme-syntax-mline", list );

        list.replace( 0, theme.syntaxConstant().weight() );
        list.replace( 1, theme.syntaxConstant().italic() );
        list.replace( 2, getRGBString( theme.syntaxConstant().color() ) );
        arrayElement.insert( "theme-syntax-constant", list );

        themeArray.append( arrayElement );
    }

    object.insert( "themes", themeArray );

}


void Settings::save()
{
    if ( m_flagNoSaveConfig )
    {
        return;
    }


    QJsonObject object;

    object.insert( "advFile-text",          m_advancedFindText );
    object.insert( "advFile-filetype",      m_advancedFindFileType );
    object.insert( "advFile-folder",        m_advancedFindFolder );
    object.insert( "advFile-searchFolders", m_advancedFSearchFolders );
    object.insert( "autoLoad",              autoLoad() );
    object.insert( "pos-x",                 m_lastPosition.x()  );
    object.insert( "pos-y",                 m_lastPosition.y()  );
    object.insert( "size-width",            m_lastSize.width()  );
    object.insert( "size-height",           m_lastSize.height() );
    object.insert( "pos-x",                 m_lastPosition.x()  );
    object.insert( "pos-y",                 m_lastPosition.y()  );
    object.insert( "size-width",            m_lastSize.width()  );
    object.insert( "size-height",           m_lastSize.height() );
    object.insert( "about-url",             aboutUrl() );

    // opened files
    QJsonArray temp = QJsonArray::fromStringList( m_openedFiles );
    object.insert( "opened-files", temp );

    createThemeArray( object );

    object.insert( "column-mode",   m_isColumnMode );
    object.insert( "dictMain",      mainDictionary() );
    object.insert( "dictUser",      userDictionary() );

    temp = QJsonArray::fromStringList( m_findList );
    object.insert( "find-list", temp );

    temp = QJsonArray::fromStringList( m_findList );
    object.insert( "find-list", temp );

    temp = QJsonArray::fromStringList( m_replaceList );
    object.insert( "replace-list", temp );

    QString tempStr = m_fontNormal.toString();
    object.insert( "font-normal", tempStr );

    tempStr = m_fontColumn.toString();
    object.insert( "font-column", tempStr );

    object.insert( "formatDate", formatDate() );
    object.insert( "formatTime", formatTime() );


    // keys
    object.insert( "key-close",             m_options.keys().close() );
    object.insert( "key-columnMode",        m_options.keys().columnMode() );
    object.insert( "key-copy",              m_options.keys().copy() );
    object.insert( "key-copyBuffer",        m_options.keys().copyBuffer() );
    object.insert( "key-cut",               m_options.keys().cut() );
    object.insert( "key-deleteLine",        m_options.keys().deleteLine() );
    object.insert( "key-deleteThroughEOL",  m_options.keys().deleteThroughEOL() );
    object.insert( "key-deleteToEOL",       m_options.keys().deleteToEOL() );
    object.insert( "key-find",              m_options.keys().find() );
    object.insert( "key-findNext",          m_options.keys().findNext() );
    object.insert( "key-findPrev",          m_options.keys().findPrev() );
    object.insert( "key-goBottom",          m_options.keys().goBottom() );
    object.insert( "key-goTop",             m_options.keys().goTop() );
    object.insert( "key-gotoLine",          m_options.keys().gotoLine() );
    object.insert( "key-indentDecrement",   m_options.keys().indentDecrement() );
    object.insert( "key-indentIncrement",   m_options.keys().indentIncrement() );
    object.insert( "key-lower",             m_options.keys().lower() );
    object.insert( "key-macroPlay",         m_options.keys().macroPlay() );
    object.insert( "key-newTab",            m_options.keys().newTab() );
    object.insert( "key-open",              m_options.keys().open() );
    object.insert( "key-paste",             m_options.keys().paste() );
    object.insert( "key-print",             m_options.keys().print() );
    object.insert( "key-printPreview",      m_options.keys().printPreview() );
    object.insert( "key-redo",              m_options.keys().redo() );
    object.insert( "key-reload",            m_options.keys().reload() );
    object.insert( "key-replace",           m_options.keys().replace() );
    object.insert( "key-save",              m_options.keys().save() );
    object.insert( "key-saveAs",            m_options.keys().saveAs() );
    object.insert( "key-selectAll",         m_options.keys().selectAll() );
    object.insert( "key-selectBlock",       m_options.keys().selectBlock() );
    object.insert( "key-selectLine",        m_options.keys().selectLine() );
    object.insert( "key-selectWord",        m_options.keys().selectWord() );
    object.insert( "key-showBreaks",        m_options.keys().showBreaks() );
    object.insert( "key-showSpaces",        m_options.keys().showSpaces() );
    object.insert( "key-spellCheck",        m_options.keys().spellCheck() );
    object.insert( "key-undo",              m_options.keys().undo() );
    object.insert( "key-upper",             m_options.keys().upper() );

    //EDT keys
    object.insert( "key-edt-del-word",      m_options.keys().edtDeleteWord() );
    object.insert( "key-edt-gold",          m_options.keys().edtGold() );
    object.insert( "key-edt-help",          m_options.keys().edtHelp() );
    object.insert( "key-edt-findNext",      m_options.keys().edtFindNext() );
    object.insert( "key-edt-deleteLine",    m_options.keys().edtDeleteLine() );
    object.insert( "key-edt-page",          m_options.keys().edtPage() );
    object.insert( "key-edt-section",       m_options.keys().edtSection() );
    object.insert( "key-edt-append",        m_options.keys().edtAppend() );
    object.insert( "key-edt-del-char",      m_options.keys().edtDeleteChar() );
    object.insert( "key-edt-advance",       m_options.keys().edtDirectionAdvance() );
    object.insert( "key-edt-back",          m_options.keys().edtDirectionBack() );
    object.insert( "key-edt-cut",           m_options.keys().edtCut() );
    object.insert( "key-edt-word",          m_options.keys().edtWord() );
    object.insert( "key-edt-eol",           m_options.keys().edtEOL() );
    object.insert( "key-edt-char",          m_options.keys().edtChar() );
    object.insert( "key-edt-line",          m_options.keys().edtLine() );
    object.insert( "key-edt-select",        m_options.keys().edtSelect() );
    object.insert( "key-edt-enter",         m_options.keys().edtEnter() );
    object.insert( "edt-enabled",           m_options.keys().edtEnabled() );


    object.insert( "pathPrior", m_priorPath );
    object.insert( "pathSyntax", syntaxPath() );

    // TODO:: figure this macro stuff out later.
#if 0
    QList<QKeyEvent *> eventList;
    eventList = m_textEdit->get_MacroKeyList();

    int count = eventList.count();

    if ( count > 0 )
    {
        QVariantList macroList;

        QKeyEvent *event;

        for ( int k = 0; k < count; ++k )
        {
            event = eventList.at( k );

            // hard coded order
            QStringList eventList;
            eventList.append( QString::number( event->key() ) );
            eventList.append( QString::number( event->modifiers() ) );
            eventList.append( event->text() );

            //
            macroList.append( eventList );
        }

        bool ok = true;

        // get next macro name
        QString macroName = object.value( "macro-next" ).toString();

        // next macro id number
        int id = macroName.mid( 9 ).toInteger<int>();

        if ( id > DiamondLimits::MACRO_MAX - 1 )
        {

            QStringList macroIds = json_Load_MacroIds();
            // TODO:: this dialog logic needs to be in main window

            // select macro id to overwrite
            Dialog_Macro *dw = new Dialog_Macro( this, Dialog_Macro::MACRO_SAVE, macroIds, m_macroNames );

            int result = dw->exec();

            if ( result == QDialog::Accepted )
            {
                // over write
                QString text = dw->get_Macro();
                macroName = text;

            }
            else
            {
                // do not save
                ok = false;
            }

            delete dw;

        }
        else
        {
            // save next macro name
            object.insert( "macro-next", "macro-id-" + QString::number( id + 1 ) );

            // save macro_names
            m_macroNames.append( "Macro Name " + QString::number( id + 1 ) );

            QJsonArray temp = QJsonArray::fromStringList( m_macroNames );
            object.insert( "macro-names", temp );
        }

        if ( ok )
        {
            // save macro
            QJsonArray temp = QJsonArray::fromVariantList( macroList );
            object.insert( macroName, temp );
        }
    }

#endif

    temp = QJsonArray::fromStringList( m_macroNames );
    object.insert( "macro-names", temp );

    temp = QJsonArray::fromStringList( m_preFolderList );
    object.insert( "preset-folders", temp );

    temp = QJsonArray::fromStringList( m_rFolderList );
    object.insert( "recent-folders", temp );

    temp = QJsonArray::fromStringList( m_recentFilesList );
    object.insert( "recent-files", temp );

    object.insert( "removeSpace", removeSpaces() );
    object.insert( "rewrapColumn", rewrapColumn() );
    object.insert( "showLineHighlight", m_showLineHighlight );
    object.insert( "showLineNumbers", m_showLineNumbers );
    object.insert( "showSpaces", m_showSpaces );
    object.insert( "showBreaks", m_showBreaks );
    object.insert( "spellcheck", m_isSpellCheck );
    object.insert( "tabSpacing", tabSpacing() );
    object.insert( "useSpaces", useSpaces() );
    object.insert( "word-wrap", m_isWordWrap );


    QJsonDocument doc( object );
    QByteArray data = doc.toJson();

    QString path = pathName( m_configFileName );
    QDir directory( path );

    if ( ! directory.exists() )
    {
        directory.mkpath( path );
    }

    /* Comment by Roland Hughes:
     *
     * The previous codebase took great pains to only change the handfull
     * of fields actually modified via the various dialogs. If this was
     * being written for a system powered by a 9 volt battery I would probably
     * keep that approach. People don't choose JSON for efficiency. It gets
     * chosen because you can change the data with a standard text editor.
     * I come from an OpenVMS background. There we have file versioning. The
     * operating system automatically creates a new version, counting from 1 - 32767.
     * We won't do anything quite that extreme. Seven is usually a lucky number for
     * some people so we will first limit the backup copies; rename the
     * current file to be a backup; then write a shiny new file containing all
     * of the current settings. It will be much more memory manipulation, but
     * should be dramatically less disk I/O because now we don't read the
     * whole file in before each update.
     *
     * This approach also makes changes to the JSON format much easier to implement.
     * When new things are added they will "just appear" in the file. Most importantly
     * when old things get removed, they will be gone in the next version, not
     * hanging around causing confusion and potential problems.
     */
    trimBackups( path );

    QFile file( m_configFileName );

    if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
    {
        const QString msg = tr( "Unable to save Configuration File: " )
                            +  m_configFileName + " : " + file.errorString();
        csError( tr( "Save Json" ), msg );
        return;
    }

    file.write( data );
    file.close();
}

void Settings::trimBackups( QString path )
{
    QDir dir( path );
    QStringList filters;
    QString configFileNameBase = QFileInfo( m_configFileName ).baseName();
    QString wild = configFileNameBase + ".b*";
    filters << wild;

    if ( !QFile::exists( m_configFileName ) )
    { return; }

    QStringList backupFiles = dir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

    // Looks weird but we are deleting the lowest backup numbers
    // which will be at the beginning of the list.
    bool okFlag = true;
    bool deletedFile = false;

    while ( ( backupFiles.size() >= DiamondLimits::BACKUP_FILES_MAX ) && okFlag )
    {
        QString fName = strippedName( backupFiles.takeFirst() );
        okFlag = dir.remove( fName );

        if ( !okFlag )
        {
            QString msg = QString( "Failed to remove " )+  fName;
            csError( tr( "Purging Backups" ), msg );
        }
        else
        {
            deletedFile = true;
        }
    }

    int versionNumber = -1;

    if ( backupFiles.size() > 0 )
    {
        QString lastFile = backupFiles.last();
        QString tmp = suffixName( lastFile ).mid( 1, -1 );
        versionNumber = tmp.toInteger<int>();
    }

    versionNumber++;
    QString namePattern = QFileInfo( m_configFileName ).baseName() + ".b%1";
    QString newName     = namePattern.formatArg( versionNumber, 3, 10, '0' );
    QString oldName     = strippedName( m_configFileName );

    if ( !dir.rename( oldName, newName ) )
    {
        QString msg = QString( "Failed to rename " )
                      + strippedName( m_configFileName )
                      + " to " + newName;
        csError( tr( "Purging Backups" ), msg );
    }

    if ( versionNumber > DiamondLimits::BACKUP_VERSION_MAX )
    {
        versionNumber = 0;
        // refresh the list
        //
        backupFiles = dir.entryList( filters, QDir::Files | QDir::Writable, QDir::Name );

        while ( backupFiles.size() > 0 )
        {
            newName = namePattern.formatArg( versionNumber, 3, 10, '0' );
            // Yes, I know I should check the return value.
            dir.rename( strippedName( backupFiles.takeFirst() ), newName );

            versionNumber++;
        }
    }


}

QStringList Settings::json_Load_MacroIds()
{
    // get existing json data
    QByteArray data = json_ReadFile();

    QJsonDocument doc  = QJsonDocument::fromJson( data );
    QJsonObject object = doc.object();

    //
    QStringList keyList = object.keys();
    QStringList macroList;

    for ( int k = 0; k < keyList.count(); k++ )
    {
        QString key = keyList.at( k );

        if ( key.left( 9 ) == "macro-id-" )
        {
            macroList.append( key );
        }
    }

    return macroList;
}

bool Settings::json_Load_Macro( QString macroName )
{
    bool ok = true;

    // get existing json data
    QByteArray data = json_ReadFile();

    QJsonDocument doc = QJsonDocument::fromJson( data );

    QJsonObject object = doc.object();
    QJsonArray list;

    // macro data
    list = object.value( macroName ).toArray();
    int cnt = list.count();

    //  TODO:: have to decouple this from the GUI.
    //         probably move the logic into mainwindow and
    //         move m_macroList to Settings, providing accessor methods.
#if 0
    m_textEdit->macroStart();
    m_macroList.clear();

    for ( int k = 0; k < cnt; k++ )
    {

        QJsonArray element = list.at( k ).toArray();

        // hard coded order
        int key      = element.at( 0 ).toString().toInteger<int>();
        Qt::KeyboardModifier modifier = Qt::KeyboardModifier( element.at( 1 ).toString().toInteger<int>() );
        QString text = element.at( 2 ).toString();

        QKeyEvent *event = new QKeyEvent( QEvent::KeyPress, key, modifier, text );
        m_macroList.append( event );
    }

    m_textEdit->macroStop();
#endif
    return ok;
}

QList<macroStruct> Settings::json_View_Macro( QString macroName )
{
    QList<macroStruct> retval;

    // get existing json data
    QByteArray data = json_ReadFile();

    QJsonDocument doc = QJsonDocument::fromJson( data );

    QJsonObject object = doc.object();
    QJsonArray list;

    // macro data
    list = object.value( macroName ).toArray();
    int cnt = list.count();

    for ( int k = 0; k < cnt; k++ )
    {

        QJsonArray element = list.at( k ).toArray();

        // hard coded order
        int key      = element.at( 0 ).toString().toInteger<int>();
        Qt::KeyboardModifier modifier = Qt::KeyboardModifier( element.at( 1 ).toString().toInteger<int>() );
        QString text = element.at( 2 ).toString();

        struct macroStruct temp;
        temp.key       = key;
        temp.modifier  = modifier;
        temp.text      = text;

        retval.append( temp );
    }

    return retval;
}


void Settings::json_Save_MacroNames( const QStringList &macroNames )
{
    m_macroNames = macroNames;
    save();
}


bool Settings::openedFilesContains( QString name )
{
    return m_openedFiles.contains( name, Qt::CaseInsensitive );
}

void Settings::createAndLoadNew()
{
    Settings settings;
    settings.save();
    json_ReadFile();
}

int Settings::openedFilesFind( QString name )
{
    return m_openedFiles.indexOf( name );
}

void Settings::openedModifiedReplace( int sub, bool yesNo )
{
    m_openedModified.replace( sub, yesNo );
}

int Settings::findListFind( QString text )
{
    return m_findList.indexOf( text );
}

bool Settings::recentFilesListContains( QString text )
{
    return m_recentFilesList.contains( text );
}

// Create all of the default themes.
//
void Settings::generateDefaultThemes()
{
    if ( m_themes.count() > 0 )
    {
        m_themes.clear();
    }

    /* each line a single string
     * the n, n following syntax color is first Bold then Italic
     *
     * name,
     *        text,
     *        background,
     *        gutter-text,
     *        gutter-back,
     *        current-line-back
     *        key-color, n, n,
     *        type-color, n, n,
     *        class-color, n, n,
     *        func-color, n, n,
     *        quote-color, n, n,
     *        comment-color, n, n,
     *        mline-color, n, n,
     *        constant-color, n, n
     */
    QStringList themeData =
    {
        "CLASSIC, #000000, #FFFFFF, QT::DARKGRAY, 0XD0D0D0, #FFFF99, #0000FF, Y, N, #0000FF, N, N, #800080, N, N, #0000FF, N, N, #008000, N, N, #008000, N, N, #008000, N, N, #FF518C, N, N",
        "SOLARIZEDLIGHT, #657B83, #FDF6E3, #657B83, #E0DBCD, #E0DBCD, #709D06, N, N, #B58900, N, N, #268BD2, N, N, #657B83, N, N, #2AA198, N, N, #93A1A1, N, Y,#93A1A1, N, Y, #FF518C, N, N",
        "GRAYGREY, #000000, #b9c3c5, #000000, #d1ded1, #d7dede, #98061A,  Y, N, #98061A, N, N, #0055FF, N, N, #2E6C33, N, N, #2c7631, N, N, #0000FF, N, Y, #0000FF, N, Y, #FF518C, N, N",
        "JUSTDARKENOUGH, #F4F4F4, #424244, #000000, #E0DBCD, #E0DBCD, #00C2C2, N, N, #C195B4, N, N, #55AAFF, N, N, #00EBAD, N, N, #FFFF00, N, N, #F59F00, N, Y, #F59F00, N, Y, #FF518C, N, N",
        "TANGOLIGHT, #2E3436, #EEEEEC, #2E3436, #E0DBCD, #346604, #346604, N, N, #CE5C00, N, N, #234670, N, N, #A40000, N, N, #5C3566, N, N, #5F615C, N, Y, #5F615C, N, Y, #FF518C, N, N",
        "COBALT, #F8F8F8, #09223F, #888888, #111111, #00162A, #FA9E18, N, N, #FFEF79, N, N, #42D915, N,N, #FFDD00, N, N, #42D915, N, N, #008AFF, N, N, #008AFF, N, N, #FF518C, N, N",
    };

    TextAttributes attr;

    // Note:  This parsing logic ass-u-mes you didn't fat finger anything in the data above.
    //        There is no "user" input here, only programmer error and the programmer 'should'
    //        have at least tested the "happy path."
    //
    for ( QString str : themeData )
    {
        QStringList lst = str.split( "," );
        int fld = 0;

        Themes *theme = new Themes( lst[fld++].trimmed(), true ); // name

        theme->set_colorText( QColor( lst[fld++].trimmed() ) );
        theme->set_colorBack( QColor( lst[fld++].trimmed() ) );
        theme->set_gutterText( QColor( lst[fld++].trimmed() ) );
        theme->set_gutterBack( QColor( lst[fld++].trimmed() ) );
        theme->set_currentLineBack( QColor( lst[fld++].trimmed() ) );

        // key
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxKey( attr );

        // type
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxType( attr );

        // class
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxClass( attr );

        // func
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxFunc( attr );

        // quote
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxQuote( attr );

        // Comment
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxComment( attr );

        // MLine
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );
        qDebug() << "attr MLine: " << attr.color();

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxMLine( attr );

        // Constant
        attr.set_color( QColor( lst[fld++].trimmed() ) );
        attr.set_weight( QFont::Normal );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_weight( QFont::Bold );
        }

        attr.set_italic( false );

        if ( QString::compare( lst[fld++].trimmed(), "y", Qt::CaseInsensitive ) == 0 )
        {
            attr.set_italic( true );
        }

        theme->set_syntaxConstant( attr );

        m_themes[theme->name()] = *theme;

        theme->deleteLater();
    }

    m_activeTheme = "COBALT";
}
