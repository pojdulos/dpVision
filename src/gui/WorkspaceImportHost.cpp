#include "WorkspaceImportHost.h"

#include "../core/interfaces/IProgressListener.h"
#include "../core/Model3D.h"
#include "../core/Workspace.h"

namespace
{
    std::shared_ptr<CModel3D> attachLoadedModel(
        std::shared_ptr<CModel3D> object,
        bool setCurrent)
    {
        if (object != nullptr) {
            CWorkspace* workspace = CWorkspace::instance();
            const int result = workspace->_objectAdd(object);
            if (result != -1) {
                if (setCurrent) {
                    workspace->_objectActivate(object->id());
                }
                return object;
            }
        }

        return nullptr;
    }
}

std::shared_ptr<CModel3D> WorkspaceImportHost::loadModel(
    const QString& path,
    bool synchronous,
    bool setCurrent)
{
    return loadModel(path, synchronous, setCurrent, nullptr);
}

std::shared_ptr<CModel3D> WorkspaceImportHost::loadModel(
    const QString& path,
    bool synchronous,
    bool setCurrent,
    std::shared_ptr<IProgressListener> progress)
{
    return attachLoadedModel(CModel3D::load(path, synchronous, progress), setCurrent);
}

std::shared_ptr<CModel3D> WorkspaceImportHost::loadModel(
    const QString& fileExtension,
    const QString& path,
    bool synchronous,
    bool setCurrent)
{
    return attachLoadedModel(CModel3D::load(fileExtension, path, synchronous), setCurrent);
}
