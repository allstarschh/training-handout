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
    , m_ui(new Ui::Window) // m_ui must be initialized
{
    m_ui->setupUi(this);

    connect(m_ui->setBirthday, &QToolButton::clicked,
            this, &Window::changeBirthday);

    m_ui->today->setText(m_today.toString());

    setBirthday(m_today);
}

Window::~Window()
{
}

void Window::changeBirthday()
{
    /* Create on stack and react to changes via signal.
     *
     * Alternatively, create it on the heap and set the Qt::WA_DeleteOnClose
     * attribute from here.
     */
    DateInputDialog dialog;
    connect(&dialog, &DateInputDialog::dateValueChanged,
            this, &Window::setBirthday);
    dialog.exec();
}

void Window::setBirthday(const QDate &date)
{
    m_ui->birthday->setText(date.toString());

    /* Note that dates way back in the past or far in the future can easily
     * lead to signed integer overflows. The undefined behavior sanitizer (UBSAN)
     * will detect this as e.g.:
     *
     * sol_dialog/window.cpp:58:43: runtime error: signed integer overflow:
     *         87317 * 86400 cannot be represented in type 'int'
     *
     * To prevent this issue, we upcast the int here to a wider type.
     */
    qint64 days = date.daysTo(m_today);
    m_ui->days->setText(QString::number(days));
    m_ui->hours->setText(QString::number(days * 24));
    m_ui->minutes->setText(QString::number(days * 24 * 60));
    m_ui->seconds->setText(QString::number(days * 24 * 60 * 60));
}
