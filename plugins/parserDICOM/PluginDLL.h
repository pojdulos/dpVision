#pragma once;
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGINDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGINDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DPVISION_PLUGIN_DLL
#define PLUGINDLL_API __declspec(dllexport)
#else
#define PLUGINDLL_API __declspec(dllimport)
#endif

// This class is exported from the PluginDLL.dll
class PLUGINDLL_API CPluginDLL {
public:
	CPluginDLL(void);
	// TODO: add your methods here.
};

extern "C" PLUGINDLL_API Plugin* CreatePlugin(void);
extern "C" PLUGINDLL_API void DestroyPlugin(void);
