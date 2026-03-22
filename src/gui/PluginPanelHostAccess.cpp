#include "PluginPanelHostAccess.h"

#include "DockWidgetPluginPanel.h"
#include "MainWindow.h"
#include <QGroupBox>
#include <QWidget>

namespace {
DockWidgetPluginPanel* pluginPanelHost()
{
    if (auto win = CMainWindow::instance()) {
        return win->dockPluginPanel;
    }
    return nullptr;
}
}

QWidget* PluginPanelHostAccess::panel(unsigned int pluginId)
{
    if (auto panel = pluginPanelHost()) {
        return static_cast<QWidget*>(panel->getPanel(pluginId));
    }
    return nullptr;
}

void PluginPanelHostAccess::create(unsigned int pluginId, const QString& label)
{
    if (auto panel = pluginPanelHost()) {
        panel->addPluginPanel(pluginId, label);
    }
}

void PluginPanelHostAccess::clear(unsigned int pluginId)
{
    if (auto widget = panel(pluginId)) {
        while (QWidget* child = widget->findChild<QWidget*>()) {
            delete child;
        }
    }
}

void PluginPanelHostAccess::setEnabled(unsigned int pluginId, bool enabled)
{
    if (auto widget = panel(pluginId)) {
        widget->setEnabled(enabled);
    }
}

void PluginPanelHostAccess::removeWidget(unsigned int pluginId, const QString& name)
{
    if (auto panel = pluginPanelHost()) {
        panel->removeWidget(pluginId, name);
    }
}

QPushButton* PluginPanelHostAccess::addButton(
    unsigned int pluginId,
    const QString& name,
    const QString& label,
    int row,
    int col,
    int rspan,
    int cspan)
{
    if (auto panel = pluginPanelHost()) {
        return panel->addButton(pluginId, name, label, row, col, rspan, cspan);
    }
    return nullptr;
}

void PluginPanelHostAccess::setButtonText(unsigned int pluginId, const QString& name, const QString& value)
{
    if (auto panel = pluginPanelHost()) {
        panel->setButtonText(pluginId, name, value);
    }
}

void PluginPanelHostAccess::addSlider(
    unsigned int pluginId,
    const QString& name,
    int min,
    int max,
    int row,
    int col,
    int rspan,
    int cspan)
{
    if (auto panel = pluginPanelHost()) {
        panel->addSlider(pluginId, name, min, max, row, col, rspan, cspan);
    }
}

int PluginPanelHostAccess::getSliderValue(unsigned int pluginId, const QString& name)
{
    if (auto panel = pluginPanelHost()) {
        return panel->getSliderValue(pluginId, name);
    }
    return -1;
}

int PluginPanelHostAccess::setSliderValue(unsigned int pluginId, const QString& name, int value)
{
    if (auto panel = pluginPanelHost()) {
        return panel->setSliderValue(pluginId, name, value);
    }
    return -1;
}

void PluginPanelHostAccess::setSliderRange(unsigned int pluginId, const QString& name, int min, int max)
{
    if (auto panel = pluginPanelHost()) {
        panel->setSliderRange(pluginId, name, min, max);
    }
}

void PluginPanelHostAccess::addEditBox(
    unsigned int pluginId,
    const QString& name,
    const QString& label,
    const QString& value,
    int row,
    int col,
    int rspan,
    int cspan)
{
    if (auto panel = pluginPanelHost()) {
        panel->addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
    }
}

QString PluginPanelHostAccess::getEditBoxValue(unsigned int pluginId, const QString& name)
{
    if (auto panel = pluginPanelHost()) {
        return panel->getEditBoxValue(pluginId, name);
    }
    return "";
}

void PluginPanelHostAccess::setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value)
{
    if (auto panel = pluginPanelHost()) {
        panel->setEditBoxValue(pluginId, name, value);
    }
}

void PluginPanelHostAccess::addComboBox(
    unsigned int pluginId,
    const QString& name,
    int row,
    int col,
    int rspan,
    int cspan)
{
    if (auto panel = pluginPanelHost()) {
        panel->addComboBox(pluginId, name, row, col, rspan, cspan);
    }
}

QString PluginPanelHostAccess::getComboBoxCurrentItemText(unsigned int pluginId, const QString& name)
{
    if (auto panel = pluginPanelHost()) {
        return panel->getComboBoxCurrentItemText(pluginId, name);
    }
    return "";
}

void PluginPanelHostAccess::setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items)
{
    if (auto panel = pluginPanelHost()) {
        panel->setComboBoxItems(pluginId, name, items);
    }
}

void PluginPanelHostAccess::addLabel(
    unsigned int pluginId,
    const QString& name,
    const QString& text,
    int row,
    int col,
    int rspan,
    int cspan)
{
    if (auto panel = pluginPanelHost()) {
        panel->addLabel(pluginId, name, text, row, col, rspan, cspan);
    }
}

void PluginPanelHostAccess::setLabel(unsigned int pluginId, const QString& name, const QString& text)
{
    if (auto panel = pluginPanelHost()) {
        panel->setLabel(pluginId, name, text);
    }
}
