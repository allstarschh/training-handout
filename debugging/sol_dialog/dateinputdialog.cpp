/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include "dateinputdialog.h"

#include "ui_dateinputdialog.h"

DateInputDialog::DateInputDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::DateInputDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->calendar, &QCalendarWidget::selectionChanged,
            this, &DateInputDialog::selectionChanged);

    // NOTE: only set Qt::WA_DeleteOnClose externally as needed
}

DateInputDialog::~DateInputDialog()
{
}

QDate DateInputDialog::dateValue() const
{
    return m_ui->calendar->selectedDate();
}

void DateInputDialog::selectionChanged()
{
    emit dateValueChanged(dateValue());
}
