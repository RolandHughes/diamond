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

#include "dialog_colors.h"
#include "util.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QLineEdit>
#include <QPalette>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

Dialog_Colors::Dialog_Colors( QWidget *parent, Settings& settings )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Colors )
    , m_syntaxParser(nullptr)
{
    m_ui->setupUi( this );
    this->setWindowIcon( QIcon( "://resources/diamond.png" ) );

    qDebug() << "Dialog_Colors: " << settings.activeTheme();
    m_settings = settings;
    m_syntaxFname  = m_settings.syntaxPath() + "syn_cpp.json";

    m_syntaxParser = new Syntax( m_ui->sample->document(), m_syntaxFname, m_settings );
    updateParser( false );


    connect( m_ui->text_TB,           &QToolButton::clicked, this, &Dialog_Colors::text_TB       );
    connect( m_ui->back_TB,           &QToolButton::clicked, this, &Dialog_Colors::back_TB       );
    connect( m_ui->gutterText_TB,     &QToolButton::clicked, this, &Dialog_Colors::gutterText_TB );
    connect( m_ui->gutterBack_TB,     &QToolButton::clicked, this, &Dialog_Colors::gutterBack_TB );
    connect( m_ui->current_TB,        &QToolButton::clicked, this, &Dialog_Colors::current_TB    );

    connect( m_ui->key_TB,            &QToolButton::clicked, this, &Dialog_Colors::key_TB      );
    connect( m_ui->type_TB,           &QToolButton::clicked, this, &Dialog_Colors::type_TB     );
    connect( m_ui->class_TB,          &QToolButton::clicked, this, &Dialog_Colors::class_TB    );
    connect( m_ui->func_TB,           &QToolButton::clicked, this, &Dialog_Colors::func_TB     );
    connect( m_ui->quote_TB,          &QToolButton::clicked, this, &Dialog_Colors::quote_TB    );
    connect( m_ui->comment_TB,        &QToolButton::clicked, this, &Dialog_Colors::comment_TB  );
    connect( m_ui->mline_TB,          &QToolButton::clicked, this, &Dialog_Colors::mline_TB    );
    connect( m_ui->const_TB,          &QToolButton::clicked, this, &Dialog_Colors::const_TB    );

    connect( m_ui->key_Bold_CB,       &QCheckBox::clicked,   this, &Dialog_Colors::key_bold       );
    connect( m_ui->key_Italic_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::key_italic     );
    connect( m_ui->type_Bold_CB,      &QCheckBox::clicked,   this, &Dialog_Colors::type_bold      );
    connect( m_ui->type_Italic_CB,    &QCheckBox::clicked,   this, &Dialog_Colors::type_italic    );
    connect( m_ui->class_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::class_bold     );
    connect( m_ui->class_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::class_italic   );
    connect( m_ui->func_Bold_CB,      &QCheckBox::clicked,   this, &Dialog_Colors::func_bold      );
    connect( m_ui->func_Italic_CB,    &QCheckBox::clicked,   this, &Dialog_Colors::func_italic    );
    connect( m_ui->quote_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::quote_bold     );
    connect( m_ui->quote_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::quote_italic   );
    connect( m_ui->comment_Bold_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::comment_bold   );
    connect( m_ui->comment_Italic_CB, &QCheckBox::clicked,   this, &Dialog_Colors::comment_italic );
    connect( m_ui->mline_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::mline_bold     );
    connect( m_ui->mline_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::mline_italic   );

    connect( m_ui->save_PB,           &QPushButton::clicked, this, &Dialog_Colors::save          );
    connect( m_ui->cancel_PB,         &QPushButton::clicked, this, &Dialog_Colors::cancel        );
    connect( m_ui->copy_button,       &QPushButton::clicked, this, &Dialog_Colors::copyClicked   );
    connect( m_ui->delete_button,     &QPushButton::clicked, this, &Dialog_Colors::deleteClicked );
    connect( m_ui->export_button,     &QPushButton::clicked, this, &Dialog_Colors::exportClicked );
    connect( m_ui->import_button,     &QPushButton::clicked, this, &Dialog_Colors::importClicked );

    m_ui->theme_comboBox->clear();
    m_ui->theme_comboBox->addItems( m_settings.availableThemes() );

    // Do not connect before assigning data as that will cause syntax update with
    // ill-formed m_settings.
    //
    connect( m_ui->theme_comboBox,
             static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
             this,
             &Dialog_Colors::themeChanged );

    m_ui->theme_comboBox->setCurrentIndex(
        m_ui->theme_comboBox->findText( m_settings.activeTheme() ) );

}

Dialog_Colors::~Dialog_Colors()
{
    delete m_ui;
    if (m_syntaxParser != nullptr)
        delete m_syntaxParser;
}

void Dialog_Colors::colorBox( QLineEdit *field, QColor color )
{
    QPalette temp;

    temp = field->palette();
    temp.setColor( QPalette::Base, color );
    field->setPalette( temp );
}

void Dialog_Colors::themeChanged( const QString& themeName )
{
    if ( themeName.isEmpty() )
    { return; }

    m_settings.set_activeTheme(themeName);

    // ordinary text
    m_ui->text_Color->setReadOnly( true );
    colorBox( m_ui->text_Color, m_settings.currentTheme().colorText() );

    m_ui->back_Color->setReadOnly( true );
    colorBox( m_ui->back_Color, m_settings.currentTheme().colorBack() );

    // line number margin/gutter
    m_ui->gutterText_Color->setReadOnly( true );
    colorBox( m_ui->gutterText_Color, m_settings.currentTheme().gutterText() );

    m_ui->gutterBack_Color->setReadOnly( true );
    colorBox( m_ui->gutterBack_Color, m_settings.currentTheme().gutterBack() );

    // current line background color
    m_ui->current_Color->setReadOnly( true );
    colorBox( m_ui->current_Color, m_settings.currentTheme().currentLineBack() );

    /* Key */
    m_ui->key_Color->setReadOnly( true );
    colorBox( m_ui->key_Color, m_settings.currentTheme().syntaxKey().color() );

    if ( m_settings.currentTheme().syntaxKey().weight() == QFont::Bold )
    {
        m_ui->key_Bold_CB->setChecked( true );
    }

    m_ui->key_Italic_CB->setChecked( m_settings.currentTheme().syntaxKey().italic() );

    /* Type */
    m_ui->type_Color->setReadOnly( true );
    colorBox( m_ui->type_Color, m_settings.currentTheme().syntaxType().color() );

    if ( m_settings.currentTheme().syntaxType().weight() == QFont::Bold )
    {
        m_ui->type_Bold_CB->setChecked( true );
    }

    m_ui->type_Italic_CB->setChecked( m_settings.currentTheme().syntaxType().italic() );

    /* Class */
    m_ui->class_Color->setReadOnly( true );
    colorBox( m_ui->class_Color, m_settings.currentTheme().syntaxClass().color() );

    if ( m_settings.currentTheme().syntaxClass().weight() == QFont::Bold )
    {
        m_ui->class_Bold_CB->setChecked( true );
    }

    m_ui->class_Italic_CB->setChecked( m_settings.currentTheme().syntaxClass().italic() );

    /* Func */
    m_ui->func_Color->setReadOnly( true );
    colorBox( m_ui->func_Color, m_settings.currentTheme().syntaxFunc().color() );

    if ( m_settings.currentTheme().syntaxFunc().weight() == QFont::Bold )
    {
        m_ui->func_Bold_CB->setChecked( true );
    }

    m_ui->func_Italic_CB->setChecked( m_settings.currentTheme().syntaxFunc().italic() );

    /* Quote */
    m_ui->quote_Color->setReadOnly( true );
    colorBox( m_ui->quote_Color, m_settings.currentTheme().syntaxQuote().color() );

    if ( m_settings.currentTheme().syntaxQuote().weight() == QFont::Bold )
    {
        m_ui->quote_Bold_CB->setChecked( true );
    }

    m_ui->quote_Italic_CB->setChecked( m_settings.currentTheme().syntaxQuote().italic() );

    /* Comment */
    m_ui->comment_Color->setReadOnly( true );
    colorBox( m_ui->comment_Color, m_settings.currentTheme().syntaxComment().color() );

    if ( m_settings.currentTheme().syntaxComment().weight() == QFont::Bold )
    {
        m_ui->comment_Bold_CB->setChecked( true );
    }

    m_ui->comment_Italic_CB->setChecked( m_settings.currentTheme().syntaxComment().italic() );

    /* MLine comment*/
    m_ui->mline_Color->setReadOnly( true );
    colorBox( m_ui->mline_Color, m_settings.currentTheme().syntaxMLine().color() );

    if ( m_settings.currentTheme().syntaxMLine().weight() == QFont::Bold )
    {
        m_ui->mline_Bold_CB->setChecked( true );
    }

    m_ui->mline_Italic_CB->setChecked( m_settings.currentTheme().syntaxMLine().italic() );

    /* Constant */
    m_ui->const_Color->setReadOnly( true );
    colorBox( m_ui->const_Color, m_settings.currentTheme().syntaxConstant().color() );

    if ( m_settings.currentTheme().syntaxConstant().weight() == QFont::Bold )
    {
        m_ui->constant_Bold_CB->setChecked( true );
    }

    m_ui->constant_Italic_CB->setChecked( m_settings.currentTheme().syntaxConstant().italic() );

    if (m_settings.currentTheme().isProtected())
    {
        m_ui->delete_button->setEnabled(false);
    }
    else
    {
        m_ui->delete_button->setEnabled(true);
    }

    updateParser( true);
}

void Dialog_Colors::copyClicked()
{
    QString destName;
    bool okFlag = false;

    //  TODO:: Really need to get an AlphaNumeric hint added
    destName = QInputDialog::getText(this, "New Theme name", "Name: ", QLineEdit::Normal,
                                     destName, &okFlag, Qt::Dialog, Qt::ImhNone);
    if (okFlag)
    {
        m_settings.copyTheme(m_ui->theme_comboBox->currentText(), destName);
        m_ui->theme_comboBox->addItem( destName );
        m_ui->theme_comboBox->setCurrentIndex( m_ui->theme_comboBox->findText( destName ) );
    }
}

void Dialog_Colors::deleteClicked()
{
    qDebug() << "Theme: " << m_settings.currentTheme().name() << "  protected: "
             << m_settings.currentTheme().isProtected();
    if (m_settings.currentTheme().isProtected())
    {
        int rsp = QMessageBox::warning(this, "Delete", "Cannot delete pre-installed theme",
                                       QMessageBox::Ok);
        return;
    }
    
    int btn = QMessageBox::question(this, "Confirm Deletion", "Really delete selected theme?",
                                    QMessageBox::Yes, QMessageBox::No);
    if (btn == QMessageBox::Yes)
    {
        m_settings.deleteTheme(m_ui->theme_comboBox->currentText());
        m_ui->theme_comboBox->removeItem(m_ui->theme_comboBox->currentIndex());
        m_ui->theme_comboBox->setCurrentIndex(0);
    }
}

void Dialog_Colors::exportClicked()
{
    // TODO:: Need to use file choose to obtain destination name
    //        Also need to add export function to settings
    int btn = QMessageBox::information(this, "Export", "Not yet implemented", QMessageBox::Ok);
}

void Dialog_Colors::importClicked()
{
    // TODO:: Need to use file choose to obtain input file name
    //        Also need to add import function to settings
    int btn = QMessageBox::information(this, "Import", "Not yet implemented", QMessageBox::Ok);
}

void Dialog_Colors::updateParser( bool newSettings )
{
    if ( newSettings )
    {
        m_syntaxParser->processSyntax( m_settings );

    }
    else
    {
        m_syntaxParser->processSyntax();
    }
}

QColor Dialog_Colors::pickColor( QColor oldColor )
{
    QColor color = oldColor;

    if ( false )       // (useNative)
    {
        color = QColorDialog::getColor( color, this );
    }
    else
    {
        color = QColorDialog::getColor( color, this, "Select Color", QColorDialog::DontUseNativeDialog );
    }

    if ( color.isValid() )
    {
        return color;
    }

    return oldColor;
}

void Dialog_Colors::text_TB()
{
    QColor color = m_settings.currentTheme().colorText();
    m_settings.currentTheme().set_colorText( pickColor( color ) );

    //
    colorBox( m_ui->text_Color, m_settings.currentTheme().colorText() );

    //
    QPalette temp;
    temp = m_ui->sample->palette();
    temp.setColor( QPalette::Text, m_settings.currentTheme().colorText() );
    temp.setColor( QPalette::Base, m_settings.currentTheme().colorBack() );
    m_ui->sample->setPalette( temp );
}

void Dialog_Colors::back_TB()
{
    QColor color = m_settings.currentTheme().colorBack();
    m_settings.currentTheme().set_colorBack( pickColor( color ) );

    //
    colorBox( m_ui->back_Color, m_settings.currentTheme().colorBack() );

    //
    QPalette temp;
    temp = m_ui->sample->palette();
    temp.setColor( QPalette::Text, m_settings.currentTheme().colorText() );
    temp.setColor( QPalette::Base, m_settings.currentTheme().colorBack() );
    m_ui->sample->setPalette( temp );
}

void Dialog_Colors::gutterText_TB()
{
    QColor color = m_settings.currentTheme().gutterText();
    m_settings.currentTheme().set_gutterText( pickColor( color ) );

    //
    colorBox( m_ui->gutterText_Color, m_settings.currentTheme().gutterText() );
    updateParser( true );
}

void Dialog_Colors::gutterBack_TB()
{
    QColor color = m_settings.currentTheme().gutterBack();
    m_settings.currentTheme().set_gutterBack( pickColor( color ) );

    //
    colorBox( m_ui->gutterBack_Color, m_settings.currentTheme().gutterBack() );
    updateParser( true );
}

void Dialog_Colors::current_TB()
{
    QColor color = m_settings.currentTheme().currentLineBack();
    m_settings.currentTheme().set_currentLineBack( pickColor( color ) );

    //
    colorBox( m_ui->current_Color, m_settings.currentTheme().currentLineBack() );
    updateParser( true );
}

void Dialog_Colors::key_TB()
{
    QColor color = m_settings.currentTheme().syntaxKey().color();
    m_settings.currentTheme().syntaxKey().set_color( pickColor( color ) );

    //
    colorBox( m_ui->key_Color, m_settings.currentTheme().syntaxKey().color() );
    updateParser( true );
}

void Dialog_Colors::type_TB()
{
    QColor color = m_settings.currentTheme().syntaxType().color();
    m_settings.currentTheme().syntaxType().set_color( pickColor( color ) );

    //
    colorBox( m_ui->type_Color, m_settings.currentTheme().syntaxType().color() );
    updateParser( true );
}

void Dialog_Colors::class_TB()
{
    QColor color = m_settings.currentTheme().syntaxClass().color();
    m_settings.currentTheme().syntaxClass().set_color( pickColor( color ) );

    //
    colorBox( m_ui->class_Color, m_settings.currentTheme().syntaxClass().color() );
    updateParser( true );
}

void Dialog_Colors::func_TB()
{
    QColor color = m_settings.currentTheme().syntaxFunc().color();
    m_settings.currentTheme().syntaxFunc().set_color( pickColor( color ) );

    //
    colorBox( m_ui->func_Color, m_settings.currentTheme().syntaxFunc().color() );
    updateParser( true );
}

void Dialog_Colors::quote_TB()
{
    QColor color = m_settings.currentTheme().syntaxQuote().color();
    m_settings.currentTheme().syntaxQuote().set_color( pickColor( color ) );

    //
    colorBox( m_ui->quote_Color, m_settings.currentTheme().syntaxQuote().color() );
    updateParser( true );
}

void Dialog_Colors::comment_TB()
{
    QColor color = m_settings.currentTheme().syntaxComment().color();
    m_settings.currentTheme().syntaxComment().set_color( pickColor( color ) );

    //
    colorBox( m_ui->comment_Color, m_settings.currentTheme().syntaxComment().color() );
    updateParser( true );
}

void Dialog_Colors::mline_TB()
{
    QColor color = m_settings.currentTheme().syntaxMLine().color();
    m_settings.currentTheme().syntaxMLine().set_color( pickColor( color ) );

    //
    colorBox( m_ui->mline_Color, m_settings.currentTheme().syntaxMLine().color() );
    updateParser( true );
}

void Dialog_Colors::const_TB()
{
    QColor color = m_settings.currentTheme().syntaxConstant().color();
    m_settings.currentTheme().syntaxConstant().set_color( pickColor( color ) );

    //
    colorBox( m_ui->const_Color, m_settings.currentTheme().syntaxConstant().color() );
    updateParser( true );
}


/**/
void Dialog_Colors::key_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->key_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxKey().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::key_italic()
{
    bool b = false;

    if ( m_ui->key_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxKey().set_italic( b );

    updateParser( true );
}

void Dialog_Colors::type_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->type_Bold_CB->isChecked() )
    {
        f = QFont::Normal;
    }

    m_settings.currentTheme().syntaxType().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::type_italic()
{
    bool b = false;

    if ( m_ui->type_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxType().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::class_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->class_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxClass().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::class_italic()
{
    bool b = false;

    if ( m_ui->class_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxClass().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::func_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->func_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxFunc().set_weight( f );
    updateParser( true );;
}

void Dialog_Colors::func_italic()
{
    bool b = false;

    if ( m_ui->func_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxFunc().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::quote_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->quote_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxQuote().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::quote_italic()
{
    bool b = false;

    if ( m_ui->quote_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxQuote().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::comment_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->comment_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxComment().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::comment_italic()
{
    bool b = false;

    if ( m_ui->comment_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxComment().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::mline_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->mline_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_settings.currentTheme().syntaxMLine().set_weight( f );
    updateParser( true );
}

void Dialog_Colors::mline_italic()
{
    bool b = false;

    if ( m_ui->mline_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_settings.currentTheme().syntaxMLine().set_italic( b );
    updateParser( true );
}

void Dialog_Colors::save()
{
    this->done( QDialog::Accepted );
}

void Dialog_Colors::cancel()
{
    this->done( QDialog::Rejected );
}

Settings &Dialog_Colors::get_Colors()
{
    return m_settings;
}
