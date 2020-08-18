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

#include "edt_lineedit.h"
#include <QEvent>
#include <QKeyEvent>

Edt_LineEdit::Edt_LineEdit( QWidget *parent ) :
    QLineEdit( parent )
    , m_terminator( NONE )
    , m_ctrlMSubstitution( false )
    , m_allowDirection(false)
{
}

void Edt_LineEdit::keyPressEvent( QEvent *ev ) 
{
    if ( ev->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyPressEvent = dynamic_cast<QKeyEvent *>( ev );

        int key = keyPressEvent->key();
        int modifiers = keyPressEvent->modifiers();
        bool isKeypad = ( modifiers & Qt::KeypadModifier );

        switch ( key )
        {
            case Qt::Key_4:
            case Qt::Key_Left:
                if ( isKeypad  && m_allowDirection)
                {
                    qDebug() << "caught 4 key";
                    ev->accept();
                    m_terminator = ADVANCE;
                    inputComplete();
                }

                break;

            case Qt::Key_5:
            case Qt::Key_Clear:
                if ( isKeypad && m_allowDirection)
                {
                    qDebug() << "caught 5 key";
                    ev->accept();
                    m_terminator = BACKUP;
                    inputComplete();
                }
                break;

        case Qt::Key_Enter:
            if (isKeypad)
            {
                qDebug() << "caught Enter";
                ev->accept();
                m_terminator = ENTER;
                inputComplete();
            }
            break;
            
        case Qt::Key_Return:
            qDebug() << "caught Return";
            ev->accept();
            m_ctrlMSubstitution = true;
            QCoreApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ShiftModifier, "^"));
            QCoreApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_M, Qt::ShiftModifier, "M"));
            break;
        default:
            ev->ignore();
            break;
        }
    }
    else
    {
        ev->ignore();
    }

}
