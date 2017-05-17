#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include "qdispatcher.h"
#include "tests.h"

Tests::Tests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void Tests::test_dispatch()
{
    QDispatcher dispatcher;
    int count = 0;

    int id = dispatcher.addListener("test", [&]() {
        count++;
    });

    QCOMPARE(dispatcher.hasListener("test"), true);
    QCOMPARE(dispatcher.hasListener("valid"), false);

    dispatcher.dispatch("invalid");
    QCOMPARE(count, 0);
    dispatcher.dispatch("test");
    QCOMPARE(count, 1);

    dispatcher.dispatch("test");
    QCOMPARE(count, 2);
    dispatcher.removeListener(id);
    QCOMPARE(dispatcher.hasListener("test"), false);
}

