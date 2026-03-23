#pragma once

#include "../interfaces/IWorkspaceImportAPI.h"
#include "../../core/Workspace.h"
#include "../../core/Model3D.h"

class WorkspaceImportAPIAdapter : public IWorkspaceImportAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceImportAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    std::shared_ptr<CModel3D> loadModel(
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false,
        std::shared_ptr<IProgressListener> progress = nullptr) override
    {
        std::shared_ptr<CModel3D> object = CModel3D::load(path, synchronous, progress);
        if (object != nullptr) {
            const int result = ws_->_objectAdd(object);
            if (result != -1) {
                if (setCurrent) {
                    ws_->_objectActivate(object->id());
                }
                return object;
            }
        }
        return nullptr;
    }

    std::shared_ptr<CModel3D> loadModel(
        const QString& fileExtension,
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false) override
    {
        std::shared_ptr<CModel3D> object = CModel3D::load(fileExtension, path, synchronous);
        if (object != nullptr) {
            const int result = ws_->_objectAdd(object);
            if (result != -1) {
                if (setCurrent) {
                    ws_->_objectActivate(object->id());
                }
                return object;
            }
        }
        return nullptr;
    }
};
