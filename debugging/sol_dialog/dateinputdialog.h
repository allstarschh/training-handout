/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef DATEINPUTDIALOG_H
#define DATEINPUTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class DateInputDialog;
}

class QDate;

class DateInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DateInputDialog(QWidget *parent = nullptr);
    ~DateInputDialog() override;

    QDate dateValue() const;

Q_SIGNALS:
    void dateValueChanged(const QDate &date);

private Q_SLOTS:
    void selectionChanged();

private:
    QScopedPointer<Ui::DateInputDialog> m_ui;
};

#endif // DATEINPUTDIALOG_H
