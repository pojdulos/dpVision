#include "RemoteCommandDispatcher.h"

#include "Model3D.h"
#include "Utilities.h"
#include "Workspace.h"

RemoteCommandDispatcher::RemoteCommandDispatcher(std::shared_ptr<IRemoteCommandHost> host)
    : host_(std::move(host))
{
}

RemoteCommandResult RemoteCommandDispatcher::dispatch(const QByteArray& receivedData) const
{
    RemoteCommandResult result;

    const QByteArrayList cmdline = receivedData.simplified().split(' ');
    if (cmdline.isEmpty())
    {
        result.response = "Nie rozumiem polecenia.\n\r";
        return result;
    }

    const QString cmd(cmdline.first());

    if (cmd.startsWith("loadDICOM"))
    {
        if (cmdline.size() < 2)
        {
            result.response = "Brak nazwy pliku.\n\r";
            return result;
        }

        const QString filename(cmdline.at(1));
        std::shared_ptr<CModel3D> obj = host_ ? host_->loadDicom(filename) : nullptr;
        result.response = (obj != nullptr)
            ? QString::number(obj->id()) + "\n\r"
            : "Przykro mi. Cos poszlo nie tak...\n\r";
        return result;
    }

    if (cmd.startsWith("load"))
    {
        if (cmdline.size() < 2)
        {
            result.response = "Brak nazwy pliku.\n\r";
            return result;
        }

        const QString filename(cmdline.at(1));
        std::shared_ptr<CModel3D> obj = host_ ? host_->loadModel(filename) : nullptr;
        result.response = (obj != nullptr)
            ? QString::number(obj->id()) + "\n\r"
            : "Przykro mi. Cos poszlo nie tak...\n\r";
        return result;
    }

    if (cmd.startsWith("list"))
    {
        QString response;
        for (auto& x : *CWorkspace::instance())
        {
            if (x.second != nullptr)
            {
                response += QString::number(x.second->id()) + " " + x.second->getLabel() + " " + x.second->path() + "\n\r";
            }
        }
        result.response = response;
        return result;
    }

    if (cmd.startsWith("rotate"))
    {
        if (cmdline.size() < 3)
        {
            result.response = "Niepoprawna skladnia polecenia.\n\r";
            return result;
        }

        const int id = QString(cmdline.at(1)).toInt();
        const double rX = QString(cmdline.at(2)).toDouble();

        std::shared_ptr<CModel3D> obj = std::dynamic_pointer_cast<CModel3D>(CWorkspace::instance()->getSomethingWithId(id));
        if (obj != nullptr)
        {
            obj->getTransform().rotateAroundAxisDeg(CVector3d::XAxis(), rX);
            CWorkspace::instance()->notifyObjectStateChanged(obj->id());
            result.response = "Gotowe.\n\r";
        }
        else
        {
            result.response = "Przykro mi. Cos poszlo nie tak...\n\r";
        }
        return result;
    }

    if (cmd.startsWith("remove"))
    {
        if (cmdline.size() < 2)
        {
            result.response = "Niepoprawna skladnia polecenia.\n\r";
            return result;
        }

        const int id = QString(cmdline.at(1)).toInt();
        std::shared_ptr<CModel3D> obj = std::dynamic_pointer_cast<CModel3D>(CWorkspace::instance()->getSomethingWithId(id));
        if (obj != nullptr)
        {
            CWorkspace::instance()->_objectRemove(id);
            result.response = "Gotowe.\n\r";
        }
        else
        {
            result.response = "Przykro mi. Cos poszlo nie tak...\n\r";
        }
        return result;
    }

    if (cmd.startsWith("plugin"))
    {
        if (cmdline.size() <= 2)
        {
            result.response = "Niepoprawna skladnia polecenia, napisales: %1 \n\r" + QString::fromLocal8Bit(receivedData) + "\n\r";
            return result;
        }

        if (cmdline.at(1).startsWith("run"))
        {
            result.response = (host_ && host_->runPlugin(cmdline.at(2)))
                ? "Wtyczka zostala uruchomiona.\n\r"
                : "Nie znaleziono wtyczki o podanym UUID.\n\r";
        }
        else
        {
            result.response = "Niepoprawna skladnia polecenia, napisales: %1 \n\r" + QString::fromLocal8Bit(receivedData) + "\n\r";
        }
        return result;
    }

    if (cmd.startsWith("quit"))
    {
        result.response = "Bye!!!\n\r";
        result.quitRequested = true;
        return result;
    }

    result.response = "Nie rozumiem polecenia, napisales: %1 \n\r" + QString::fromLocal8Bit(receivedData) + "\n\r";
    return result;
}
