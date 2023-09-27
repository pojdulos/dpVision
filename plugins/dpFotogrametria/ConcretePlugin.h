#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#define PLUGIN_NAME L"(dp) Fotogrametria"

class PLUGINDLL_API ConcretePlugin : public Plugin
{
	bool m_picking;

public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);

	/* ============= EVENT HANDLING BEGIN =========================*/

	// called on doubleclick on plugin name
	virtual void onRun() override;

	// called when plugin is loaded
	virtual void onLoad() override;

	// called before plugin unload
	virtual void onUnload() override;

	// called when you select plugin
	//virtual void onActivate() override {};

	// called when you select another plugin
	//virtual void onDeactivate() override {};

	// called if button on PluginPanel is pressed
	virtual void onButton(std::wstring name) override;

	// called if plugin is active and ctrl+MouseClick is done on any model
	//virtual bool onMousePick(Plugin::PickEvent pickEvent) override;

	/* ============= EVENT HANDLING END =========================*/
};
