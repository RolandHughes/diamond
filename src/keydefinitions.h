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

#ifndef KEYDEFINITIONS_H
#define KEYDEFINITIONS_H

#include <QString>

class KeyDefinitions
{
public:
    KeyDefinitions();
    KeyDefinitions( const KeyDefinitions &def );

    KeyDefinitions &operator =( const KeyDefinitions &def );
    
    friend bool operator ==( const KeyDefinitions &left, const KeyDefinitions &right );
    friend bool operator !=( const KeyDefinitions &left, const KeyDefinitions &right );

    void setDefaultKeyValues();
    void platformAdjustKeyValues();

    //
    // getters
    //
    QString close()                 { return m_close;}
    QString columnMode()            { return m_columnMode;}
    QString copy()                  { return m_copy;}
    QString copyBuffer()            { return m_copyBuffer;}
    QString cut()                   { return m_cut;}
    QString deleteLine()            { return m_deleteLine;}
    QString deleteThroughEOL()      { return m_deleteThroughEOL;}
    QString deleteToEOL()           { return m_deleteToEOL;}
    QString find()                  { return m_find;}
    QString findNext()              { return m_findNext;}
    QString findPrev()              { return m_findPrev;}
    QString goBottom()              { return m_goBottom;}
    QString goTop()                 { return m_goTop;}
    QString gotoLine()              { return m_gotoLine;}
    QString indentDecrement()       { return m_indentDecrement;}
    QString indentIncrement()       { return m_indentIncrement;}
    QString lower()                 { return m_lower;}
    QString macroPlay()             { return m_macroPlay;}
    QString newTab()                { return m_newTab;}
    QString open()                  { return m_open;}
    QString paste()                 { return m_paste;}
    QString print()                 { return m_print;}
    QString printPreview()          { return m_printPreview;}
    QString redo()                  { return m_redo;}
    QString reload()                { return m_reload;}
    QString replace()               { return m_replace;}
    QString save()                  { return m_save;}
    QString saveAs()                { return m_saveAs;}
    QString selectAll()             { return m_selectAll;}
    QString selectBlock()           { return m_selectBlock;}
    QString selectLine()            { return m_selectLine;}
    QString selectWord()            { return m_selectWord;}
    QString showBreaks()            { return m_showBreaks;}
    QString showSpaces()            { return m_showSpaces;}
    QString spellCheck()            { return m_spellCheck;}
    QString undo()                  { return m_undo;}
    QString upper()                 { return m_upper;}

    QString edtDeleteWord()         { return m_edtDeleteWord;}
    QString edtGold()               { return m_edtGold;}
    QString edtHelp()               { return m_edtHelp;}
    QString edtFindNext()           { return m_edtFindNext;}
    QString edtDeleteLine()         { return m_edtDeleteLine;}
    QString edtPage()               { return m_edtPage;}
    QString edtSection()            { return m_edtSection;}
    QString edtAppend()             { return m_edtAppend;}
    QString edtDeleteChar()         { return m_edtDeleteChar;}
    QString edtDirectionAdvance()   { return m_edtAdvance;}
    QString edtDirectionBack()      { return m_edtBackup;}
    QString edtCut()                { return m_edtCut;}
    QString edtWord()               { return m_edtWord;}
    QString edtEOL()                { return m_edtEOL;}
    QString edtChar()               { return m_edtChar;}
    QString edtLine()               { return m_edtLine;}
    QString edtSelect()             { return m_edtSelect;}
    QString edtEnter()              { return m_edtEnter;}
    QString edtGotoLine()           { return m_edtGotoLine;}

    bool    edtEnabled()            { return m_edtEnabled;}



    //
    // Setters
    //
    void set_close( QString keyValue )             { m_close = keyValue;}
    void set_columnMode( QString keyValue )        { m_columnMode = keyValue;}
    void set_copy( QString keyValue )              { m_copy = keyValue;}
    void set_copyBuffer( QString keyValue )        { m_copyBuffer = keyValue;}
    void set_cut( QString keyValue )               { m_cut = keyValue;}
    void set_deleteLine( QString keyValue )        { m_deleteLine = keyValue;}
    void set_deleteThroughEOL( QString keyValue )  { m_deleteThroughEOL = keyValue;}
    void set_deleteToEOL( QString keyValue )       { m_deleteToEOL = keyValue;}
    void set_find( QString keyValue )              { m_find = keyValue;}
    void set_findNext( QString keyValue )          { m_findNext = keyValue;}
    void set_findPrev( QString keyValue )          { m_findPrev = keyValue;}
    void set_goBottom( QString keyValue )          { m_goBottom = keyValue;}
    void set_goTop( QString keyValue )             { m_goTop = keyValue;}
    void set_gotoLine( QString keyValue )          { m_gotoLine = keyValue;}
    void set_indentDecrement( QString keyValue )   { m_indentDecrement = keyValue;}
    void set_indentIncrement( QString keyValue )   { m_indentIncrement = keyValue;}
    void set_lower( QString keyValue )             { m_lower = keyValue;}
    void set_macroPlay( QString keyValue )         { m_macroPlay = keyValue;}
    void set_newTab( QString keyValue )            { m_newTab = keyValue;}
    void set_open( QString keyValue )              { m_open = keyValue;}
    void set_paste( QString keyValue )             { m_paste = keyValue;}
    void set_print( QString keyValue )             { m_print = keyValue;}
    void set_printPreview( QString keyValue )      { m_printPreview = keyValue;}
    void set_redo( QString keyValue )              { m_redo = keyValue;}
    void set_reload( QString keyValue )            { m_reload = keyValue;}
    void set_replace( QString keyValue )           { m_replace = keyValue;}
    void set_save( QString keyValue )              { m_save = keyValue;}
    void set_saveAs( QString keyValue )            { m_saveAs = keyValue;}
    void set_selectAll( QString keyValue )         { m_selectAll = keyValue;}
    void set_selectBlock( QString keyValue )       { m_selectBlock = keyValue;}
    void set_selectLine( QString keyValue )        { m_selectLine = keyValue;}
    void set_selectWord( QString keyValue )        { m_selectWord = keyValue;}
    void set_showBreaks( QString keyValue )        { m_showBreaks = keyValue;}
    void set_showSpaces( QString keyValue )        { m_showSpaces = keyValue;}
    void set_spellCheck( QString keyValue )        { m_spellCheck = keyValue;}
    void set_undo( QString keyValue )              { m_undo = keyValue;}
    void set_upper( QString keyValue )             { m_upper = keyValue;}

    void set_edtDeleteWord( QString txt )          { m_edtDeleteWord = txt;}
    void set_edtGold( QString txt )                { m_edtGold = txt;}
    void set_edtHelp( QString txt )                { m_edtHelp = txt;}
    void set_edtFindNext( QString txt )            { m_edtFindNext = txt;}
    void set_edtDeleteLine( QString txt )          { m_edtDeleteLine = txt;}
    void set_edtPage( QString txt )                { m_edtPage = txt;}
    void set_edtSection( QString txt )             { m_edtSection = txt;}
    void set_edtAppend( QString txt )              { m_edtAppend = txt;}
    void set_edtDeleteChar( QString txt )          { m_edtDeleteChar = txt;}
    void set_edtDirectionAdvance( QString txt )    { m_edtAdvance = txt;}
    void set_edtDirectionBack( QString txt )       { m_edtBackup = txt;}
    void set_edtCut( QString txt )                 { m_edtCut = txt;}
    void set_edtWord( QString txt )                { m_edtWord = txt;}
    void set_edtEOL( QString txt )                 { m_edtEOL = txt;}
    void set_edtChar( QString txt )                { m_edtChar = txt;}
    void set_edtLine( QString txt )                { m_edtLine = txt;}
    void set_edtSelect( QString txt )              { m_edtSelect = txt;}
    void set_edtEnter( QString txt )               { m_edtEnter = txt;}
    void set_edtEnabled( bool yesNo )              { m_edtEnabled = yesNo;}
    void set_edtGotoLine( QString txt )            { m_edtGotoLine = txt;}

private:
    QString adjustKey( QString sequence );

    QString m_close;
    QString m_columnMode;
    QString m_copy;
    QString m_copyBuffer;
    QString m_cut;
    QString m_deleteLine;
    QString m_deleteThroughEOL;
    QString m_deleteToEOL;
    QString m_find;
    QString m_findNext;
    QString m_findPrev;
    QString m_goBottom;
    QString m_goTop;
    QString m_gotoLine;
    QString m_indentDecrement;
    QString m_indentIncrement;
    QString m_lower;
    QString m_macroPlay;
    QString m_newTab;
    QString m_open;
    QString m_paste;
    QString m_print;
    QString m_printPreview;
    QString m_redo;
    QString m_reload;
    QString m_replace;
    QString m_save;
    QString m_saveAs;
    QString m_selectAll;
    QString m_selectBlock;
    QString m_selectLine;
    QString m_selectWord;
    QString m_showBreaks;
    QString m_showSpaces;
    QString m_spellCheck;
    QString m_undo;
    QString m_upper;

    /*
     * EDT - unlike the PC world where you have to press Ctrl+L at the same time to
     *       do whatever that key sequence is mapped to, EDT uses individual keystrokes
     *       [PF3] is normally mapped to the [*] on the numeric keypad. Pressing just
     *       it is "find next." Pressing GOLD and releasing it just in front of [*]
     *       causes the "find" text prompt to happen. EDT is direction sensitive.
     *       [4] is advance  [5] is backup. These remain in effect. "find next"
     *       travels in the direction currently in effect.
     */
    bool    m_edtEnabled;       // only look for EDT keypad support when enabled.
    QString m_edtDeleteWord;    // this one is a different key sequence becuse the PC numeric
    // keypad is missing one key. GOLD del_word = undelete word

    QString m_edtGold;
    QString m_edtHelp;          // has no GOLD option
    QString m_edtFindNext;      // GOLD find = find
    QString m_edtDeleteLine;    // GOLD deleteLine = undeleteLine
    QString m_edtPage;          // GOLD page = command
    QString m_edtSection;       // GOLD sect = fill
    QString m_edtAppend;        // GOLD append = replace
    QString m_edtDeleteChar;    // GOLD del_char = undelete char
    QString m_edtAdvance;       // GOLD advance = goto bottom
    QString m_edtBackup;        // GOLD backup = goto top
    QString m_edtCut;           // GOLD cut = paste the last cut
    QString m_edtWord;          // GOLD word = change case
    QString m_edtEOL;           // GOLD eol = delete to end of line leaving newline char(s)
    QString m_edtChar;          // GOLD char = special insert
    QString m_edtLine;          // GOLD line = open line
    QString m_edtSelect;        // GOLD select = reset
    QString m_edtEnter;         // GOLD enter = substitute
    QString m_edtGotoLine;      // GOLD =

};

#endif
