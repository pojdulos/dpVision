// AppAPIAdapter.h
#pragma once
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"
#include "../../gui/MainApplication.h"
#include "Global.h"
#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <atomic>

class AppAPIAdapter : public IAppAPI {
    WorkspaceAPIAdapter workspaceAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
public:
    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
    int uniqueId() override {
        static std::atomic<int> nextId{MODEL_ID_OFFSET};
        return ++nextId;
    }
    const QString& exeFilePath() override {
        static QString exeDir = QCoreApplication::applicationDirPath();
        return exeDir;
    }
    void setStatusText(const QString& text) override {
        Q_UNUSED(text);
    }
    void updateProperties() override {
    }
    void updateAllViews(bool buffered = true) override {
        Q_UNUSED(buffered);
    }
    void adjustForCurrentFile(const QString& filePath) override {
        Q_UNUSED(filePath);
    }
};
