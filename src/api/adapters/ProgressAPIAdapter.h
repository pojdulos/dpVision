#pragma once

#include "../interfaces/IProgressAPI.h"
#include "../UI.h"

class ProgressAPIAdapter : public IProgressAPI {
public:
    ProgressIndicator* instance() override {
        return UI::PROGRESSBAR::instance();
    }

    void init(int min, int max, int val) override {
        UI::PROGRESSBAR::init(min, max, val);
    }

    void setValue(int val) override {
        UI::PROGRESSBAR::setValue(val);
    }

    void hide() override {
        UI::PROGRESSBAR::hide();
    }

    void setText(const QString& text) override {
        UI::PROGRESSBAR::setText(text);
    }
};
