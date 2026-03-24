// AppAPIAdapter.h
#pragma once
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceActivationAPIAdapter.h"
#include "../adapters/WorkspaceBulkAPIAdapter.h"
#include "../adapters/WorkspaceDuplicationAPIAdapter.h"
#include "../adapters/WorkspaceImageAPIAdapter.h"
#include "../adapters/WorkspaceImportAPIAdapter.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/WorkspaceSelectionAPIAdapter.h"
#include "../adapters/ModelLoadAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"
#include "../adapters/SettingsAPIAdapter.h"
#include "../../core/interfaces/ISettingsStorage.h"
#include "../../core/StatusBarManager.h"
#include "../../core/AppStateManager.h"
#include "Global.h"
#include <QtCore/QCoreApplication>
#include <atomic>
#include <functional>
#include <memory>

class AppAPIAdapter : public IAppAPI {
public:
    using PluginSettingsFactory = std::function<std::unique_ptr<ISettingsStorage>(const QString&)>;

private:
    WorkspaceAPIAdapter workspaceAPI_;
    WorkspaceActivationAPIAdapter workspaceActivationAPI_;
    WorkspaceBulkAPIAdapter workspaceBulkAPI_;
    WorkspaceDuplicationAPIAdapter workspaceDuplicationAPI_;
    WorkspaceImageAPIAdapter workspaceImageAPI_;
    WorkspaceImportAPIAdapter workspaceImportAPI_;
    WorkspaceSelectionAPIAdapter workspaceSelectionAPI_;
    ModelLoadAPIAdapter modelLoadAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
    SettingsAPIAdapter settingsAPI_;
    std::unique_ptr<ISettingsStorage> pluginSettingsStorage_;
    SettingsAPIAdapter pluginSettingsAPI_;

    static ISettingsStorage*& defaultSettingsStorageRef() {
        static ISettingsStorage* storage = nullptr;
        return storage;
    }

    static PluginSettingsFactory& defaultPluginSettingsFactoryRef() {
        static PluginSettingsFactory factory;
        return factory;
    }

public:
    explicit AppAPIAdapter(
        ISettingsStorage* settingsStorage = nullptr,
        PluginSettingsFactory pluginSettingsFactory = {})
        : settingsAPI_(settingsStorage),
          pluginSettingsAPI_(nullptr)
    {
        if (pluginSettingsFactory) {
            defaultPluginSettingsFactoryRef() = std::move(pluginSettingsFactory);
        }
    }

    static void setDefaultSettingsStorage(ISettingsStorage* storage) {
        defaultSettingsStorageRef() = storage;
    }

    static void setDefaultPluginSettingsFactory(PluginSettingsFactory factory) {
        defaultPluginSettingsFactoryRef() = std::move(factory);
    }

    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IWorkspaceActivationAPI& workspaceActivation() override { return workspaceActivationAPI_; }
    IWorkspaceBulkAPI& workspaceBulk() override { return workspaceBulkAPI_; }
    IWorkspaceDuplicationAPI& workspaceDuplication() override { return workspaceDuplicationAPI_; }
    IWorkspaceImageAPI& workspaceImage() override { return workspaceImageAPI_; }
    IWorkspaceImportAPI& workspaceImport() override { return workspaceImportAPI_; }
    IWorkspaceSelectionAPI& workspaceSelection() override { return workspaceSelectionAPI_; }
    IModelLoadAPI& modelLoad() override { return modelLoadAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
    ISettingsAPI& settings() override {
        if (defaultSettingsStorageRef() != nullptr) {
            settingsAPI_.reset(defaultSettingsStorageRef());
        }
        return settingsAPI_;
    }

    ISettingsAPI& pluginSettings(const QString& pluginId) override {
        if (!defaultPluginSettingsFactoryRef()) {
            pluginSettingsStorage_.reset();
            pluginSettingsAPI_.reset(nullptr);
            return pluginSettingsAPI_;
        }

        pluginSettingsStorage_ = defaultPluginSettingsFactoryRef()(pluginId);
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
