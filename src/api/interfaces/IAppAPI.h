#pragma once

#include "IWorkspaceAPI.h"
#include "IModelAPI.h"
#include "IObjectAPI.h"

class QString;

class IAppAPI {
public:
    virtual ~IAppAPI() = default;
    virtual IWorkspaceAPI& workspace() = 0;
    virtual IModelAPI& model() = 0;
    virtual IObjectAPI& object() = 0;
    virtual int uniqueId() = 0;
    virtual const QString& exeFilePath() = 0;
    virtual void adjustForCurrentFile(const QString& filePath) = 0;
};
