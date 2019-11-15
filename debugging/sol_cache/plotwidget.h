#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

class QPaintEvent;

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void advance(qint64 time);

protected:
    /**
     * Paint two sin curves in the full width of the widget:
     * - a red curve using CachedSin
     * - a green curve using std::sin
     *
     * If no red can be seen, then the cached implementation is "good enough".
     */
    void paintEvent(QPaintEvent *event) override;

private:
    qint64 m_time = 0;
};

#endif // PLOTWIDGET_H
