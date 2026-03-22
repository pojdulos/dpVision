#pragma once

#include "../interfaces/IProgressAPI.h"

class NullProgressAPIAdapter : public IProgressAPI {
public:
    ProgressIndicator* instance() override {
        return nullptr;
    }

    void init(int min, int max, int val) override {
        Q_UNUSED(min);
        Q_UNUSED(max);
        Q_UNUSED(val);
    }

    void setValue(int val) override {
        Q_UNUSED(val);
    }

    void hide() override {
    }

    void setText(const QString& text) override {
        Q_UNUSED(text);
    }
};
