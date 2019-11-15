#include "plotwidget.h"
#include "cachedsin.h"

#include <QPen>
#include <QPainter>
#include <QPolygonF>
#include <QPaintEvent>

#include <cmath>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
{}

void PlotWidget::advance(qint64 time)
{
    m_time += time;
    update();
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    const QRect rect = event->rect();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect, Qt::black);

    // calculate 1000 points of std::sin and cache the result
    static CachedSin cachedSin(1000);

    const float omega = M_PI / 200.f;
    const float timeOffset = m_time * rect.width() / 2000;
    float x = rect.x();
    const float deltaX = .1;
    const int numPoints = rect.width() / deltaX;
    const float deltaY = size().height() / 2;
    const float scaleY = size().height() / 4;

    QPolygonF pointsOriginal(numPoints);
    QPolygonF pointsCached(numPoints);
    QPolygonF pointsDiff(numPoints);

    auto mapPoint = [scaleY, deltaY] (float x, float y)
    {
        return QPointF(x, y * scaleY + deltaY);
    };

    for (int i = 0; i < numPoints; ++i, x += deltaX) {
        const auto x_i = (x + timeOffset) * omega;
        const float original = std::sin(x_i);
        const float cached = cachedSin(x_i);

        pointsOriginal[i] = mapPoint(x, original);
        pointsCached[i] = mapPoint(x, cached);
        pointsDiff[i] = mapPoint(x, original - cached);
    }

    QPen pen(Qt::white, 4);
    painter.setPen(pen);
    painter.drawPolyline(pointsDiff);

    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawPolyline(pointsCached);

    pen.setColor(Qt::green);
    painter.setPen(pen);
    painter.drawPolyline(pointsOriginal);
}
