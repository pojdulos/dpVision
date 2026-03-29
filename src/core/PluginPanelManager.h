#pragma once

#include "dll_global.h"
#include "interfaces/IPluginPanelListener.h"

class DPVISION_EXPORT PluginPanelManager {
public:
    static void setListener(IPluginPanelListener* listener) { listenerRef() = listener; }
    static IPluginPanelListener* listener() { return listenerRef(); }

    static QWidget* panel(unsigned int pluginId) {
        return listenerRef() ? listenerRef()->panel(pluginId) : nullptr;
    }

    static void create(unsigned int pluginId, const QString& label) {
        if (listenerRef()) {
            listenerRef()->create(pluginId, label);
        }
    }

    static void clear(unsigned int pluginId) {
        if (listenerRef()) {
            listenerRef()->clear(pluginId);
        }
    }

    static void setEnabled(unsigned int pluginId, bool enabled) {
        if (listenerRef()) {
            listenerRef()->setEnabled(pluginId, enabled);
        }
    }

    static void removeWidget(unsigned int pluginId, const QString& name) {
        if (listenerRef()) {
            listenerRef()->removeWidget(pluginId, name);
        }
    }

    static QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) {
        return listenerRef() ? listenerRef()->addButton(pluginId, name, label, row, col, rspan, cspan) : nullptr;
    }

    static void setButtonText(unsigned int pluginId, const QString& name, const QString& value) {
        if (listenerRef()) {
            listenerRef()->setButtonText(pluginId, name, value);
        }
    }

    static void addSlider(
        unsigned int pluginId,
        const QString& name,
        int min,
        int max,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) {
        if (listenerRef()) {
            listenerRef()->addSlider(pluginId, name, min, max, row, col, rspan, cspan);
        }
    }

    static int getSliderValue(unsigned int pluginId, const QString& name) {
        return listenerRef() ? listenerRef()->getSliderValue(pluginId, name) : 0;
    }

    static int setSliderValue(unsigned int pluginId, const QString& name, int value) {
        return listenerRef() ? listenerRef()->setSliderValue(pluginId, name, value) : 0;
    }

    static void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) {
        if (listenerRef()) {
            listenerRef()->setSliderRange(pluginId, name, min, max);
        }
    }

    static void addEditBox(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        const QString& value,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) {
        if (listenerRef()) {
            listenerRef()->addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
        }
    }

    static QString getEditBoxValue(unsigned int pluginId, const QString& name) {
        return listenerRef() ? listenerRef()->getEditBoxValue(pluginId, name) : QString();
    }

    static void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) {
        if (listenerRef()) {
            listenerRef()->setEditBoxValue(pluginId, name, value);
        }
    }

    static void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) {
        if (listenerRef()) {
            listenerRef()->addComboBox(pluginId, name, row, col, rspan, cspan);
        }
    }

    static QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) {
        return listenerRef() ? listenerRef()->getComboBoxCurrentItemText(pluginId, name) : QString();
    }

    static void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) {
        if (listenerRef()) {
            listenerRef()->setComboBoxItems(pluginId, name, items);
        }
    }

    static void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) {
        if (listenerRef()) {
            listenerRef()->addLabel(pluginId, name, text, row, col, rspan, cspan);
        }
    }

    static void setLabel(unsigned int pluginId, const QString& name, const QString& text) {
        if (listenerRef()) {
            listenerRef()->setLabel(pluginId, name, text);
        }
    }

private:
    static IPluginPanelListener*& listenerRef() {
        static IPluginPanelListener* listener = nullptr;
        return listener;
    }
};
