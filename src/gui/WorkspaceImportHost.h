#pragma once

#include "dll_global.h"
#include <memory>

class CModel3D;
class IProgressListener;
class QString;

class DPVISION_EXPORT WorkspaceImportHost {
public:
    static std::shared_ptr<CModel3D> loadModel(
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false);

    static std::shared_ptr<CModel3D> loadModel(
        const QString& path,
        bool synchronous,
        bool setCurrent,
        std::shared_ptr<IProgressListener> progress);

    static std::shared_ptr<CModel3D> loadModel(
        const QString& fileExtension,
        const QString& path,
        bool synchronous = true,
        bool setCurrent = false);
};
