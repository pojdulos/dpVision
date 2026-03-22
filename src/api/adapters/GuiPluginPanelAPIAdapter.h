#pragma once

#include "../../gui/PluginPanelHostAccess.h"
#include "../interfaces/IPluginPanelAPI.h"
#include <QString>
#include <QStringList>
#include <QWidget>

class GuiPluginPanelAPIAdapter : public IPluginPanelAPI {
public:
    QWidget* panel(unsigned int pluginId) override {
        return PluginPanelHostAccess::panel(pluginId);
    }

    void create(unsigned int pluginId, const QString& label) override {
        PluginPanelHostAccess::create(pluginId, label);
    }

    void clear(unsigned int pluginId) override {
        PluginPanelHostAccess::clear(pluginId);
    }

    void setEnabled(unsigned int pluginId, bool enabled) override {
        PluginPanelHostAccess::setEnabled(pluginId, enabled);
    }

    void removeWidget(unsigned int pluginId, const QString& name) override {
        PluginPanelHostAccess::removeWidget(pluginId, name);
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        return PluginPanelHostAccess::addButton(pluginId, name, label, row, col, rspan, cspan);
    }

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override {
        PluginPanelHostAccess::setButtonText(pluginId, name, value);
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
        PluginPanelHostAccess::addSlider(pluginId, name, min, max, row, col, rspan, cspan);
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override {
        return PluginPanelHostAccess::getSliderValue(pluginId, name);
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override {
        return PluginPanelHostAccess::setSliderValue(pluginId, name, value);
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override {
        PluginPanelHostAccess::setSliderRange(pluginId, name, min, max);
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
        PluginPanelHostAccess::addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override {
        return PluginPanelHostAccess::getEditBoxValue(pluginId, name);
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override {
        PluginPanelHostAccess::setEditBoxValue(pluginId, name, value);
    }

    void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        PluginPanelHostAccess::addComboBox(pluginId, name, row, col, rspan, cspan);
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override {
        return PluginPanelHostAccess::getComboBoxCurrentItemText(pluginId, name);
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override {
        PluginPanelHostAccess::setComboBoxItems(pluginId, name, items);
    }

    void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        PluginPanelHostAccess::addLabel(pluginId, name, text, row, col, rspan, cspan);
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override {
        PluginPanelHostAccess::setLabel(pluginId, name, text);
    }
};
