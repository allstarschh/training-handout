#ifndef EVENTS_H
#define EVENTS_H

#include <QEvent>

class UpdateEvent : public QEvent
{
public:
    UpdateEvent()
        : QEvent(static_cast<QEvent::Type>(ms_eventType))
    {}

    static int eventType() { return ms_eventType; }

private:
    static int ms_eventType;
};

#endif // EVENTS_H
