#pragma once

#include <QFont>
#include <QPalette>
#include <QSettings>
#include <QApplication>

#include "dll_global.h"

class AppSettingsNotifier : public QObject {
    Q_OBJECT
public:
    static AppSettingsNotifier* instance();

signals:
    void darkModeChanged(bool enabled);
    void fontChanged(const QFont& font);
};

class DPVISION_EXPORT AppSettings {
public:
    static void init();
    static void apply();  // apply font and palette

    static void setDarkMode(bool enable);
    static bool isDarkMode();

    static void setFont(const QFont& font);
    static QFont getFont();

    static QSettings* mainSettings();
    static std::unique_ptr<QSettings> pluginSettings(const QString& pluginId);

private:
    static std::unique_ptr<QSettings> settings;
};
