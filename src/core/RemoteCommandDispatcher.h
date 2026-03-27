#pragma once

#include <memory>

#include <QByteArray>
#include <QString>

class CModel3D;

class IRemoteCommandHost
{
public:
    virtual ~IRemoteCommandHost() = default;

    virtual std::shared_ptr<CModel3D> loadModel(const QString& filename) = 0;
    virtual std::shared_ptr<CModel3D> loadDicom(const QString& filename) = 0;
    virtual bool runPlugin(const QString& uuid) = 0;
};

struct RemoteCommandResult
{
    QString response;
    bool quitRequested = false;
};

class RemoteCommandDispatcher
{
public:
    explicit RemoteCommandDispatcher(std::shared_ptr<IRemoteCommandHost> host);

    RemoteCommandResult dispatch(const QByteArray& receivedData) const;

private:
    std::shared_ptr<IRemoteCommandHost> host_;
};
