#pragma once

#include <QString>
#include <QStringList>
#include <string>
#include <vector>

class QWidget;
class QPushButton;

// Privileged GUI contract.
//
// This interface intentionally exposes Qt widgets and should only be reachable
// through the explicit GUI-capable plugin path. It is not part of the default
// plugin host surface.
class IPluginPanelAPI {
public:
    virtual ~IPluginPanelAPI() = default;
    virtual QWidget* panel(unsigned int pluginId) = 0;
    virtual void create(unsigned int pluginId, const QString& label) = 0;
    virtual void clear(unsigned int pluginId) = 0;
    virtual void setEnabled(unsigned int pluginId, bool enabled) = 0;
    virtual void removeWidget(unsigned int pluginId, const QString& name) = 0;
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

    void create(unsigned int pluginId, const char* label = "") {
        create(pluginId, QString::fromUtf8(label));
    }

    void create(unsigned int pluginId, const std::string& label) {
        create(pluginId, QString::fromUtf8(label.c_str()));
    }

    void create(unsigned int pluginId, const std::wstring& label) {
        create(pluginId, QString::fromWCharArray(label.c_str()));
    }

    void removeWidget(unsigned int pluginId, const char* name) {
        removeWidget(pluginId, QString::fromUtf8(name));
    }

    void removeWidget(unsigned int pluginId, const std::string& name) {
        removeWidget(pluginId, QString::fromUtf8(name.c_str()));
    }

    void removeWidget(unsigned int pluginId, const std::wstring& name) {
        removeWidget(pluginId, QString::fromWCharArray(name.c_str()));
    }

    QPushButton* addButton(unsigned int pluginId, const std::string& name, const std::string& label, int row, int col, int rspan = 0, int cspan = 0) {
        return addButton(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(label.c_str()), row, col, rspan, cspan);
    }

    QPushButton* addButton(unsigned int pluginId, const std::wstring& name, const std::wstring& label, int row, int col, int rspan = 0, int cspan = 0) {
        return addButton(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(label.c_str()), row, col, rspan, cspan);
    }

    void setButtonText(unsigned int pluginId, const char* name, const char* value) {
        setButtonText(pluginId, QString::fromUtf8(name), QString::fromUtf8(value));
    }

    void setButtonText(unsigned int pluginId, const std::string& name, const std::string& value) {
        setButtonText(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str()));
    }

    void setButtonText(unsigned int pluginId, const std::wstring& name, const std::wstring& value) {
        setButtonText(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str()));
    }

    void addSlider(unsigned int pluginId, const char* name, int min, int max, int row, int col, int rspan = 0, int cspan = 0) {
        addSlider(pluginId, QString::fromUtf8(name), min, max, row, col, rspan, cspan);
    }

    void addSlider(unsigned int pluginId, const std::string& name, int min, int max, int row, int col, int rspan = 0, int cspan = 0) {
        addSlider(pluginId, QString::fromUtf8(name.c_str()), min, max, row, col, rspan, cspan);
    }

    void addSlider(unsigned int pluginId, const std::wstring& name, int min, int max, int row, int col, int rspan = 0, int cspan = 0) {
        addSlider(pluginId, QString::fromWCharArray(name.c_str()), min, max, row, col, rspan, cspan);
    }

    int getSliderValue(unsigned int pluginId, const char* name) {
        return getSliderValue(pluginId, QString::fromUtf8(name));
    }

    int getSliderValue(unsigned int pluginId, const std::string& name) {
        return getSliderValue(pluginId, QString::fromUtf8(name.c_str()));
    }

    int getSliderValue(unsigned int pluginId, const std::wstring& name) {
        return getSliderValue(pluginId, QString::fromWCharArray(name.c_str()));
    }

    int setSliderValue(unsigned int pluginId, const char* name, int value) {
        return setSliderValue(pluginId, QString::fromUtf8(name), value);
    }

    int setSliderValue(unsigned int pluginId, const std::string& name, int value) {
        return setSliderValue(pluginId, QString::fromUtf8(name.c_str()), value);
    }

    int setSliderValue(unsigned int pluginId, const std::wstring& name, int value) {
        return setSliderValue(pluginId, QString::fromWCharArray(name.c_str()), value);
    }

    void setSliderRange(unsigned int pluginId, const char* name, int min, int max) {
        setSliderRange(pluginId, QString::fromUtf8(name), min, max);
    }

    void setSliderRange(unsigned int pluginId, const std::string& name, int min, int max) {
        setSliderRange(pluginId, QString::fromUtf8(name.c_str()), min, max);
    }

    void setSliderRange(unsigned int pluginId, const std::wstring& name, int min, int max) {
        setSliderRange(pluginId, QString::fromWCharArray(name.c_str()), min, max);
    }

    void addEditBox(unsigned int pluginId, const char* name, const char* label, const char* value, int row, int col, int rspan = 0, int cspan = 0) {
        addEditBox(pluginId, QString::fromUtf8(name), QString::fromUtf8(label), QString::fromUtf8(value), row, col, rspan, cspan);
    }

    void addEditBox(unsigned int pluginId, const std::string& name, const std::string& label, const std::string& value, int row, int col, int rspan = 0, int cspan = 0) {
        addEditBox(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(label.c_str()), QString::fromUtf8(value.c_str()), row, col, rspan, cspan);
    }

    void addEditBox(unsigned int pluginId, const std::wstring& name, const std::wstring& label, const std::wstring& value, int row, int col, int rspan = 0, int cspan = 0) {
        addEditBox(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(label.c_str()), QString::fromWCharArray(value.c_str()), row, col, rspan, cspan);
    }

    std::string getEditBoxValue(unsigned int pluginId, const std::string& name) {
        return getEditBoxValue(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString();
    }

    std::wstring getEditBoxValue(unsigned int pluginId, const std::wstring& name) {
        return getEditBoxValue(pluginId, QString::fromWCharArray(name.c_str())).toStdWString();
    }

    void setEditBoxValue(unsigned int pluginId, const char* name, const char* value) {
        setEditBoxValue(pluginId, QString::fromUtf8(name), QString::fromUtf8(value));
    }

    void setEditBoxValue(unsigned int pluginId, const std::string& name, const std::string& value) {
        setEditBoxValue(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str()));
    }

    void setEditBoxValue(unsigned int pluginId, const std::wstring& name, const std::wstring& value) {
        setEditBoxValue(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str()));
    }

    void addComboBox(unsigned int pluginId, const char* name, int row, int col, int rspan = 0, int cspan = 0) {
        addComboBox(pluginId, QString::fromUtf8(name), row, col, rspan, cspan);
    }

    void addComboBox(unsigned int pluginId, const std::string& name, int row, int col, int rspan = 0, int cspan = 0) {
        addComboBox(pluginId, QString::fromUtf8(name.c_str()), row, col, rspan, cspan);
    }

    void addComboBox(unsigned int pluginId, const std::wstring& name, int row, int col, int rspan = 0, int cspan = 0) {
        addComboBox(pluginId, QString::fromWCharArray(name.c_str()), row, col, rspan, cspan);
    }

    std::string getComboBoxCurrentItemText(unsigned int pluginId, const std::string& name) {
        return getComboBoxCurrentItemText(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString();
    }

    std::wstring getComboBoxCurrentItemText(unsigned int pluginId, const std::wstring& name) {
        return getComboBoxCurrentItemText(pluginId, QString::fromWCharArray(name.c_str())).toStdWString();
    }

    void setComboBoxItems(unsigned int pluginId, const char* name, std::initializer_list<const char*> items) {
        QStringList qItems;
        for (const char* item : items) {
            qItems.append(QString::fromUtf8(item));
        }
        setComboBoxItems(pluginId, QString::fromUtf8(name), qItems);
    }

    void setComboBoxItems(unsigned int pluginId, const std::string& name, const std::vector<std::string>& items) {
        QStringList qItems;
        for (const std::string& item : items) {
            qItems.append(QString::fromUtf8(item.c_str()));
        }
        setComboBoxItems(pluginId, QString::fromUtf8(name.c_str()), qItems);
    }

    void setComboBoxItems(unsigned int pluginId, const std::wstring& name, const std::vector<std::wstring>& items) {
        QStringList qItems;
        for (const std::wstring& item : items) {
            qItems.append(QString::fromWCharArray(item.c_str()));
        }
        setComboBoxItems(pluginId, QString::fromWCharArray(name.c_str()), qItems);
    }

    void addLabel(unsigned int pluginId, const char* name, const char* text, int row, int col, int rspan = 0, int cspan = 0) {
        addLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text), row, col, rspan, cspan);
    }

    void addLabel(unsigned int pluginId, const std::string& name, const std::string& text, int row, int col, int rspan = 0, int cspan = 0) {
        addLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str()), row, col, rspan, cspan);
    }

    void addLabel(unsigned int pluginId, const std::wstring& name, const std::wstring& text, int row, int col, int rspan = 0, int cspan = 0) {
        addLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str()), row, col, rspan, cspan);
    }

    void setLabel(unsigned int pluginId, const char* name, const char* text) {
        setLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text));
    }

    void setLabel(unsigned int pluginId, const std::string& name, const std::string& text) {
        setLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str()));
    }

    void setLabel(unsigned int pluginId, const std::wstring& name, const std::wstring& text) {
        setLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str()));
    }
};
