#include <QCoreApplication>
#include <QPointer>
#include <QtCore>
#include "qdispatcher.h"

static QPointer<QDispatcher> m_globalInstance;

QDispatcher *QDispatcher::globalInstance()
{
    if (!m_globalInstance) {
        QCoreApplication* app = QCoreApplication::instance();
        m_globalInstance = new QDispatcher(app);
    }
    return m_globalInstance;
}

QDispatcher::QDispatcher(QObject *parent) : QObject(parent) , m_dispatching(false), nextId(0) {
}

void QDispatcher::dispatch(QString type , QVariantMap message) {

    if (m_dispatching) {
        m_queue.enqueue(QPair<QString,QVariantMap> (type,message) );
        return;
    }

    auto process = [=](QString& type, QVariantMap& message) {
        ListenerInfoList list;
        if (m_listeners.contains(type)) {
            list = m_listeners[type];
        }

        for (int i = 0 ; i < list.size(); i++) {
            list[i].listener(message);
        }

        emit dispatched(type, message);
    };

    m_dispatching = true;
    process(type,message);

    while (m_queue.size() > 0) {
        QPair<QString,QVariantMap> pair = m_queue.dequeue();
        process(pair.first,pair.second);
    }

    m_dispatching = false;
}

int QDispatcher::addListener(QString type, std::function<void (QVariantMap)> listener)
{
    ListenerInfoList listeners;

    if (m_listeners.contains(type)) {
        listeners = m_listeners[type];
    }

    ListenerInfo info;
    info.id = nextId++;
    info.listener = listener;
    listeners << info;

    m_listeners[type] = listeners;
    reverseLookupTable[info.id] = type;

    return info.id;
}

int QDispatcher::addListener(const QString type, std::function<void ()> listener)
{
    return addListener(type, [=](QVariantMap message) {
        Q_UNUSED(message);
        listener();
    });
}

bool QDispatcher::hasListener(QString type)
{
    return m_listeners.contains(type);
}

void QDispatcher::removeListener(int id)
{
    if (!reverseLookupTable.contains(id)) {
        return;
    }

    QString type = reverseLookupTable[id];

    ListenerInfoList listeners = m_listeners[type];

    for (int i = 0 ; i < listeners.size();i++) {
        if (listeners[i].id == id) {
            listeners.removeAt(i);
            break;
        }
    }

    if (listeners.size() > 0) {
        m_listeners[type] = listeners;
    } else {
        m_listeners.remove(type);
    }
    reverseLookupTable.remove(id);
}
