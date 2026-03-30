#pragma once

#include "dll_global.h"

#include <memory>

#include <QByteArray>
#include <QString>

class CModel3D;

class DPVISION_EXPORT IRemoteCommandHost
{
public:
    virtual ~IRemoteCommandHost() = default;

    virtual std::shared_ptr<CModel3D> loadModel(const QString& filename) = 0;
    virtual std::shared_ptr<CModel3D> loadDicom(const QString& filename) = 0;
    virtual bool runPlugin(const QString& uuid) = 0;
};

struct DPVISION_EXPORT RemoteCommandResult
{
    QString response;
    bool quitRequested = false;
};

class DPVISION_EXPORT RemoteCommandDispatcher
{
public:
    explicit RemoteCommandDispatcher(std::shared_ptr<IRemoteCommandHost> host);

    RemoteCommandResult dispatch(const QByteArray& receivedData) const;

private:
    std::shared_ptr<IRemoteCommandHost> host_;
};
