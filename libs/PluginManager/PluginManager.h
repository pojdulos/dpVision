#pragma once
#include "dll_global.h"

#include "CommonDLL.h"
#include "Plugin.h"

class PluginManagerPimpl;

class DPVISION_DLL_API PluginManager
{
public:
    static PluginManager& Instance();

    Plugin* LoadPlugin( const std::wstring& pluginName );
    void UnloadPlugin( Plugin*& plugin );
    void UnloadPlugin( const std::wstring& pluginName );

private:
    PluginManager(void);
    ~PluginManager(void);

    PluginManagerPimpl* m_Implementation;
};
