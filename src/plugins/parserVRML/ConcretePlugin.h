#pragma once
#include "dll_global.h"

#include "Plugin.h"
#include "PluginDLL.h"

#define PLUGIN_NAME L"(dp-parser) Pliki VRML"

class DPVISION_DLL_API ConcretePlugin : public Plugin
{
public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);
	
	void MarchingTetrahedron(void);

	int isHidden(){ return 1; };

	void onLoad();
	void onUnload();
};
