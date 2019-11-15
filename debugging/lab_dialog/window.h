/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDate>

namespace Ui
{
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);

private Q_SLOTS:
    void changeBirthday();
    void setBirthday(const QDate &date);

private:
    QDate m_today;
    Ui::Window *m_ui;
};

#endif // WINDOW_H
