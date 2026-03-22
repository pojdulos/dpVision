#pragma once

#include "../interfaces/ISettingsAPI.h"
#include "../../core/AppSettings.h"

class SettingsAPIAdapter : public ISettingsAPI {
    QSettings* settings_ = nullptr;
public:
    SettingsAPIAdapter()
        : settings_(AppSettings::mainSettings()) {
    }

    explicit SettingsAPIAdapter(QSettings* settings)
        : settings_(settings) {
    }

    void reset(QSettings* settings) {
        settings_ = settings;
    }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override {
        if (settings_ != nullptr) {
            return settings_->value(key, defaultValue);
        }
        return defaultValue;
    }

    void setValue(const QString& key, const QVariant& value) override {
        if (settings_ != nullptr) {
            settings_->setValue(key, value);
        }
    }

    bool contains(const QString& key) const override {
        if (settings_ != nullptr) {
            return settings_->contains(key);
        }
        return false;
    }

    void remove(const QString& key) override {
        if (settings_ != nullptr) {
            settings_->remove(key);
        }
    }

    QStringList childGroups() const override {
        if (settings_ != nullptr) {
            return settings_->childGroups();
        }
        return {};
    }

    void beginGroup(const QString& prefix) override {
        if (settings_ != nullptr) {
            settings_->beginGroup(prefix);
        }
    }

    void endGroup() override {
        if (settings_ != nullptr) {
            settings_->endGroup();
        }
    }

    void sync() override {
        if (settings_ != nullptr) {
            settings_->sync();
        }
    }
};
