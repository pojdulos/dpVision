#pragma once

#include "dll_global.h"
#include "interfaces/ISettingsStorage.h"
#include <functional>
#include <memory>

class QString;

class DPVISION_EXPORT SettingsStorageRegistry {
public:
    using PluginSettingsFactory = std::function<std::unique_ptr<ISettingsStorage>(const QString&)>;

    static void setDefaultStorage(ISettingsStorage* storage);
    static ISettingsStorage* defaultStorage();

    static void setPluginSettingsFactory(PluginSettingsFactory factory);
    static const PluginSettingsFactory& pluginSettingsFactory();
};
