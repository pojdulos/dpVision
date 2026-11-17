#pragma once

#include <memory>

class CModel3D;
class QString;
class IProgressListener;

class IWorkspaceImportAPI {
public:
    virtual ~IWorkspaceImportAPI() = default;

    virtual std::shared_ptr<CModel3D> loadModel(
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false,
        std::shared_ptr<IProgressListener> progress = nullptr) = 0;

    virtual std::shared_ptr<CModel3D> loadModel(
        const QString& fileExtension,
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false) = 0;
};
