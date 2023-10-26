#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#define PLUGIN_NAME L"(dp) Plugin Testowy nr 2"

class DPVISION_DLL_API ConcretePlugin : public Plugin
{
public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);
	
	virtual void onRun() override;

	virtual void onLoad() override;
	virtual void onUnload() override;

	virtual void onActivate() override; 
	virtual void onDeactivate() override;

	virtual void onButton( std::wstring name ) override;

	void akcja( std::wstring ext );
};
