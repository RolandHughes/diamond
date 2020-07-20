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

#include "mainwindow.h"
#include "util.h"

#include <QStringList>
#include <QVariant>

void MainWindow::openTab_CreateMenus()
{
   // re-populate m_openedFiles
   QString fullName;
   QString tName;

   int cnt = m_tabWidget->count();
   m_settings.openedFilesClear();
   m_settings.openedModifiedClear();

   for (int k = 0; k < cnt; ++k) {
      fullName = this->get_curFileName(k);

      if (fullName.isEmpty()) {
         --cnt;

      } else {
          m_settings.openedFilesAppend(fullName);
          m_settings.openedModifiedAppend(false);
      }
   }

   //
   QMenu *windowMenu = m_ui->menuWindow;
   windowMenu->addSeparator();

   for (int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k) {

      if (k < cnt)  {
          fullName = m_settings.openedFiles(k);
         tName    = fullName;

         if (m_settings.openedModified(k)) {
            tName += " *";
         }

      } else {
         tName = "file"+QString::number(k);

      }

      openTab_Actions[k] = new QAction(tName, this);

//    openTab_Actions[k]->setData("select-tab");
      openTab_Actions[k]->setData(QString("select-tab"));

      windowMenu->addAction(openTab_Actions[k]);

      if (k >= cnt)  {
         openTab_Actions[k]->setVisible(false);
      }

      connect(openTab_Actions[k], &QAction::triggered, this, [this, k](bool){ openTab_Select(k); } );
   }
}

void MainWindow::openTab_Select(int index)
{ 
   bool match = false;
   QString fullName = m_settings.openedFiles(index);

   if (fullName.isEmpty()) {
      // something is pretty bogus

   } else {
      int cnt   = m_tabWidget->count();
      int index = m_tabWidget->currentIndex();

      for (int k = 0; k < cnt; ++k) {
         QString tcurFile = this->get_curFileName(k);

         if (tcurFile == fullName) {
            match = true;
            index = k;
            break;
         }
      }

      if (match) {
         // select new tab
         m_tabWidget->setCurrentIndex(index);

      } else {
         // delete entry from list since it did not exist
          m_settings.openedFilesRemove(fullName);

         // update actions
         openTab_UpdateActions();
      }
   }
}

void MainWindow::showContext_Tabs(const QPoint &pt)
{
   QAction *action = m_ui->menuWindow->actionAt(pt);

   if (action)  {
      QString data = action->data().toString();

      if (data == "select-tab")  {
         QMenu *menu = new QMenu(this);
         menu->addAction("Reorder Tab file list", this, SLOT(openTab_redo() ));

         menu->exec(m_ui->menuWindow->mapToGlobal(pt));
         delete menu;
      }
   }
}

void MainWindow::openTab_redo()
{
   QWidget *temp;
   DiamondTextEdit *textEdit;

   QAction *action;
   action = (QAction *)sender();

   // TODO:: this should save settings
   if (action) {
      // re-populate m_openedFiles and m_openedModified
      QString tName;
      bool isModified;

      m_settings.openedFilesClear();
      m_settings.openedModifiedClear();

      int cnt = m_tabWidget->count();

      for (int k = 0; k < cnt; ++k) {
         tName = this->get_curFileName(k);
         m_settings.openedFilesAppend(tName);

         //
         temp = m_tabWidget->widget(k);
         textEdit = dynamic_cast<DiamondTextEdit *>(temp);

         if (textEdit) {
            isModified = textEdit->document()->isModified();
            m_settings.openedModifiedAppend(isModified);
         }
      }

      for (int i = 0; i < DiamondLimits::OPENTABS_MAX; ++i) {

         if (i < cnt)  {
             tName = m_settings.openedFiles(i);

             if (m_settings.openedModified(i)) {
               tName += " *";
            }

         } else {
            tName = "file"+QString::number(i);
         }

         openTab_Actions[i]->setText(tName);

         if (i >= cnt)  {
            openTab_Actions[i]->setVisible(false);
         }

      }
   }
}

void MainWindow::openTab_Add()
{ 
   if (m_curFile.isEmpty()) {
      return;
   }

   m_settings.openedFilesAppend(m_curFile);
   m_settings.openedModifiedAppend(false);

   // update actions
   openTab_UpdateActions();
}

void MainWindow::openTab_Delete()
{
    m_settings.openedFilesRemove(m_curFile);

   // update actions
   openTab_UpdateActions();
}

void MainWindow::openTab_UpdateActions()
{
    int cnt = m_settings.openedFilesCount();

   for (int k = 0; k < DiamondLimits::OPENTABS_MAX; ++k) {

      if (k < cnt)  {
         QString modified;

         if (m_settings.openedModified(k)) {
            modified += " *";
         }

         openTab_Actions[k]->setText(m_settings.openedFiles(k) + modified);
         openTab_Actions[k]->setVisible(true);

     } else {        
         openTab_Actions[k]->setVisible(false);
     }
   }
}

void MainWindow::openTab_UpdateOneAction(int index, bool isModified)
{
    if (index < DiamondLimits::OPENTABS_MAX) {
      QString modified;

      if (isModified) {
         modified += " *";
      }

      openTab_Actions[index]->setText(m_settings.openedFiles(index) + modified);
   }
}
