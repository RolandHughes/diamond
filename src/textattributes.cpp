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

#include "textattributes.h"
#include <QFont>

TextAttributes::TextAttributes() :
    m_weight(QFont::Normal)
    ,m_italic(false)
{
}

TextAttributes::TextAttributes(const QColor color, int weight, bool yesNo) :
    m_color(color)
    ,m_weight(weight)
    ,m_italic(yesNo)
{
}

TextAttributes::TextAttributes(const TextAttributes& attr) :
    m_color(attr.m_color)
    ,m_weight(attr.m_weight)
    ,m_italic(attr.m_italic)
{
}

TextAttributes& TextAttributes::operator=(const TextAttributes& attr)
{
    if (this != &attr)
    {
        m_color     = attr.m_color;
        m_weight    = attr.m_weight;
        m_italic    = attr.m_italic;
    }
    return *this;
}
