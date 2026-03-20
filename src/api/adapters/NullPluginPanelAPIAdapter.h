#pragma once

#include "../interfaces/IPluginPanelAPI.h"

class NullPluginPanelAPIAdapter : public IPluginPanelAPI {
public:
    QWidget* panel(unsigned int pluginId) override {
        Q_UNUSED(pluginId);
        return nullptr;
    }

    void create(unsigned int pluginId, const QString& label) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(label);
    }

    void clear(unsigned int pluginId) override {
        Q_UNUSED(pluginId);
    }

    void setEnabled(unsigned int pluginId, bool enabled) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(enabled);
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(label);
        Q_UNUSED(row);
        Q_UNUSED(col);
        Q_UNUSED(rspan);
        Q_UNUSED(cspan);
        return nullptr;
    }
};
