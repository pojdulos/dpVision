#pragma once

#include "dll_global.h"

class PluginInterface;

class DPVISION_EXPORT PluginRuntimeManager {
public:
    static PluginInterface* activePlugin() { return activePluginRef(); }
    static void setActivePlugin(PluginInterface* plugin) { activePluginRef() = plugin; }

    static bool globalPickingEnabled() { return globalPickingRef(); }
    static void setGlobalPickingEnabled(bool enabled) { globalPickingRef() = enabled; }

    static bool pickSnapEnabled() { return pickSnapRef(); }
    static void setPickSnapEnabled(bool enabled) { pickSnapRef() = enabled; }

private:
    static PluginInterface*& activePluginRef() {
        static PluginInterface* plugin = nullptr;
        return plugin;
    }

    static bool& globalPickingRef() {
        static bool enabled = false;
        return enabled;
    }

    static bool& pickSnapRef() {
        static bool enabled = false;
        return enabled;
    }
};
