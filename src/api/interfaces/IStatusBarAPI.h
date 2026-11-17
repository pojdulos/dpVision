#pragma once

class QString;

class IStatusBarAPI {
public:
    virtual ~IStatusBarAPI() = default;
    virtual void setText(const QString& message) = 0;
    virtual void setTimedText(int mst, const QString& message) = 0;
};
