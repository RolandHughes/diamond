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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFont>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QPoint>
#include <QSize>
#include <QTextDocument>
#include "themes.h"
#include "keydefinitions.h"
#include "options.h"
#include "printsettings.h"
#include "macrostruct.h"

class Settings : public QObject
{
    CS_OBJECT( Settings )
public:

    // TODO:: figure out what these were really used for
    //        looks like some dead code
    enum Config { CFG_STARTUP, CFG_DEFAULT };

    Settings();
    Settings( const Settings &other );

    Settings &operator =( const Settings &other );

    void copyTheme( QString name, QString dest );
    void deleteTheme( QString name );
    bool themeNameExists( QString name );
    void generateDefaultThemes();
    void createAndLoadNew();


    bool load( Config trail ); // TODO:: need ability to check for old version and convert it
    void save();

    //
    // getters
    //   first group from Options dialog
    //
    Options copyOfOptions()              { return m_options;}
    PrintSettings copyOfPrintSettings()  { return m_printSettings;}
    QStringList copyOfPreFolderList()   { return m_preFolderList;}

    int     rewrapColumn()          { return m_options.rewrapColumn();}
    int     tabSpacing()            { return m_options.tabSpacing();}
    bool    useSpaces()             { return m_options.useSpaces();}
    bool    removeSpaces()          { return m_options.removeSpaces();}
    bool    autoLoad()              { return m_options.autoLoad();}
    QString formatDate()            { return m_options.formatDate();}
    QString formatTime()            { return m_options.formatTime();}
    QString mainDictionary()        { return m_options.mainDictionary();}
    QString userDictionary()        { return m_options.userDictionary();}
    QString aboutUrl()              { return m_options.aboutUrl();}
    QString syntaxPath()            { return m_options.syntaxPath();}

    KeyDefinitions &keys()          { return m_options.keys();}

    bool    showLineHighlight()     { return m_showLineHighlight;}
    bool    showLineNumbers()       { return m_showLineNumbers;}
    bool    isColumnMode()          { return m_isColumnMode;}
    bool    isSpellCheck()          { return m_isSpellCheck;}
    bool    isWordWrap()            { return m_isWordWrap;}
    bool    showSpaces()            { return m_showSpaces;}
    bool    showBreaks()            { return m_showBreaks;}
    bool    flagNoAutoLoad()        { return m_flagNoAutoLoad;}
    bool    flagNoSaveConfig()      { return m_flagNoSaveConfig;}
    bool    isComplete()            { return m_isComplete;}

    QString priorPath()             { return m_priorPath;}
    QString activeTheme()           { return m_activeTheme;}

    QFont   fontNormal()            { return m_fontNormal;}
    QFont   fontColumn()            { return m_fontColumn;}

    Themes  &currentTheme()         { return m_themes[m_activeTheme];}
    QStringList availableThemes()   { return m_themes.keys();}

    QStringList &findList()         { return m_findList;}
    QStringList &replaceList()      { return m_replaceList;}
    QStringList &macroNames()       { return m_macroNames;}
    QStringList &recentFiles()      { return m_recentFilesList;}
    QStringList &recentFolders()    { return m_rFolderList;}
    QStringList &presetFolders()    { return m_preFolderList;}

    QSize   lastSize()              { return m_lastSize;}
    QPoint  lastPosition()          { return m_lastPosition;}

    QString openedFiles( int sub )      { return m_openedFiles[sub];}
    int     openedFilesCount()          { return m_openedFiles.count();}
    bool    openedModified( int sub )   { return m_openedModified[sub];}
    bool    openedFilesContains( QString name );
    int     openedFilesFind( QString name );
    void    openedModifiedReplace( int sub, bool yesNo );
    int     findListFind( QString text );
    bool    recentFilesListContains( QString text );

    QString configFileName()        { return m_configFileName;}

    QString advancedFindText()      { return m_advancedFindText;}
    QString advancedFindFileType()  { return m_advancedFindFileType;}
    QString advancedFindFolder()    { return m_advancedFindFolder;}
    bool advancedFSearchFolders() { return m_advancedFSearchFolders;}
    bool advancedFCase()            { return m_advancedFCase;}
    bool advancedFWholeWords()      { return m_advancedFWholeWords;}
    QTextDocument::FindFlags findFlags()  { return m_findFlags;}
    QString findText()              { return m_findText;}
    QString replaceText()           { return m_replaceText;}
    bool findWholeWords()           { return m_findWholeWords;}
    bool findDirection()            { return m_findDirection;}
    bool findCase()                 { return m_findCase;}
    int  replaceListFind( QString text ) { return m_replaceList.indexOf( text );}

    //
    // setters
    //
    void set_options( const Options &opt );
    void set_printSettings( const PrintSettings &prt );

    void set_preFolderList( QStringList lst )  { m_preFolderList = lst;}
    void set_showLineHighlight( bool yesNo )   { m_showLineHighlight = yesNo;}
    void set_showLineNumbers( bool yesNo )     { m_showLineNumbers = yesNo;}
    void set_isColumnMode( bool yesNo )        { m_isColumnMode = yesNo;}
    void set_isSpellCheck( bool yesNo )        { m_isSpellCheck = yesNo;}
    void set_isWordWrap( bool yesNo )          { m_isWordWrap = yesNo;}
    void set_showSpaces( bool yesNo )          { m_showSpaces = yesNo;}
    void set_showBreaks( bool yesNo )          { m_showBreaks = yesNo;}
    void set_appPath( QString path )           { m_appPath = path;}

    void set_priorPath( const QString &priorPath )        { m_priorPath = priorPath;}
    void set_activeTheme( const QString &name )       { m_activeTheme = name;}

    void set_fontNormal( const QFont &font )         { m_fontNormal = font;}
    void set_fontColumn( const QFont &font )         { m_fontColumn = font;}

    void set_tabSpacing( int spacing )         { m_options.set_tabSpacing( spacing );}
    void set_rewrapColumn( int col )           { m_options.set_rewrapColumn( col );}
    void set_formatDate( const QString &fmt )         { m_options.set_formatDate( fmt );}
    void set_formatTime( const QString &fmt )         { m_options.set_formatTime( fmt );}
    void set_useSpaces( bool yesNo )           { m_options.set_useSpaces( yesNo );}

    void set_flagNoAutoLoad( bool yesNo )      { m_flagNoAutoLoad = yesNo;}
    void set_flagNoSaveConfig( bool yesNo )    { m_flagNoSaveConfig = yesNo;}

    void set_configFileName( QString name )   { m_configFileName = name;}
    void set_findList( QStringList list )     { m_findList = list; }
    void set_advancedFindText( QString text ) { m_advancedFindText = text;}
    void set_advancedFindFileType( QString text ) { m_advancedFindFileType = text;}
    void set_advancedFindFolder( QString text )   { m_advancedFindFolder = text;}
    void set_advancedFCase( bool yesNo )          { m_advancedFCase = yesNo;}
    void set_advancedFWholeWords( bool yesNo )    { m_advancedFWholeWords = yesNo; }
    void set_advancedFSearchFolders( bool yesNo ) { m_advancedFSearchFolders = yesNo;}
    void set_findText( QString text )             { m_findText = text;}
    void set_findFlags( QTextDocument::FindFlags flags ) {m_findFlags = flags;}
    void set_replaceText( QString text )          { m_replaceText = text;}
    void set_findWholeWords( bool yesNo )         { m_findWholeWords = yesNo;}
    void set_findDirection( bool yesNo )          { m_findDirection = yesNo;}
    void set_findFlagsBackward()                 { m_findFlags |= QTextDocument::FindBackward;}
    void set_findFlagsCaseSensitive()            { m_findFlags |= QTextDocument::FindCaseSensitively;}
    void set_findFlagsWholeWords()               { m_findFlags |= QTextDocument::FindWholeWords;}
    void set_findCase( bool yesNo )               { m_findCase = yesNo;}
    void set_replaceList( QStringList lst )       { m_replaceList = lst;}

    // TODO:: Need to auto-store on each value change
    //
    void openedFilesClear()                  { m_openedFiles.clear();}
    void openedModifiedClear()               { m_openedModified.clear();}
    void recentFileListClear()               { m_recentFilesList.clear();}
    void openedFilesAppend( QString curFile )  { m_openedFiles.append( curFile );}
    void openedModifiedAppend( bool yesNo )    { m_openedModified.append( yesNo );}
    void openedFilesRemove( QString name )     { m_openedFiles.removeOne( name );}
    void findListPrepend( QString text )       { m_findList.prepend( text );}
    void findListMove( int index, int dest )   { m_findList.move( index, dest );}
    void replaceListPrepend( QString text )    { m_replaceList.prepend( text );}
    void replaceListMove( int index, int dest ) { m_replaceList.move( index, dest );}

    // json
    void json_Save_MacroNames( const QStringList &macroNames );
    bool json_Read( Config trail = CFG_DEFAULT );
    QByteArray json_ReadFile();
    QList<macroStruct> json_View_Macro( QString macroName );
    QStringList json_Load_MacroIds();
    bool json_Load_Macro( QString macroName );

    void set_lastSize( QSize size )               { m_lastSize = size;}
    void set_lastPosition( QPoint pos )           { m_lastPosition = pos;}

    //
    // signals
    //
    CS_SIGNAL_1( Public, void Move( QPoint pos ) )
    CS_SIGNAL_2( Move, pos )

    CS_SIGNAL_1( Public, void Resize( QSize size ) )
    CS_SIGNAL_2( Resize, size )

private:
    void trimBackups(QString path);
    void createThemeArray( QJsonObject& object );

    bool m_advancedFCase;
    bool m_advancedFSearchFolders;
    bool m_advancedFWholeWords;
    bool m_findCase;
    bool m_findDirection;
    bool m_findWholeWords;
    bool m_flagNoAutoLoad;        // command line param - do not save
    bool m_flagNoSaveConfig;      // command line param - do not save
    bool m_isColumnMode;
    bool m_isComplete;
    bool m_isSpellCheck;
    bool m_isWordWrap;
    bool m_showBreaks;
    bool m_showLineHighlight;
    bool m_showLineNumbers;
    bool m_showSpaces;

    QString  m_activeTheme;
    QString  m_advancedFindFileType;
    QString  m_advancedFindFolder;
    QString  m_advancedFindText;
    QString  m_appPath;
    QString  m_configFileName;
    QString  m_findText;
    QString  m_priorPath;
    QString  m_replaceText;

    QStringList  m_findList;
    QStringList  m_macroNames;
    QStringList  m_openedFiles;
    QStringList  m_preFolderList;
    QStringList  m_rFolderList;
    QStringList  m_replaceList;
    QStringList  m_recentFilesList;

    QFont    m_fontColumn;
    QFont    m_fontNormal;
    QPoint   m_lastPosition;
    QSize    m_lastSize;

    Options  m_options;

    PrintSettings m_printSettings;

    QMap <QString, Themes> m_themes;

    QList<bool> m_openedModified;

    QTextDocument::FindFlags m_findFlags;

};

#endif
