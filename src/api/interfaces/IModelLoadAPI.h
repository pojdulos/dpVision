#pragma once

#include <memory>
#include <string>

class CModel3D;

class IModelLoadAPI {
public:
    virtual ~IModelLoadAPI() = default;

    virtual std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous = true) = 0;
};
