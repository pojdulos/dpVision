#pragma once

#include <memory>

#include <QObject>

class QTcpServer;

class RemoteCommandDispatcher;

class TcpCommandServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpCommandServer(std::shared_ptr<RemoteCommandDispatcher> dispatcher, QObject* parent = nullptr);

    bool start(quint16 port = 49199);

signals:
    void quitRequested();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer* server_ = nullptr;
    std::shared_ptr<RemoteCommandDispatcher> dispatcher_;
};
