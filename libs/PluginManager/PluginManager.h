#pragma once
#include "dll_global.h"

#include "CommonDLL.h"
#include "Plugin.h"

class PluginManagerPimpl;

class DPVISION_DLL_API PluginManager
{
public:
    static PluginManager& Instance();

    PluginInterface* LoadPlugin( const QString& pluginName );
    bool UnloadPlugin( PluginInterface*& plugin );
    bool UnloadPlugin( const QString& pluginPath );

private:
    PluginManager(void);
    ~PluginManager(void);

    PluginManagerPimpl* m_Implementation;
};
