#pragma once

#include "../interfaces/IPluginPanelAPI.h"
#include "../UI.h"
#include <QString>
#include <QStringList>
#include <QWidget>

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

    void removeWidget(unsigned int pluginId, const QString& name) override {
        UI::PLUGINPANEL::removeWidget(pluginId, name);
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        return UI::PLUGINPANEL::addButton(
            pluginId,
            name.toStdString(),
            label.toStdString(),
            row,
            col,
            rspan,
            cspan);
    }

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override {
        UI::PLUGINPANEL::setButtonText(pluginId, name, value);
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
        UI::PLUGINPANEL::addSlider(pluginId, name, min, max, row, col, rspan, cspan);
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override {
        return UI::PLUGINPANEL::getSliderValue(pluginId, name);
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override {
        return UI::PLUGINPANEL::setSliderValue(pluginId, name, value);
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override {
        UI::PLUGINPANEL::setSliderRange(pluginId, name, min, max);
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
        UI::PLUGINPANEL::addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override {
        return UI::PLUGINPANEL::getEditBoxValue(pluginId, name);
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override {
        UI::PLUGINPANEL::setEditBoxValue(pluginId, name, value);
    }

    void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        UI::PLUGINPANEL::addComboBox(pluginId, name, row, col, rspan, cspan);
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override {
        return UI::PLUGINPANEL::getComboBoxCurrentItemText(pluginId, name);
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override {
        UI::PLUGINPANEL::setComboBoxItems(pluginId, name, items);
    }

    void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        UI::PLUGINPANEL::addLabel(pluginId, name, text, row, col, rspan, cspan);
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override {
        UI::PLUGINPANEL::setLabel(pluginId, name, text);
    }
};
