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

#ifndef TEXTATTRIBUTES_H
#define TEXTATTRIBUTES_H

#include <QColor>

class TextAttributes : public QObject
{
    CS_OBJECT( TextAttributes )
public:
    TextAttributes();
    TextAttributes( const QColor color, int weight, bool yesNo );
    TextAttributes( const TextAttributes &attr );

    TextAttributes &operator = ( const TextAttributes &attr );

    //
    // getters
    //
    QColor color()              { return m_color;}
    int    weight()             { return m_weight;}
    bool   italic()             { return m_italic;}

    //
    // setters
    //
    void set_color( QColor color )    { m_color = color;}
    void set_weight( int weight )     { m_weight = weight;}
    void set_italic( bool yesNo )     { m_italic = yesNo;}

private:
    QColor  m_color;
    int     m_weight;
    bool    m_italic;
};

#endif
