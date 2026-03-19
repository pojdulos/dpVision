// AppAPIAdapter.h
#pragma once
#include "../api/AP.h"
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"

class AppAPIAdapter : public IAppAPI {
    WorkspaceAPIAdapter workspaceAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
public:
    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
    int uniqueId() override { return AP::getUniqueId(); }
    const QString& exeFilePath() override { return AP::getExeFilePath(); }
    void adjustForCurrentFile(const QString& filePath) override { AP::adjustForCurrentFile(filePath); }
};
