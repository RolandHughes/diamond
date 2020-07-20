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

#include "themes.h"

Themes::Themes() :
    m_protected(false)
{
}

Themes::Themes(const QString name, bool isProtected) :
    m_protected(isProtected)
    , m_name(name)
{
}


// never copy protected status
//
Themes::Themes(const Themes& theme) :
    m_name(theme.m_name)
    , m_colorText(theme.m_colorText)
    , m_colorBack(theme.m_colorBack)
    , m_gutterText(theme.m_gutterText)
    , m_gutterBack(theme.m_gutterBack)
    , m_currentLineBack(theme.m_currentLineBack)
    , m_synKey(theme.m_synKey)
    , m_synType(theme.m_synType)
    , m_synClass(theme.m_synClass)
    , m_synFunc(theme.m_synFunc)
    , m_synQuote(theme.m_synQuote)
    , m_synComment(theme.m_synComment)
    , m_synMLine(theme.m_synMLine)
    , m_synConstant(theme.m_synConstant)
{
}

// never copy protected status
//
Themes& Themes::operator=(const Themes& theme)
{
    if (this != &theme)
    {
        m_protected         = false;
        m_name              = theme.m_name;
        m_colorText         = theme.m_colorText;
        m_colorBack         = theme.m_colorBack;
        m_gutterText        = theme.m_gutterText;
        m_gutterBack        = theme.m_gutterBack;
        m_currentLineBack   = theme.m_currentLineBack;
        m_synKey            = theme.m_synKey;
        m_synType           = theme.m_synType;
        m_synClass          = theme.m_synClass;
        m_synFunc           = theme.m_synFunc;
        m_synQuote          = theme.m_synQuote;
        m_synComment        = theme.m_synComment;
        m_synMLine          = theme.m_synMLine;
        m_synConstant       = theme.m_synConstant;
    }

    return *this;
}
