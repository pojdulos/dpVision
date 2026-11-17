#pragma once

#include "dll_global.h"
#include <QString>
#include <QStringList>

class QObject;
class DockWidgetPluginPanel;
class QPushButton;
class QWidget;

class DPVISION_EXPORT PluginPanelHostAccess {
public:
    static DockWidgetPluginPanel* host();
    static QWidget* panel(unsigned int pluginId);
    static void create(unsigned int pluginId, const QString& label);
    static void clear(unsigned int pluginId);
    static void setEnabled(unsigned int pluginId, bool enabled);
    static void removeWidget(unsigned int pluginId, const QString& name);
    static QPushButton* addButton(
        unsigned int pluginId,
        const QString& label,
        QObject* receiver,
        const char* slot,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static void setButtonText(unsigned int pluginId, const QString& name, const QString& value);
    static void addSlider(
        unsigned int pluginId,
        const QString& name,
        int min,
        int max,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static int getSliderValue(unsigned int pluginId, const QString& name);
    static int setSliderValue(unsigned int pluginId, const QString& name, int value);
    static void setSliderRange(unsigned int pluginId, const QString& name, int min, int max);
    static void addEditBox(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        const QString& value,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static QString getEditBoxValue(unsigned int pluginId, const QString& name);
    static void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value);
    static void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name);
    static void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items);
    static void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0);
    static void setLabel(unsigned int pluginId, const QString& name, const QString& text);
};
