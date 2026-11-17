#pragma once

#include "../interfaces/IPluginHostAPI.h"
#include "FileDialogAPIAdapter.h"
#include "FileSystemAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "TextEncodingAPIAdapter.h"
#include "../../core/AppStateManager.h"

class PluginHostAPIAdapter : public IPluginHostAPI {
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    FileSystemAPIAdapter fileSystemAdapter_;
    TextEncodingAPIAdapter textEncodingAdapter_;

public:
    IFileDialogAPI& fileDialog() override { return fileDialogAdapter_; }
    IMessageBoxAPI& messageBox() override { return messageBoxAdapter_; }
    IStatusBarAPI& statusBar() override { return statusBarAdapter_; }
    IFileSystemAPI& fileSystem() override { return fileSystemAdapter_; }
    ITextEncodingAPI& textEncoding() override { return textEncodingAdapter_; }

    void updateAllViews(bool buffered = true) override {
        AppStateManager::updateAllViews(buffered);
    }

    void updateProperties() override {
        AppStateManager::updateProperties();
    }
};
