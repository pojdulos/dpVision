#pragma once

#include "../interfaces/IStatusBarAPI.h"
#include "../../core/StatusBarManager.h"

class StatusBarAPIAdapter : public IStatusBarAPI {
public:
    void setText(const QString& message) override {
        StatusBarManager::setText(message);
    }

    void setTimedText(int mst, const QString& message) override {
        StatusBarManager::setTextTimed(mst, message);
    }
};
