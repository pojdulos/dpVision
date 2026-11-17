#pragma once

class QString;

class IAppStateListener {
public:
    virtual ~IAppStateListener() = default;
    virtual void updateProperties() = 0;
    virtual void updateView(bool repaintAll = true, bool buffered = true) = 0;
    virtual void updateAllViews(bool buffered = true) = 0;
    virtual void adjustForCurrentFile(const QString& filePath) = 0;
    virtual void changeMenuAfterSelect() = 0;
};
