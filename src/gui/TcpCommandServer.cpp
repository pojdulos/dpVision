#include "TcpCommandServer.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "../core/RemoteCommandDispatcher.h"

TcpCommandServer::TcpCommandServer(std::shared_ptr<RemoteCommandDispatcher> dispatcher, QObject* parent)
    : QObject(parent), server_(new QTcpServer(this)), dispatcher_(std::move(dispatcher))
{
    connect(server_, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

bool TcpCommandServer::start(quint16 port)
{
    return server_->listen(QHostAddress::LocalHost, port);
}

void TcpCommandServer::onNewConnection()
{
    while (server_->hasPendingConnections())
    {
        QTcpSocket* socket = server_->nextPendingConnection();
        if (socket != nullptr)
        {
            connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        }
    }
}

void TcpCommandServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr || dispatcher_ == nullptr)
    {
        return;
    }

    const RemoteCommandResult result = dispatcher_->dispatch(socket->readAll());
    socket->write(result.response.toLocal8Bit());

    if (result.quitRequested)
    {
        emit quitRequested();
    }
}
