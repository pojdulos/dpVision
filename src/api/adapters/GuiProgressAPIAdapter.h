#pragma once

#include "../../gui/ProgressHostAccess.h"
#include "../interfaces/IProgressControlAPI.h"

// Narrow progress contract used by explicit GUI-capable host APIs.
// Public plugin compatibility should go through UI::PROGRESSBAR.
class GuiProgressAPIAdapter : public IProgressControlAPI {
public:
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
