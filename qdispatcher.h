#pragma once
#include <QObject>
#include <QVariantMap>
#include <functional>
#include <QQueue>
#include <QPair>

/// QDispatcher is an universal message dispatcher to provide an asynchronous communication interface between C++ classes and Objective-C source code.

class QDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit QDispatcher(QObject *parent = 0);

    static QDispatcher* globalInstance();

    /// Deliver a message
    /** If there has a registered listener function , it will return TRUE. Otherwise, it will return FALSE.
     *
     * After processed by the registered helper, the "received" signal will be emitted
     * in next tick of event loop.
     */
    Q_INVOKABLE void dispatch(QString type , QVariantMap message = QVariantMap());

    /// Register a message listener.
    /**
     * @brief addListener
     * @param type
     * @param listener
     * @return The ID of the registered listener
     */
    int addListener(const QString type, std::function<void(QVariantMap)> listener);

    int addListener(const QString type, std::function<void()> listener);

    /// Return TRUE if it there has any message listener registered for this type of message.
    bool hasListener(QString type);

    void removeListener(int id);

signals:
    /// The signal is emitted when a message is ready to dispatch.
    void dispatched(QString name , QVariantMap data);

private:
    typedef std::function<void(QVariantMap)> Listener;

    class ListenerInfo {
    public:
        int id;
        Listener listener;
    };

    typedef QList<ListenerInfo> ListenerInfoList;

    QMap<QString, ListenerInfoList> m_listeners;
    QMap<int, QString> reverseLookupTable;

    bool m_dispatching;
    QQueue<QPair<QString,QVariantMap>> m_queue;
    int nextId;
};
