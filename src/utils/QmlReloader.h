#pragma once
#include <QObject>
#include <qqmlapplicationengine.h>
#include <QApplication>

class QmlReloader : public QObject
{
    Q_OBJECT

    // QML engine
    QPointer<QQmlApplicationEngine> _engine;
    const QUrl _rootUrl = QUrl(u"qrc:/main.qml"_qs);

public:
    QmlReloader(QQmlApplicationEngine* engine, QObject *parent = nullptr)
        : QObject(parent), _engine(engine)
    {
    }

    // Expose a Q_INVOKABLE so we can call it from Java via QtNative.runOnQtThread()
    Q_INVOKABLE void reload()
    {
        if (!_engine) {
            qWarning() << "[QmlReloader] Engine is null—creating new engine.";
            _engine = new QQmlApplicationEngine;
            connect(
                _engine, &QQmlApplicationEngine::objectCreated,
                  [&](QObject *obj, const QUrl &url) {
                    if (!obj && url == _rootUrl) {
                        QApplication::exit(-1);
                    }
                });
        } else {
            qDebug() << "[QmlReloader] clearComponentCache() + reload " << _rootUrl;
            _engine->clearComponentCache();
        }

        // (Re)load QML in either case:
        _engine->load(_rootUrl);
    }
};

