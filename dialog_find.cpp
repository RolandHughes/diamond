/**************************************************************************
*
* Copyright (c) 2012-2013 Barbara Geller
* All rights reserved.
*
* This file is part of Diamond Editor.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 3
* as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Diamond Editor.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "dialog_find.h"
#include "util.h"

#include <QComboBox>
#include <QCursor>
#include <QLineEdit>
#include <QMenu>
#include <QTextCursor>

Dialog_Find::Dialog_Find(QString findText, QStringList findList)
   : m_ui(new Ui::Dialog_Find)
{   
   m_findList = findList;

   m_ui->setupUi(this);
   this->setUp();

   // display last edit value
   m_ui->find_Combo->lineEdit()->setText(findText);

   // any key deletes first, right arrow to continue typing
   m_ui->find_Combo->lineEdit()->selectAll();

   connect(m_ui->find_PB,   SIGNAL(clicked()),this, SLOT(Find()));
   connect(m_ui->cancel_PB, SIGNAL(clicked()),this, SLOT(Cancel()));
}

Dialog_Find::~Dialog_Find()
{
   delete m_ui;
}

void Dialog_Find::setUp()
{
   m_ui->down_RB->setChecked(true);

   //
   m_ui->find_Combo->setEditable(true);
   m_ui->find_Combo->addItems(m_findList);

   // add a context menu
   m_ui->find_Combo->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui->find_Combo, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(combo_ContextMenu(const QPoint &)));
}

void Dialog_Find::combo_ContextMenu(const QPoint &pt)
{
   QMenu *menu = m_ui->find_Combo->lineEdit()->createStandardContextMenu();

   menu->addSeparator();
   menu->addAction("Clear Find List",   this, SLOT(menu_clearList()) );
   menu->addAction("Delete Find Entry", this, SLOT(menu_deleteEntry()) );
   menu->popup(QCursor::pos());

   // takes care of deleting the menu after displayed, avoids a memory leak
   connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
}

void Dialog_Find::menu_clearList()
{   
   m_findList.clear();
   m_ui->find_Combo->clear();
}

void Dialog_Find::menu_deleteEntry()
{
   QString text = m_ui->find_Combo->lineEdit()->text();
   m_findList.removeOne(text);

   int index = m_ui->find_Combo->currentIndex();
   m_ui->find_Combo->removeItem(index);
}

void Dialog_Find::Find()
{
   this->done(1);
}

void Dialog_Find::Cancel()
{
   this->done(0);
}

QString Dialog_Find::get_findText()
{
   return m_ui->find_Combo->currentText();
}

QStringList Dialog_Find::get_findList()
{
   return m_findList;
}

bool Dialog_Find::get_Direction()
{
   if (m_ui->down_RB->isChecked()) {
      return true;
   } else {
      return false;
   }
}

bool Dialog_Find::get_Case()
{
   return m_ui->case_CKB->isChecked();
}

bool Dialog_Find::get_WholeWords()
{
   return m_ui->wholeWords_CKB->isChecked();
}
