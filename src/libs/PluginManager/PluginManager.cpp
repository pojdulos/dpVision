#include "stdafx.h"

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibraryHandle;
    #define LoadLibraryFunc LoadLibrary
    #define GetSymbolFunc GetProcAddress
    #define FreeLibraryFunc FreeLibrary
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
    #define LoadLibraryFunc(path) dlopen(path, RTLD_LAZY)
    #define GetSymbolFunc dlsym
    #define FreeLibraryFunc dlclose
#endif
#include "PluginManager.h"

// Define the prototype for a function that should exist in the DLL
// that is used to create and return the plugin type in the DLL.
typedef Plugin* (*fnCreatePlugin)(void);
// Destroys the plugin type from the DLL before the library is unloaded.
typedef void (*fnDestroyPlugin)(void);

typedef std::map<QString, PluginInterface*> PluginMap;
typedef std::map<QString, LibraryHandle > LibraryMap;

class PluginManagerPimpl
{
public:

    PluginMap m_Plugins;
    LibraryMap m_Libs;

};

PluginManager::PluginManager(void)
{
    m_Implementation = new PluginManagerPimpl();
}

PluginManager::~PluginManager(void)
{
    delete m_Implementation;
}

PluginManager& PluginManager::Instance()
{
    static PluginManager pluginManager;
    return pluginManager;
}

PluginInterface* PluginManager::LoadPlugin( const QString& pluginPath )
{
    PluginInterface* plugin = NULL;
    PluginMap::iterator iter = m_Implementation->m_Plugins.find( pluginPath );
    if ( iter == m_Implementation->m_Plugins.end() )
    {
        // Try to load the plugin library
        LibraryHandle hModule =
        #ifdef _WIN32
            LoadLibraryFunc(pluginPath.toStdWString().c_str());
        #else
            LoadLibraryFunc(pluginPath.toStdString().c_str());
            if (!hModule) {
                qWarning() << "dlopen error: " << dlerror();
            }
        #endif

        if ( hModule != NULL )
        {
            fnCreatePlugin CreatePlugin = (fnCreatePlugin)GetSymbolFunc( hModule, "CreatePlugin" );
            if ( CreatePlugin != NULL )
            {
                // Invoke the function to get the plugin from the DLL.
                plugin = CreatePlugin();

                if ( plugin != NULL )
                {
                    plugin->setPath( pluginPath );
					//------------------------------------------
                    // Add the plugin and library18 to the maps.
                    m_Implementation->m_Plugins[pluginPath] = plugin;
                    m_Implementation->m_Libs[pluginPath] = hModule;
                }
                else
                {
                    qInfo() << "ERROR: Could not load plugin from " << pluginPath << Qt::endl;
                    // Unload the library.
                    FreeLibraryFunc(hModule);
                }
            }
            else
            {
                qInfo() << "ERROR: Could not find symbol \"CreatePlugin\" in " << pluginPath << Qt::endl;
                FreeLibraryFunc(hModule);
            }
        }
        else
        {
            qInfo() << "ERROR: Could not load library: " << pluginPath << Qt::endl;
        }
    }
    else
    {
        #ifdef _WIN32
            MessageBoxW(
                NULL,
                (LPCWSTR)L"Plugin is already loaded!!!",
                (LPCWSTR)L"PluginManager", MB_DEFBUTTON2 );
        #else
            qInfo() << "Plugin is already loaded!!!";
        #endif
    }

    return plugin;
}

bool PluginManager::UnloadPlugin( PluginInterface*& plugin )
{
    if ( plugin != NULL )
    {
        if (UnloadPlugin(plugin->path()))
        {
            plugin = NULL;
            return true;
        }
    }
    return false;
}

bool PluginManager::UnloadPlugin( const QString& pluginPath )
{
        LibraryMap::iterator iter = m_Implementation->m_Libs.find( pluginPath );
        if( iter != m_Implementation->m_Libs.end() )
        {
            // Remove the plugin from our plugin map.
            PluginMap::iterator pl = m_Implementation->m_Plugins.find(pluginPath);
            if (pl != m_Implementation->m_Plugins.end())
                m_Implementation->m_Plugins.erase(pl);

            LibraryHandle hModule = (*iter).second;
            fnDestroyPlugin DestroyPlugin = (fnDestroyPlugin)GetSymbolFunc( hModule, "DestroyPlugin" );
            if ( DestroyPlugin != NULL )
            {
                DestroyPlugin();
            }
            else
            {
                qInfo() << "ERROR: Unable to find symbol \"DestroyPlugin\" in library \"" << pluginPath << Qt::endl;
            }
            // Unload the library and remove the library from the map.
            FreeLibraryFunc(hModule);
            m_Implementation->m_Libs.erase(iter);

            return true;
        }
        else
        {
			//MessageBoxW(
			//NULL,
			//(LPCWSTR)L"Plugin is already unloaded or never been loaded!!!",
			//(LPCWSTR)L"PluginManager", MB_DEFBUTTON2 );
            qInfo() << "WARNING: Trying to unload a plugin that is already unloaded or has never been loaded." << Qt::endl;

            return false;
        }
}
