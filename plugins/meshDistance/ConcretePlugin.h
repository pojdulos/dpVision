#pragma once
#include "Plugin.h"
#include "PluginDLL.h"

#include <map>

#define PLUGIN_NAME L"(dp) MeshDistance"

class CAnnotationPoint;
class CMeshDistance;

class PLUGINDLL_API ConcretePlugin : public Plugin
{
	bool m_picking;
	CMeshDistance *symulator;
	std::map<size_t,std::vector<size_t>> klikniete;

public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);

	void MarchingTetrahedron(void);

	bool onMousePick(Plugin::PickEvent pickEvent);

	void onLoad();
	void onUnload();
	void onButton( std::wstring name );
};


#include "UI.h"

template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >  class __declspec(dllexport) CTest1 {
	_T x, y, z;
	public:
		inline void metodaA() { UI::STATUSBAR::printf("jestem metodaA klasy CTest1"); };
		inline void metodaB() { UI::STATUSBAR::printf("jestem metodaB klasy CTest1"); };
};

class CTest2 : public CTest1<float> {
public:
	inline static void metoda2() { UI::STATUSBAR::printf("jestem metoda2 klasy CTest2"); };
};

class CTest3 : public CTest1<float> {
public:
	inline virtual void metoda3() { UI::STATUSBAR::printf("jestem metoda3 klasy CTest3"); };
};
