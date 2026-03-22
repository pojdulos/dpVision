#pragma once

#include "ProgressHostAccess.h"
#include "../interfaces/IProgressControlAPI.h"
#include "../interfaces/IProgressAPI.h"

class GuiProgressAPIAdapter : public IProgressAPI, public IProgressControlAPI {
public:
    ProgressIndicator* instance() override {
        return ProgressHostAccess::instance();
    }

    void init(int min, int max, int val) override {
        ProgressHostAccess::init(min, max, val);
    }

    void setValue(int val) override {
        ProgressHostAccess::setValue(val);
    }

    void hide() override {
        ProgressHostAccess::hide();
    }

    void setText(const QString& text) override {
        ProgressHostAccess::setText(text);
    }
};
