#pragma once

#include "IWorkspaceAPI.h"
#include "IWorkspaceActivationAPI.h"
#include "IWorkspaceBulkAPI.h"
#include "IWorkspaceDuplicationAPI.h"
#include "IWorkspaceImageAPI.h"
#include "IWorkspaceImportAPI.h"
#include "IWorkspaceSelectionAPI.h"
#include "IModelLoadAPI.h"
#include "IModelAPI.h"
#include "IObjectAPI.h"
#include "ISettingsAPI.h"

class QString;

class IAppAPI {
public:
    virtual ~IAppAPI() = default;
    virtual IWorkspaceAPI& workspace() = 0;
    virtual IWorkspaceActivationAPI& workspaceActivation() = 0;
    virtual IWorkspaceBulkAPI& workspaceBulk() = 0;
    virtual IWorkspaceDuplicationAPI& workspaceDuplication() = 0;
    virtual IWorkspaceImageAPI& workspaceImage() = 0;
    virtual IWorkspaceImportAPI& workspaceImport() = 0;
    virtual IWorkspaceSelectionAPI& workspaceSelection() = 0;
    virtual IModelLoadAPI& modelLoad() = 0;
    virtual IModelAPI& model() = 0;
    virtual IObjectAPI& object() = 0;
    virtual ISettingsAPI& settings() = 0;
    virtual ISettingsAPI& pluginSettings(const QString& pluginId) = 0;
    virtual int uniqueId() = 0;
    virtual const QString& exeFilePath() = 0;
    virtual void setStatusText(const QString& text) = 0;
    virtual void updateProperties() = 0;
    virtual void updateAllViews(bool buffered = true) = 0;
    virtual void adjustForCurrentFile(const QString& filePath) = 0;
};
