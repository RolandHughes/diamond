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

#include "dialog_busy.h"

#include <QMovie>
#include <QByteArray>

Dialog_Busy::Dialog_Busy( QWidget *parent )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Busy )
{
    m_ui->setupUi( this );

    setWindowFlags( Qt::FramelessWindowHint | Qt::Dialog );
    m_ui->movieLabel->setAttribute( Qt::WA_NoSystemBackground );

    qDebug() << "supported formats: " << QMovie::supportedFormats();
    QByteArray emptyFormat;
    m_ui->movieLabel->setMovie( new QMovie( "://resources/spinning-red-diamond-4.gif", emptyFormat, this ) );



}

Dialog_Busy::~Dialog_Busy()
{
    delete m_ui;
}

void Dialog_Busy::showBusy()
{
    this->show();
    m_ui->movieLabel->movie()->start();
}

void Dialog_Busy::hideBusy()
{
    this->hide();
    m_ui->movieLabel->movie()->stop();
}