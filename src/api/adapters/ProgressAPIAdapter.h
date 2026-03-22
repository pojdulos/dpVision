#pragma once

#include "../interfaces/IProgressAPI.h"
#include "../UI.h"
#include "../../core/interfaces/IProgressListener.h"

class ProgressAPIAdapter : public IProgressAPI {
public:
    ProgressIndicator* instance() override {
        return UI::PROGRESSBAR::instance();
    }

    void init(int min, int max, int val) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->init(min, max, val);
        } else {
            UI::PROGRESSBAR::init(min, max, val);
        }
    }

    void setValue(int val) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->setValue(val);
        } else {
            UI::PROGRESSBAR::setValue(val);
        }
    }

    void hide() override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->hide();
        } else {
            UI::PROGRESSBAR::hide();
        }
    }

    void setText(const QString& text) override {
        if (auto listener = IProgressListener::getDefault()) {
            listener->setText(text.toStdString());
        } else {
            UI::PROGRESSBAR::setText(text);
        }
    }
};
