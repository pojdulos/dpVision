#pragma once

#include <memory>

class CImage;

class IWorkspaceImageAPI {
public:
    virtual ~IWorkspaceImageAPI() = default;

    virtual bool addImage(std::shared_ptr<CImage> image, bool show3d = false) = 0;
};
