#pragma once

#include <memory>

class CModel3D;

class IWorkspaceDuplicationAPI {
public:
    virtual ~IWorkspaceDuplicationAPI() = default;

    virtual std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> original) = 0;
    virtual std::shared_ptr<CModel3D> duplicateModel(int id) = 0;
    virtual std::shared_ptr<CModel3D> duplicateCurrentModel() = 0;
};
