#pragma once
#include "dll_global.h"

class Plugin;

// This class is exported from the PluginDLL.dll
class DPVISION_DLL_API CPluginDLL {
public:
	CPluginDLL(void);
	// TODO: add your methods here.
};

extern "C" DPVISION_DLL_API Plugin* CreatePlugin(void);
extern "C" DPVISION_DLL_API void DestroyPlugin(void);
