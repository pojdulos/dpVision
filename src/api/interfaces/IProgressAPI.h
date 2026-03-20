#pragma once

class QString;
class ProgressIndicator;

class IProgressAPI {
public:
    virtual ~IProgressAPI() = default;
    virtual ProgressIndicator* instance() = 0;
    virtual void init(int min, int max, int val) = 0;
    virtual void setValue(int val) = 0;
    virtual void hide() = 0;
    virtual void setText(const QString& text) = 0;
};
