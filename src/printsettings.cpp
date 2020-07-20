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
#include "printsettings.h"

PrintSettings::PrintSettings() :
    m_lineNumbers(true)
    , m_printHeader(true)
    , m_printFooter(true)
{
    m_headerLeft    = "$(PathFileName)";
    m_headerCenter  = "";
    m_headerRight   =  "Page $(PageNo)";
    m_headerLine2   = "";
    m_footerLeft    = "$(Date)";
    m_footerCenter  = "";
    m_footerRight   = "Diamond Editor";
    m_footerLine2   = "";
    m_marginTop     = 0.75;
    m_marginBottom  = 0.75;
    m_marginLeft    = 0.50;
    m_marginRight   = 0.50;
    m_headerGap     = 0.25;
    m_fontHeader    = QFont("Monospace", 10);
    m_fontFooter    = QFont("Monospace", 10);
    m_fontText      = QFont("Monospace", 12);
}

PrintSettings::PrintSettings(const PrintSettings& other) :
    m_lineNumbers(other.m_lineNumbers)
    , m_printHeader(other.m_printHeader)
    , m_printFooter(other.m_printFooter)
    , m_headerLeft(other.m_headerLeft)
    , m_headerCenter(other.m_headerCenter)
    , m_headerRight(other.m_headerRight)
    , m_headerLine2(other.m_headerLine2)
    , m_footerLeft(other.m_footerLeft)
    , m_footerCenter(other.m_footerCenter)
    , m_footerRight(other.m_footerRight)
    , m_footerLine2(other.m_footerLine2)
    , m_marginLeft(other.m_marginLeft)
    , m_marginTop(other.m_marginTop)
    , m_marginRight(other.m_marginRight)
    , m_marginBottom(other.m_marginBottom)
    , m_headerGap(other.m_headerGap)
    , m_fontHeader(other.m_fontHeader)
    , m_fontFooter(other.m_fontFooter)
    , m_fontText(other.m_fontText)
{
}

PrintSettings& PrintSettings::operator =(const PrintSettings& other)
{
    if (this != &other)
    {
        m_lineNumbers       = other.m_lineNumbers;
        m_printHeader       = other.m_printHeader;
        m_printFooter       = other.m_printFooter;
        m_headerLeft        = other.m_headerLeft;
        m_headerCenter      = other.m_headerCenter;
        m_headerRight       = other.m_headerRight;
        m_headerLine2       = other.m_headerLine2;
        m_footerLeft        = other.m_footerLeft;
        m_footerCenter      = other.m_footerCenter;
        m_footerRight       = other.m_footerRight;
        m_footerLine2       = other.m_footerLine2;
        m_marginLeft        = other.m_marginLeft;
        m_marginTop         = other.m_marginTop;
        m_marginRight       = other.m_marginRight;
        m_marginBottom      = other.m_marginBottom;
        m_headerGap         = other.m_headerGap;
        m_fontHeader        = other.m_fontHeader;
        m_fontFooter        = other.m_fontFooter;
        m_fontText          = other.m_fontText;
    }

    return *this;
}
