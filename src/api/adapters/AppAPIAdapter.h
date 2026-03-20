// AppAPIAdapter.h
#pragma once
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"
#include "../adapters/SettingsAPIAdapter.h"
#include "../../core/StatusBarManager.h"
#include "../../core/AppStateManager.h"
#include "Global.h"
#include <QtCore/QCoreApplication>
#include <atomic>

class AppAPIAdapter : public IAppAPI {
    WorkspaceAPIAdapter workspaceAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
    SettingsAPIAdapter settingsAPI_;
public:
    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
    ISettingsAPI& settings() override { return settingsAPI_; }
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
