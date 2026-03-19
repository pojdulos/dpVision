#pragma once

class QString;
class QWidget;
class QPushButton;

class IPluginPanelAPI {
public:
    virtual ~IPluginPanelAPI() = default;
    virtual QWidget* panel(unsigned int pluginId) = 0;
    virtual void create(unsigned int pluginId, const QString& label) = 0;
    virtual void clear(unsigned int pluginId) = 0;
    virtual void setEnabled(unsigned int pluginId, bool enabled) = 0;
    virtual QPushButton* addButton(
        unsigned int pluginId,
        const QString& name,
        const QString& label,
        int row,
        int col,
        int rspan = 0,
        int cspan = 0) = 0;
};
