#include "StdAfx.h"
#include "PluginManager.h"

// Define the prototype for a function that should exist in the DLL
// that is used to create and return the plugin type in the DLL.
typedef Plugin* (*fnCreatePlugin)(void);
// Destroys the plugin type from the DLL before the library is unloaded.
typedef void (*fnDestroyPlugin)(void);

typedef std::map<std::wstring, Plugin*> PluginMap;
typedef std::map<std::wstring, HMODULE > LibraryMap;

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

Plugin* PluginManager::LoadPlugin( const std::wstring& pluginPath )
{
    Plugin* plugin = NULL;
    PluginMap::iterator iter = m_Implementation->m_Plugins.find( pluginPath );
    if ( iter == m_Implementation->m_Plugins.end() )
    {
        // Try to load the plugin library
        HMODULE hModule = LoadLibraryW( pluginPath.c_str() );
        if ( hModule != NULL )
        {
            fnCreatePlugin CreatePlugin = (fnCreatePlugin)GetProcAddress( hModule, "CreatePlugin" );
            if ( CreatePlugin != NULL )
            {
                // Invoke the function to get the plugin from the DLL.
                plugin = CreatePlugin();

                if ( plugin != NULL )
                {
					//------------------------------------------
					//nie wiem po co tu wymusza sie nazwe pliku:
                    //plugin->SetName( pluginPath );
					//bez tego tez dziala, zamieniam na dodane przeze mnie:
                    plugin->setPath( pluginPath );
					//------------------------------------------
                    // Add the plugin and library18 to the maps.
                    m_Implementation->m_Plugins.insert( PluginMap::value_type( pluginPath, plugin ) );
                    m_Implementation->m_Libs.insert( LibraryMap::value_type(pluginPath, hModule ) );
                }
                else
                {
                    std::wcerr << "ERROR: Could not load plugin from " << pluginPath << std::endl;
                    // Unload the library.
                    FreeLibrary(hModule);
                }
            }
            else
            {
                std::wcerr << "ERROR: Could not find symbol \"CreatePlugin\" in " << pluginPath << std::endl;
                FreeLibrary(hModule);
            }
        }
        else
        {
            std::wcerr << "ERROR: Could not load library: " << pluginPath << std::endl;
        }
    }
    else
    {
		MessageBoxW(
        NULL,
        (LPCWSTR)L"Plugin already loaded!!!",
        (LPCWSTR)L"PluginManager", MB_DEFBUTTON2 );
        std::wcout << "INFO: Library \"" << pluginPath << "\" already loaded." << std::endl;
        //plugin = iter->second;
    }

    return plugin;
}

void PluginManager::UnloadPlugin( Plugin*& plugin )
{
    if ( plugin != NULL )
    {
        LibraryMap::iterator iter = m_Implementation->m_Libs.find( plugin->name() );
        if( iter != m_Implementation->m_Libs.end() )
        {
            // Remove the plugin from our plugin map.
            m_Implementation->m_Plugins.erase(plugin->name());

            HMODULE hModule = iter->second;
            fnDestroyPlugin DestroyPlugin = (fnDestroyPlugin)GetProcAddress( hModule, "DestroyPlugin" );
            if ( DestroyPlugin != NULL )
            {
                DestroyPlugin();
            }
            else
            {
                std::wcerr << "ERROR: Unable to find symbol \"DestroyPlugin\" in library \"" << plugin->name() << std::endl;
            }
            // Unload the library and remove the library from the map.
            FreeLibrary(hModule);
            m_Implementation->m_Libs.erase(iter);
        }
        else
        {
			MessageBoxW(
			NULL,
			(LPCWSTR)L"Plugin is already unloaded or never been loaded!!!",
			(LPCWSTR)L"PluginManager", MB_DEFBUTTON2 );
            std::cout << "WARNING: Trying to unload a plugin that is already unloaded or has never been loaded." << std::endl;
        }
        // NULL the plugin
        plugin = NULL;
    }
}

void PluginManager::UnloadPlugin( const std::wstring& pluginName )
{
        LibraryMap::iterator iter = m_Implementation->m_Libs.find( pluginName );
        if( iter != m_Implementation->m_Libs.end() )
        {
            // Remove the plugin from our plugin map.
            m_Implementation->m_Plugins.erase(pluginName);

            HMODULE hModule = iter->second;
            fnDestroyPlugin DestroyPlugin = (fnDestroyPlugin)GetProcAddress( hModule, "DestroyPlugin" );
            if ( DestroyPlugin != NULL )
            {
                DestroyPlugin();
            }
            else
            {
                std::wcerr << "ERROR: Unable to find symbol \"DestroyPlugin\" in library \"" << pluginName << std::endl;
            }
            // Unload the library and remove the library from the map.
            FreeLibrary(hModule);
            m_Implementation->m_Libs.erase(iter);
        }
        else
        {
			MessageBoxW(
			NULL,
			(LPCWSTR)L"Plugin is already unloaded or never been loaded!!!",
			(LPCWSTR)L"PluginManager", MB_DEFBUTTON2 );
            std::cout << "WARNING: Trying to unload a plugin that is already unloaded or has never been loaded." << std::endl;
        }
}
