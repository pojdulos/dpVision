// AppAPIAdapter.h
#pragma once
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceBulkAPIAdapter.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/WorkspaceSelectionAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"
#include "../adapters/SettingsAPIAdapter.h"
#include "../../core/StatusBarManager.h"
#include "../../core/AppSettings.h"
#include "../../core/AppStateManager.h"
#include "Global.h"
#include <QtCore/QCoreApplication>
#include <atomic>
#include <memory>

class AppAPIAdapter : public IAppAPI {
    WorkspaceAPIAdapter workspaceAPI_;
    WorkspaceBulkAPIAdapter workspaceBulkAPI_;
    WorkspaceSelectionAPIAdapter workspaceSelectionAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
    SettingsAPIAdapter settingsAPI_;
    std::unique_ptr<QSettings> pluginSettingsStorage_;
    SettingsAPIAdapter pluginSettingsAPI_;
public:
    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IWorkspaceBulkAPI& workspaceBulk() override { return workspaceBulkAPI_; }
    IWorkspaceSelectionAPI& workspaceSelection() override { return workspaceSelectionAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
    ISettingsAPI& settings() override { return settingsAPI_; }
    ISettingsAPI& pluginSettings(const QString& pluginId) override {
        pluginSettingsStorage_ = AppSettings::pluginSettings(pluginId);
        pluginSettingsAPI_.reset(pluginSettingsStorage_.get());
        return pluginSettingsAPI_;
    }
    int uniqueId() override {
        static std::atomic<int> nextId{MODEL_ID_OFFSET};
        return ++nextId;
    }
    const QString& exeFilePath() override {
        static QString exeDir = QCoreApplication::applicationDirPath();
        return exeDir;
    }
    void setStatusText(const QString& text) override {
        StatusBarManager::setText(text);
    }
    void updateProperties() override {
        AppStateManager::updateProperties();
    }
    void updateAllViews(bool buffered = true) override {
        AppStateManager::updateAllViews(buffered);
    }
    void adjustForCurrentFile(const QString& filePath) override {
        AppStateManager::adjustForCurrentFile(filePath);
    }
};
