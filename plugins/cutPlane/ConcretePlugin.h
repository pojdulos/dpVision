#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#ifdef PLUGIN_NAME
    #undef PLUGIN_NAME
#endif 
#define PLUGIN_NAME L"(dp) cutPlane"

class CAnnotationPlane;
class CPointCloud;
class COrderedPointCloud;
class CMesh;
class CModel3D;

class PLUGINDLL_API ConcretePlugin : public Plugin
{
	bool m_picking;
	CAnnotationPlane* m_cutPlane;

public:
	ConcretePlugin(void);
	~ConcretePlugin(void);

	bool onMousePick(Plugin::PickEvent pickEvent) override;

	void onLoad() override;
	void onUnload() override;
	CAnnotationPlane* findFirstCheckedPlane();
	void onButton(std::wstring name) override;
};
