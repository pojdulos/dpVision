// PluginDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ConcretePlugin.h"

ConcretePlugin* g_ConcretePlugin = NULL;

extern "C" Plugin* CreatePlugin(void)
{
    assert(g_ConcretePlugin == NULL);
    g_ConcretePlugin = new ConcretePlugin();
    return g_ConcretePlugin;
}

extern "C" void DestroyPlugin(void)
{
    assert(g_ConcretePlugin);
    delete g_ConcretePlugin;
    g_ConcretePlugin = NULL;
}
