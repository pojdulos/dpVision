#include "AppSettings.h"
#include "../core/SettingsStorageRegistry.h"
#include <QStyle>

namespace {
class QSettingsStorage final : public ISettingsStorage {
public:
    explicit QSettingsStorage(QSettings* settings)
        : settings_(settings) {
    }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override
    {
        if (settings_ == nullptr) {
            return defaultValue;
        }
        return settings_->value(key, defaultValue);
    }

    void setValue(const QString& key, const QVariant& value) override
    {
        if (settings_ != nullptr) {
            settings_->setValue(key, value);
        }
    }

    bool contains(const QString& key) const override
    {
        return settings_ != nullptr && settings_->contains(key);
    }

    void remove(const QString& key) override
    {
        if (settings_ != nullptr) {
            settings_->remove(key);
        }
    }

    QStringList childGroups() const override
    {
        if (settings_ == nullptr) {
            return {};
        }
        return settings_->childGroups();
    }

    void beginGroup(const QString& prefix) override
    {
        if (settings_ != nullptr) {
            settings_->beginGroup(prefix);
        }
    }

    void endGroup() override
    {
        if (settings_ != nullptr) {
            settings_->endGroup();
        }
    }

    void sync() override
    {
        if (settings_ != nullptr) {
            settings_->sync();
        }
    }

private:
    QSettings* settings_ = nullptr;
};

class OwnedQSettingsStorage final : public ISettingsStorage {
public:
    explicit OwnedQSettingsStorage(std::unique_ptr<QSettings> settings)
        : settings_(std::move(settings)) {
    }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override
    {
        if (settings_ == nullptr) {
            return defaultValue;
        }
        return settings_->value(key, defaultValue);
    }

    void setValue(const QString& key, const QVariant& value) override
    {
        if (settings_ != nullptr) {
            settings_->setValue(key, value);
        }
    }

    bool contains(const QString& key) const override
    {
        return settings_ != nullptr && settings_->contains(key);
    }

    void remove(const QString& key) override
    {
        if (settings_ != nullptr) {
            settings_->remove(key);
        }
    }

    QStringList childGroups() const override
    {
        if (settings_ == nullptr) {
            return {};
        }
        return settings_->childGroups();
    }

    void beginGroup(const QString& prefix) override
    {
        if (settings_ != nullptr) {
            settings_->beginGroup(prefix);
        }
    }

    void endGroup() override
    {
        if (settings_ != nullptr) {
            settings_->endGroup();
        }
    }

    void sync() override
    {
        if (settings_ != nullptr) {
            settings_->sync();
        }
    }

private:
    std::unique_ptr<QSettings> settings_;
};
}


//bool AppSettings::darkMode = false;
//QFont AppSettings::appFont = QApplication::font();
std::unique_ptr<QSettings>& AppSettings::settingsRef()
{
    static std::unique_ptr<QSettings> settings = nullptr;
    return settings;
}


AppSettingsNotifier* AppSettingsNotifier::instance()
{
    static AppSettingsNotifier notifier;
    return &notifier;
}


void AppSettings::init()
{
    auto& settings = settingsRef();
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

void AppSettings::configureApiDefaults()
{
    SettingsStorageRegistry::setDefaultStorage(mainStorage());
    SettingsStorageRegistry::setPluginSettingsFactory(&AppSettings::pluginStorage);
}

void AppSettings::setDarkMode(bool enable)
{
    auto& settings = settingsRef();
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
    auto& settings = settingsRef();
    return settings->value("ui/darkMode", false).toBool();
}

void AppSettings::setFont(const QFont& font)
{
    auto& settings = settingsRef();
    QApplication::setFont(font);

    settings->setValue("ui/font", font);
    settings->sync();
    emit AppSettingsNotifier::instance()->fontChanged(font);
}

QFont AppSettings::getFont()
{
    auto& settings = settingsRef();
    return settings->value("ui/font", QApplication::font()).value<QFont>();
}

QSettings* AppSettings::mainSettings()
{
    auto& settings = settingsRef();
    return settings.get();
}

ISettingsStorage* AppSettings::mainStorage()
{
    static QSettingsStorage storage(mainSettings());
    return &storage;
}

std::unique_ptr<ISettingsStorage> AppSettings::pluginStorage(const QString& pluginId)
{
    return std::make_unique<OwnedQSettingsStorage>(pluginSettings(pluginId));
}

std::unique_ptr<QSettings> AppSettings::pluginSettings(const QString& pluginId)
{
    auto s = std::make_unique<QSettings>();
    s->beginGroup("plugins/" + pluginId);
    return s;
}
