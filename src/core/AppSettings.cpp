#include "AppSettings.h"
#include <QStyle>

//bool AppSettings::darkMode = false;
//QFont AppSettings::appFont = QApplication::font();
std::unique_ptr<QSettings> AppSettings::settings = nullptr;


AppSettingsNotifier* AppSettingsNotifier::instance()
{
    static AppSettingsNotifier notifier;
    return &notifier;
}


void AppSettings::init()
{
    if (!settings)
        settings = std::make_unique<QSettings>();
}


void AppSettings::apply()
{
    QApplication::setFont(getFont());

    if (isDarkMode()) {
        QPalette dark;
        dark.setColor(QPalette::Window, QColor(53, 53, 53));
        dark.setColor(QPalette::WindowText, Qt::white);
        dark.setColor(QPalette::Base, QColor(42, 42, 42));
        dark.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        dark.setColor(QPalette::ToolTipBase, Qt::white);
        dark.setColor(QPalette::ToolTipText, Qt::white);
        dark.setColor(QPalette::Text, Qt::white);
        dark.setColor(QPalette::Button, QColor(53, 53, 53));
        dark.setColor(QPalette::ButtonText, Qt::white);
        dark.setColor(QPalette::BrightText, Qt::red);
        dark.setColor(QPalette::Highlight, QColor(128, 128, 64).lighter());
        dark.setColor(QPalette::HighlightedText, Qt::black);

        QApplication::setPalette(dark);
    }
    else {
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
}

void AppSettings::setDarkMode(bool enable)
{
    if (enable) {
        QPalette dark;
        dark.setColor(QPalette::Window, QColor(53, 53, 53));
        dark.setColor(QPalette::WindowText, Qt::white);
        dark.setColor(QPalette::Base, QColor(42, 42, 42));
        dark.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        dark.setColor(QPalette::ToolTipBase, Qt::white);
        dark.setColor(QPalette::ToolTipText, Qt::white);
        dark.setColor(QPalette::Text, Qt::white);
        dark.setColor(QPalette::Button, QColor(53, 53, 53));
        dark.setColor(QPalette::ButtonText, Qt::white);
        dark.setColor(QPalette::BrightText, Qt::red);
        dark.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
        dark.setColor(QPalette::HighlightedText, Qt::black);

        QApplication::setPalette(dark);
    }
    else {
        QApplication::setPalette(QApplication::style()->standardPalette());
    }

    settings->setValue("ui/darkMode", enable);
    settings->sync();
    emit AppSettingsNotifier::instance()->darkModeChanged(enable);
}

bool AppSettings::isDarkMode()
{
    return settings->value("ui/darkMode", false).toBool();
}

void AppSettings::setFont(const QFont& font)
{
    QApplication::setFont(font);

    settings->setValue("ui/font", font);
    settings->sync();
    emit AppSettingsNotifier::instance()->fontChanged(font);
}

QFont AppSettings::getFont()
{
    return settings->value("ui/font", QApplication::font()).value<QFont>();
}

QSettings* AppSettings::mainSettings()
{
    return settings.get();
}

std::unique_ptr<QSettings> AppSettings::pluginSettings(const QString& pluginId)
{
    auto s = std::make_unique<QSettings>();
    s->beginGroup("plugins/" + pluginId);
    return s;
}
