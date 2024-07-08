#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#define PLUGIN_NAME L"(dp-parser) Pliki VRML"

class PLUGINDLL_API ConcretePlugin : public Plugin
{
public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);
	
	void MarchingTetrahedron(void);

	int isHidden(){ return 1; };

	void onLoad();
	void onUnload();
};
