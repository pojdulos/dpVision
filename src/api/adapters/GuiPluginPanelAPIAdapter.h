#pragma once

#include "../interfaces/IPluginPanelAPI.h"
#include "../../api/AP.h"
#include "../../gui/MainWindow.h"
#include "../../gui/DockWidgetPluginPanel.h"
#include <QString>
#include <QWidget>

class GuiPluginPanelAPIAdapter : public IPluginPanelAPI {
    static DockWidgetPluginPanel* pluginPanel()
    {
        if (auto win = AP::mainWinPtr()) {
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
};
