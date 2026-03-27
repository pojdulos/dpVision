#include "MainApplication.h"

#include "../api/adapters/AppAPIAdapter.h"
#include "../core/RemoteCommandDispatcher.h"
#include "TcpCommandServer.h"

namespace
{
    class ApplicationRemoteCommandHost final : public IRemoteCommandHost
    {
    public:
        std::shared_ptr<CModel3D> loadModel(const QString& filename) override
        {
            static AppAPIAdapter api;
            return api.workspaceImport().loadModel(filename);
        }

        std::shared_ptr<CModel3D> loadDicom(const QString& filename) override
        {
            static AppAPIAdapter api;
            return api.workspaceImport().loadModel("*.dcm", filename);
        }

        bool runPlugin(const QString& uuid) override
        {
            if (auto app = static_cast<CMainApplication*>(QApplication::instance()))
            {
                const QByteArray uuidBytes = uuid.toLocal8Bit();
                return app->runPlugin(uuidBytes.constData());
            }
            return false;
        }
    };
}

void CMainApplication::startRemoteCommandServer()
{
    if (remoteCommandServer != nullptr)
    {
        return;
    }

    std::shared_ptr<IRemoteCommandHost> host = std::make_shared<ApplicationRemoteCommandHost>();
    std::shared_ptr<RemoteCommandDispatcher> dispatcher = std::make_shared<RemoteCommandDispatcher>(host);

    TcpCommandServer* server = new TcpCommandServer(dispatcher, this);
    connect(server, &TcpCommandServer::quitRequested, this, []() { QApplication::quit(); });
    server->start();
    remoteCommandServer = server;
}
