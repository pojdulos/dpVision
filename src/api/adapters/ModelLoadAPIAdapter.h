#pragma once

#include "../interfaces/IModelLoadAPI.h"
#include "../../core/Model3D.h"

class ModelLoadAPIAdapter : public IModelLoadAPI {
public:
    std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous = true) override
    {
        return CModel3D::load(std::move(path), synchronous);
    }
};
