#pragma once

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>

class QTabWidget;
class QCheckBox;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private:
    QTabWidget* tabWidget;
    QFontComboBox* fontComboBox;
    QSpinBox* fontSizeSpinBox;

    QCheckBox* darkModeCheckBox;

    void applyDarkPalette();

    void setupTabs();
};
