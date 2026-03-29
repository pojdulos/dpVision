#pragma once

#include <QString>
#include <QStringList>

class QObject;
class QWidget;
class QPushButton;

class IPluginPanelListener {
public:
    virtual ~IPluginPanelListener() = default;

    virtual QWidget* panel(unsigned int pluginId) = 0;
    virtual void create(unsigned int pluginId, const QString& label) = 0;
    virtual void clear(unsigned int pluginId) = 0;
    virtual void setEnabled(unsigned int pluginId, bool enabled) = 0;
    virtual void removeWidget(unsigned int pluginId, const QString& name) = 0;
    virtual QPushButton* addButton(
        unsigned int pluginId,
        const QString& label,
        QObject* receiver,
        const char* slot,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual void setButtonText(unsigned int pluginId, const QString& name, const QString& value) = 0;
    virtual void addSlider(
        unsigned int pluginId,
        const QString& name,
        int min,
        int max,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual int getSliderValue(unsigned int pluginId, const QString& name) = 0;
    virtual int setSliderValue(unsigned int pluginId, const QString& name, int value) = 0;
    virtual void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) = 0;
    virtual void addEditBox(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        const QString& value,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual QString getEditBoxValue(unsigned int pluginId, const QString& name) = 0;
    virtual void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) = 0;
    virtual void addComboBox(
        unsigned int pluginId,
        const QString& name,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) = 0;
    virtual void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) = 0;
    virtual void addLabel(
        unsigned int pluginId,
        const QString& name,
        const QString& text,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
    virtual void setLabel(unsigned int pluginId, const QString& name, const QString& text) = 0;
};
