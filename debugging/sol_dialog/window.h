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
    /**
     * An empty destructor definition is required such that the QScopedPointer
     * destructor will be called from the scope of the implementation. This
     * allows us to forward-declare the Ui::Window in the header here and wrap
     * it in a QScopedPointer, as it will only be used in the implementation.
     */
    ~Window() override;

private Q_SLOTS:
    void changeBirthday();
    void setBirthday(const QDate &date);

private:
    QDate m_today;
    QScopedPointer<Ui::Window> m_ui;
};

#endif // WINDOW_H
