#pragma once

class QString;

class IProgressControlAPI {
public:
    virtual ~IProgressControlAPI() = default;

    virtual void init(int min, int max, int val) = 0;
    virtual void setValue(int val) = 0;
    virtual void hide() = 0;
    virtual void setText(const QString& text) = 0;
};
