/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "window.h"

#include "ui_window.h"
#include "dateinputdialog.h"

#include <QDate>
#include <QMessageBox>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , m_today(QDate::currentDate())
{
    m_ui->setupUi(this);

    connect(m_ui->setBirthday, &QToolButton::clicked,
            this, &Window::changeBirthday);

    m_ui->today->setText(m_today.toString());

    setBirthday(m_today);
}

void Window::changeBirthday()
{
    DateInputDialog dialog;
    dialog.exec();
    setBirthday(dialog.dateValue());
}

void Window::setBirthday(const QDate &date)
{
    m_ui->birthday->setText(date.toString());

    int days = date.daysTo(m_today);
    m_ui->days->setText(QString::number(days));
    m_ui->hours->setText(QString::number(days * 24));
    m_ui->minutes->setText(QString::number(days * 24 * 60));
    m_ui->seconds->setText(QString::number(days * 24 * 60 * 60));
}
