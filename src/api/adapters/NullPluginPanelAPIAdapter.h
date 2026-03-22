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

    void removeWidget(unsigned int pluginId, const QString& name) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
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

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(value);
    }

    void addSlider(
        unsigned int pluginId,
        const QString& name,
        int min,
        int max,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(min);
        Q_UNUSED(max);
        Q_UNUSED(row);
        Q_UNUSED(col);
        Q_UNUSED(rspan);
        Q_UNUSED(cspan);
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        return -1;
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(value);
        return -1;
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(min);
        Q_UNUSED(max);
    }

    void addEditBox(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        const QString& value,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(label);
        Q_UNUSED(value);
        Q_UNUSED(row);
        Q_UNUSED(col);
        Q_UNUSED(rspan);
        Q_UNUSED(cspan);
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        return "";
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(value);
    }

    void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(row);
        Q_UNUSED(col);
        Q_UNUSED(rspan);
        Q_UNUSED(cspan);
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        return "";
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(items);
    }

    void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(text);
        Q_UNUSED(row);
        Q_UNUSED(col);
        Q_UNUSED(rspan);
        Q_UNUSED(cspan);
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override {
        Q_UNUSED(pluginId);
        Q_UNUSED(name);
        Q_UNUSED(text);
    }
};
