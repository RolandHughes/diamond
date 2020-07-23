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
    Settings();
    Settings( const Settings &other );

    Settings &operator =( const Settings &other );

    friend bool operator ==( const Settings &left, const Settings &right );
    friend bool operator !=( const Settings &left, const Settings &right );
    friend class Overlord;

    void copyTheme( QString name, QString dest );
    void deleteTheme( QString name );
    bool themeNameExists( QString name );

    // had to leave these getters and setters in place because
    // colors and options dialogs need to work with local copies
    // until change is saved.
    //
    // getters
    //
    QString activeTheme()           { return m_activeTheme;}
    Themes  &currentTheme()         { return m_themes[m_activeTheme];}
    QString syntaxPath()            { return m_options.syntaxPath();}
    QStringList availableThemes()   { return m_themes.keys();}
    QFont   fontColumn()            { return m_fontColumn;}
    QFont   fontNormal()            { return m_fontNormal;}
    bool    showLineNumbers()       { return m_showLineNumbers;}
    bool    isColumnMode()          { return m_isColumnMode;}
    QString mainDictionary()        { return m_options.mainDictionary();}
    QString userDictionary()        { return m_options.userDictionary();}
    int     tabSpacing()            { return m_options.tabSpacing();}
    bool    useSpaces()             { return m_options.useSpaces();}
    QString formatDate()            { return m_options.formatDate();}
    QString formatTime()            { return m_options.formatTime();}
    int     rewrapColumn()          { return m_options.rewrapColumn();}
    bool    showLineHighlight()     { return m_showLineHighlight;}

    //
    // setters
    //
    void set_activeTheme( const QString &name )     { m_activeTheme = name;}


private:
    void generateDefaultThemes();
    //void createAndLoadNew();

    bool    openedFilesContains( QString name );
    int     openedFilesFind( QString name );
    void    openedModifiedReplace( int sub, bool yesNo );
    int     findListFind( QString text );
    bool    recentFilesListContains( QString text );

    void openedFilesClear()                     { m_openedFiles.clear();}
    void openedModifiedClear()                  { m_openedModified.clear();}
    void recentFileListClear()                  { m_recentFilesList.clear();}
    void openedFilesAppend( QString curFile )   { m_openedFiles.append( curFile );}
    void openedModifiedAppend( bool yesNo )     { m_openedModified.append( yesNo );}
    void openedFilesRemove( QString name )      { m_openedFiles.removeOne( name );}
    void findListPrepend( QString text )        { m_findList.prepend( text );}
    void findListMove( int index, int dest )    { m_findList.move( index, dest );}
    void replaceListPrepend( QString text )     { m_replaceList.prepend( text );}
    void replaceListMove( int index, int dest ) { m_replaceList.move( index, dest );}

    // json
    void json_Save_MacroNames( const QStringList &macroNames );
    bool json_Read();
    QByteArray json_ReadFile();
    QList<macroStruct> json_View_Macro( QString macroName );
    QStringList json_Load_MacroIds();
    bool json_Load_Macro( QString macroName );

    void set_lastSize( QSize size )             { m_lastSize = size;}
    void set_lastPosition( QPoint pos )         { m_lastPosition = pos;}

    //
    // signals
    //
    CS_SIGNAL_1( Public, void Move( QPoint pos ) )
    CS_SIGNAL_2( Move, pos )

    CS_SIGNAL_1( Public, void Resize( QSize size ) )
    CS_SIGNAL_2( Resize, size )

    void trimBackups( QString path );
    void createThemeArray( QJsonObject &object );

    bool load(); // TODO:: need ability to check for old version and convert it
    void save();


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
