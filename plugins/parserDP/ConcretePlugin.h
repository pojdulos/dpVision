#pragma once
#include "dll_global.h"

#include "Plugin.h"
#include "PluginDLL.h"

#define PLUGIN_NAME L"(dp-parser) różne formaty"

class DPVISION_DLL_API ConcretePlugin : public Plugin
{
public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);  
	
	void MarchingTetrahedron(void);

	int isHidden() override;

	void onLoad() override;
	void onUnload() override;
};
