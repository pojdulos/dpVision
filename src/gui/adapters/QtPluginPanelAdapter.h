#pragma once

#include <QObject>

#include "../../core/interfaces/IPluginPanelListener.h"
#include "../MainWindow.h"

class QtPluginPanelAdapter : public QObject, public IPluginPanelListener {
public:
    explicit QtPluginPanelAdapter(CMainWindow* window, QObject* parent = nullptr)
        : QObject(parent), window_(window)
    {
    }

    QWidget* panel(unsigned int pluginId) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->getPanel(pluginId)
            : nullptr;
    }

    void create(unsigned int pluginId, const QString& label) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->addPluginPanel(pluginId, label);
        }
    }

    void clear(unsigned int pluginId) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->removePluginPanel(pluginId);
            window_->dockPluginPanel->addPluginPanel(pluginId, QString());
        }
    }

    void setEnabled(unsigned int pluginId, bool enabled) override
    {
        if (QWidget* widget = panel(pluginId)) {
            widget->setEnabled(enabled);
        }
    }

    void removeWidget(unsigned int pluginId, const QString& name) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->removeWidget(pluginId, name);
        }
    }

    QPushButton* addButton(
        unsigned int pluginId,
        const QString& label,
        QObject* receiver,
        const char* slot,
        int row,
        int col,
        int rspan,
        int cspan) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->addButton(pluginId, label, receiver, slot, row, col, rspan, cspan)
            : nullptr;
    }

    QPushButton* addButton(unsigned int pluginId, const QString& name, const QString& label, int row, int col, int rspan, int cspan) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->addButton(pluginId, name, label, row, col, rspan, cspan)
            : nullptr;
    }

    void setButtonText(unsigned int pluginId, const QString& name, const QString& value) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->setButtonText(pluginId, name, value);
        }
    }

    void addSlider(unsigned int pluginId, const QString& name, int min, int max, int row, int col, int rspan, int cspan) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->addSlider(pluginId, name, min, max, row, col, rspan, cspan);
        }
    }

    int getSliderValue(unsigned int pluginId, const QString& name) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->getSliderValue(pluginId, name)
            : 0;
    }

    int setSliderValue(unsigned int pluginId, const QString& name, int value) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->setSliderValue(pluginId, name, value)
            : 0;
    }

    void setSliderRange(unsigned int pluginId, const QString& name, int min, int max) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->setSliderRange(pluginId, name, min, max);
        }
    }

    void addEditBox(unsigned int pluginId, const QString& name, const QString& label, const QString& value, int row, int col, int rspan, int cspan) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
        }
    }

    QString getEditBoxValue(unsigned int pluginId, const QString& name) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->getEditBoxValue(pluginId, name)
            : QString();
    }

    void setEditBoxValue(unsigned int pluginId, const QString& name, const QString& value) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->setEditBoxValue(pluginId, name, value);
        }
    }

    void addComboBox(unsigned int pluginId, const QString& name, int row, int col, int rspan, int cspan) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->addComboBox(pluginId, name, row, col, rspan, cspan);
        }
    }

    QString getComboBoxCurrentItemText(unsigned int pluginId, const QString& name) override
    {
        return (window_ != nullptr && window_->dockPluginPanel != nullptr)
            ? window_->dockPluginPanel->getComboBoxCurrentItemText(pluginId, name)
            : QString();
    }

    void setComboBoxItems(unsigned int pluginId, const QString& name, const QStringList& items) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->setComboBoxItems(pluginId, name, items);
        }
    }

    void addLabel(unsigned int pluginId, const QString& name, const QString& text, int row, int col, int rspan, int cspan) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->addLabel(pluginId, name, text, row, col, rspan, cspan);
        }
    }

    void setLabel(unsigned int pluginId, const QString& name, const QString& text) override
    {
        if (window_ != nullptr && window_->dockPluginPanel != nullptr) {
            window_->dockPluginPanel->setLabel(pluginId, name, text);
        }
    }

private:
    CMainWindow* window_;
};
