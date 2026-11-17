#pragma once

#include "IFileDialogAPI.h"
#include "IFileSystemAPI.h"
#include "IMessageBoxAPI.h"
#include "IStatusBarAPI.h"
#include "ITextEncodingAPI.h"

class IPluginHostAPI {
public:
    virtual ~IPluginHostAPI() = default;

    virtual IFileDialogAPI& fileDialog() = 0;
    virtual IMessageBoxAPI& messageBox() = 0;
    virtual IStatusBarAPI& statusBar() = 0;
    virtual IFileSystemAPI& fileSystem() = 0;
    virtual ITextEncodingAPI& textEncoding() = 0;

    virtual void updateAllViews(bool buffered = true) = 0;
    virtual void updateProperties() = 0;
};
