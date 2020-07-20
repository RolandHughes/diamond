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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QMap>

#include "keydefinitions.h"

//  Class used to exchange information with Options dialog
//  All of these values come from the Settings class.
//  TODO::
class Options
{
public:
    Options();
    Options( const Options &opt );

    Options &operator = ( const Options &opt );
    const QString STANDARD_KEYS = "STANDARD";
    const QString EDT_KEYS = "EDT";
    //
    // getters
    //
    int             rewrapColumn()      { return m_rewrapColumn;}
    int             tabSpacing()        { return m_tabSpacing;}
    bool            useSpaces()         { return m_useSpaces;}
    bool            removeSpaces()      { return m_removeSpaces;}
    bool            autoLoad()          { return m_autoLoad;}
    bool            isAutoDetect()      { return m_autoDetect;}
    QString         formatDate()        { return m_formatDate;}
    QString         formatTime()        { return m_formatTime;}
    QString         mainDictionary()    { return m_mainDictionary;}
    QString         userDictionary()    { return m_userDictionary;}
    QString         syntaxPath()        { return m_syntaxPath;}
    QString         aboutUrl()          { return m_aboutUrl;}

    KeyDefinitions &keys()              { return m_keys;}

    //
    // setters
    //
    void set_rewrapColumn( int col )              { m_rewrapColumn = col;}
    void set_tabSpacing( int tabStop )            { m_tabSpacing = tabStop;}
    void set_useSpaces( bool yesNo )              { m_useSpaces = yesNo;}
    void set_removeSpaces( bool yesNo )           { m_removeSpaces = yesNo;}
    void set_autoLoad( bool yesNo )               { m_autoLoad = yesNo;}
    void set_formatDate( QString fmt )            { m_formatDate = fmt;}
    void set_formatTime( QString fmt )            { m_formatTime = fmt;}
    void set_mainDictionary( QString dictionary ) { m_mainDictionary = dictionary;}
    void set_userDictionary( QString dictionary ) { m_userDictionary = dictionary;}
    void set_syntaxPath( QString path )           { m_syntaxPath = path;}
    void set_abouturl( QString url )              { m_aboutUrl = url;}


private:
    int m_rewrapColumn;
    /*
       TODO:: tabSpacing needs to be implemented more like in Emacs.
     ;;
     ;;  Fix the tab-stop list to every 4 spaces for the first 200
     ;;
     (setq tab-stop-list (number-sequence 0 200 4))
     ;;
     ;;  Also allows for an actual ascending list so if you need a COBOL
     ;;  set of tab stops where comment is in 7; AREA-B starts in 8; 73 is
     ;;  additional comment to 80.
     ;;  See: https://www.logikalsolutions.com/wordpress/information-technology/most-text-editors-get-tabs-wrong/
    */
    int m_tabSpacing;

    bool m_useSpaces;
    bool m_removeSpaces;
    bool m_autoLoad;
    bool m_autoDetect;      // do not save this to file - runtime only

    QString m_formatDate;
    QString m_formatTime;
    QString m_mainDictionary;
    QString m_userDictionary;
    QString m_syntaxPath;
    QString m_aboutUrl;

    KeyDefinitions m_keys;
};

// TODO:: look up Enum Class - maybe put this Enum inside of Options class
//
enum SyntaxTypes {SYN_C, SYN_CLIPPER, SYN_CMAKE, SYN_CSS, SYN_DOXY, SYN_ERRLOG, SYN_HTML,
                  SYN_JAVA, SYN_JS, SYN_JSON, SYN_MAKE, SYN_NSIS, SYN_TEXT,
                  SYN_SHELL, SYN_PERL, SYN_PHP, SYN_PYTHON, SYN_XML,
                  SYN_NONE, SYN_UNUSED1, SYN_UNUSED2
                 };

#endif
