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

#include "dialog_edt_prompt.h"

Dialog_Edt_Prompt::Dialog_Edt_Prompt(QString labelText, bool allowDirection, QWidget *parent) :
    QDialog(parent)
    , m_lineEdit(nullptr)
    , m_label(nullptr)
{

    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags | Qt::Tool);

    m_lineEdit = new Edt_LineEdit(this);
    m_lineEdit->set_allowDirection( allowDirection);
    m_label    = new QLabel(labelText, this);
}

Dialog_Edt_Prompt::~Dialog_Edt_Prompt()
{
    if (m_lineEdit)
    {
        delete m_lineEdit;
        m_lineEdit = nullptr;
    }

    if (m_label)
    {
        delete m_label;
        m_label = nullptr;
    }
}
