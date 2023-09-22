#pragma once
#include "dll_global.h"

#include "Plugin.h"
#include "PluginDLL.h"

#ifdef PLUGIN_NAME
    #undef PLUGIN_NAME
#endif 
#define PLUGIN_NAME L"(dp) testy2"

class CAnnotationPlane;
class CPointCloud;
class CMesh;
class CModel3D;
class TestWorker;
class Smoother;
class Kule;

#include "Vertex.h"

class DPVISION_DLL_API ConcretePlugin : public Plugin
{
	bool m_picking;
	CAnnotationPlane *m_cutPlane;

	void showMainPluginPanel();
	void showMarchingTetrahedronPanel();

public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);

	void onLoad() override;
	void onUnload() override;
	void onButton( std::wstring name ) override;
};
