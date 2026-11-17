#pragma once

#include "../interfaces/IPluginPanelAPI.h"
#include "../../core/PluginPanelManager.h"
#include <QString>
#include <QStringList>
#include <QWidget>

class PluginPanelAPIAdapter : public IPluginPanelAPI {
public:
    QWidget* panel(unsigned int pluginId) override {
        return PluginPanelManager::panel(pluginId);
    }

    void create(unsigned int pluginId, const QString& label) override {
        PluginPanelManager::create(pluginId, label);
    }

    void clear(unsigned int pluginId) override {
        PluginPanelManager::clear(pluginId);
    }

    void setEnabled(unsigned int pluginId, bool enabled) override {
        PluginPanelManager::setEnabled(pluginId, enabled);
    }

    void removeWidget(unsigned int pluginId, const QString& name) override {
        PluginPanelManager::removeWidget(pluginId, name);
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        return PluginPanelManager::addButton(pluginId, name, label, row, col, rspan, cspan);
    }

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override {
        PluginPanelManager::setButtonText(pluginId, name, value);
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
        PluginPanelManager::addSlider(pluginId, name, min, max, row, col, rspan, cspan);
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override {
        return PluginPanelManager::getSliderValue(pluginId, name);
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override {
        return PluginPanelManager::setSliderValue(pluginId, name, value);
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override {
        PluginPanelManager::setSliderRange(pluginId, name, min, max);
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
        PluginPanelManager::addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override {
        return PluginPanelManager::getEditBoxValue(pluginId, name);
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override {
        PluginPanelManager::setEditBoxValue(pluginId, name, value);
    }

    void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        PluginPanelManager::addComboBox(pluginId, name, row, col, rspan, cspan);
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override {
        return PluginPanelManager::getComboBoxCurrentItemText(pluginId, name);
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override {
        PluginPanelManager::setComboBoxItems(pluginId, name, items);
    }

    void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        PluginPanelManager::addLabel(pluginId, name, text, row, col, rspan, cspan);
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override {
        PluginPanelManager::setLabel(pluginId, name, text);
    }
};
