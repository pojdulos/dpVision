#pragma once

#include "../../core/interfaces/IProgressListener.h"
#include "../interfaces/IProgressControlAPI.h"

// Narrow progress contract used by plugin-facing APIs.
// The actual host implementation comes from the currently registered
// IProgressListener so the safe progress path does not depend on raw GUI types.
class GuiProgressAPIAdapter : public IProgressControlAPI {
public:
    void init(int min, int max, int val) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->init(min, max, val);
        }
    }

    void setValue(int val) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->setValue(val);
        }
    }

    void hide() override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->hide();
        }
    }

    void setText(const QString& text) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->setText(text.toStdString());
        }
    }
};
