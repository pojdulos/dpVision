#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#ifdef PLUGIN_NAME
	#undef PLUGIN_NAME
#endif 
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
