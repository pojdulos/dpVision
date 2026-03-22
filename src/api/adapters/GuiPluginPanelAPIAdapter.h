#pragma once

#include "../interfaces/IPluginPanelAPI.h"
#include "../../gui/MainWindow.h"
#include "../../gui/DockWidgetPluginPanel.h"
#include <QString>
#include <QStringList>
#include <QWidget>

class GuiPluginPanelAPIAdapter : public IPluginPanelAPI {
    static DockWidgetPluginPanel* pluginPanel()
    {
        if (auto win = CMainWindow::instance()) {
            return win->dockPluginPanel;
        }
        return nullptr;
    }

public:
    QWidget* panel(unsigned int pluginId) override {
        if (auto panelWidget = pluginPanel()) {
            return (QWidget*) panelWidget->getPanel(pluginId);
        }
        return nullptr;
    }

    void create(unsigned int pluginId, const QString& label) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->addPluginPanel(pluginId, label);
        }
    }

    void clear(unsigned int pluginId) override {
        if (auto widget = panel(pluginId)) {
            while (QWidget* child = widget->findChild<QWidget*>()) {
                delete child;
            }
        }
    }

    void setEnabled(unsigned int pluginId, bool enabled) override {
        if (auto widget = panel(pluginId)) {
            widget->setEnabled(enabled);
        }
    }

    void removeWidget(unsigned int pluginId, const QString& name) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->removeWidget(pluginId, name);
        }
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        if (auto panelWidget = pluginPanel()) {
            return panelWidget->addButton(pluginId, name, label, row, col, rspan, cspan);
        }
        return nullptr;
    }

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->setButtonText(pluginId, name, value);
        }
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
        if (auto panelWidget = pluginPanel()) {
            panelWidget->addSlider(pluginId, name, min, max, row, col, rspan, cspan);
        }
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override {
        if (auto panelWidget = pluginPanel()) {
            return panelWidget->getSliderValue(pluginId, name);
        }
        return -1;
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override {
        if (auto panelWidget = pluginPanel()) {
            return panelWidget->setSliderValue(pluginId, name, value);
        }
        return -1;
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->setSliderRange(pluginId, name, min, max);
        }
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
        if (auto panelWidget = pluginPanel()) {
            panelWidget->addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
        }
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override {
        if (auto panelWidget = pluginPanel()) {
            return panelWidget->getEditBoxValue(pluginId, name);
        }
        return "";
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->setEditBoxValue(pluginId, name, value);
        }
    }

    void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->addComboBox(pluginId, name, row, col, rspan, cspan);
        }
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override {
        if (auto panelWidget = pluginPanel()) {
            return panelWidget->getComboBoxCurrentItemText(pluginId, name);
        }
        return "";
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->setComboBoxItems(pluginId, name, items);
        }
    }

    void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->addLabel(pluginId, name, text, row, col, rspan, cspan);
        }
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override {
        if (auto panelWidget = pluginPanel()) {
            panelWidget->setLabel(pluginId, name, text);
        }
    }
};
