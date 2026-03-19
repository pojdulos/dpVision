#pragma once

#include "../interfaces/IPluginPanelAPI.h"
#include "../UI.h"

class PluginPanelAPIAdapter : public IPluginPanelAPI {
public:
    QWidget* panel(unsigned int pluginId) override {
        return UI::PLUGINPANEL::instance(pluginId);
    }

    void create(unsigned int pluginId, const QString& label) override {
        UI::PLUGINPANEL::create(pluginId, label);
    }

    void clear(unsigned int pluginId) override {
        UI::PLUGINPANEL::clear(pluginId);
    }

    void setEnabled(unsigned int pluginId, bool enabled) override {
        UI::PLUGINPANEL::setEnabled(pluginId, enabled);
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        return UI::PLUGINPANEL::addButton(pluginId, name.toStdString(), label.toStdString(), row, col, rspan, cspan);
    }
};
