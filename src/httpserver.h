#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QMap>
#include <QDebug>

class HttpServer: public QObject
{
    Q_OBJECT

    QTcpServer *server = 0;

    bool listenError = false;
    QString m_port;

    static HttpServer *instance;

public:
    explicit HttpServer(QObject *parent = 0);

    static HttpServer *getInstance();

    QString port();

    bool isOk() const;

public slots:
    // starts server
    void start();

    // destroys server
    void stop();

    void onConnect();

    void onRead();

signals:
    void codeReceived(QString code, int type);
    void error();
};

#endif // HTTPSERVER_H
