#include "SettingsStorageRegistry.h"

namespace
{
    ISettingsStorage*& defaultStorageRef()
    {
        static ISettingsStorage* storage = nullptr;
        return storage;
    }

    SettingsStorageRegistry::PluginSettingsFactory& pluginSettingsFactoryRef()
    {
        static SettingsStorageRegistry::PluginSettingsFactory factory;
        return factory;
    }
}

void SettingsStorageRegistry::setDefaultStorage(ISettingsStorage* storage)
{
    defaultStorageRef() = storage;
}

ISettingsStorage* SettingsStorageRegistry::defaultStorage()
{
    return defaultStorageRef();
}

void SettingsStorageRegistry::setPluginSettingsFactory(PluginSettingsFactory factory)
{
    pluginSettingsFactoryRef() = std::move(factory);
}

const SettingsStorageRegistry::PluginSettingsFactory& SettingsStorageRegistry::pluginSettingsFactory()
{
    return pluginSettingsFactoryRef();
}
