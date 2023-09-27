#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"
#include "AnnotationPoint.h"
#include "AnnotationPlane.h"
#include <Fotogrametria.h>


ConcretePlugin::ConcretePlugin(void)
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );

	//UI::MENU::addUserAction( "Test menu", myAction );
	//UI::MESSAGEBOX::error( L"I'm LOADED" );

	m_picking = false;

}

ConcretePlugin::~ConcretePlugin(void)
{
	//UI::MESSAGEBOX::error( L"I'm UNLOADED" );
}


void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, L"Fotogrametria" );

	UI::PLUGINPANEL::addButton(m_ID, L"readCameras", L"Wczytaj kamery", 9, 0, 1, 3);

	//UI::PLUGINPANEL::addEditBox(m_ID, L"editBoxD", L"Rozepchaj o:", L"0.0", 4, 0);
	//UI::PLUGINPANEL::addButton(m_ID, L"rozepchaj", L"Zrób rozepchan¹ kopiê", 5, 0);
/*
UI::PLUGINPANEL::addLabel(m_ID, L"label01", L"Punkt1: n/a", 4, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label02", L"Punkt2: n/a", 5, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label03", L"Punkt3: n/a", 6, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label04", L"Punkt4: n/a", 7, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label05", L"Punkt5: n/a", 8, 0, 1, 3);
	UI::PLUGINPANEL::addButton(m_ID, L"fit_plane", L"P³aszczyzna regr.", 9, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label_centr", L"Punkt centr: n/a", 10, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"label_normal", L"Wektor normalny: n/a", 11, 0, 1, 3);
	UI::PLUGINPANEL::addButton(m_ID, L"set_center", L"Centruj", 12, 0, 1, 3);
	UI::PLUGINPANEL::addButton(m_ID, L"set_center_rot", L"Centruj i rotuj", 13, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"lNormal", L"Normalna: w przód", 14, 0, 1, 3);
	UI::PLUGINPANEL::addButton(m_ID, L"rev_normal", L"Odwróæ normaln¹", 15, 0, 1, 3);
	UI::PLUGINPANEL::addLabel(m_ID, L"lAxis", L"Oœ wiod¹ca: Y", 16, 0, 1, 3);
	UI::PLUGINPANEL::addButton(m_ID, L"set_axX", L"X", 17, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"set_axY", L"Y", 17, 1);
	UI::PLUGINPANEL::addButton(m_ID, L"set_axZ", L"Z", 17, 2);
*/

	//m_picking = true;
}

void ConcretePlugin::onUnload()
{
}


void ConcretePlugin::onRun()
{
}


void ConcretePlugin::onButton( std::wstring name )
{
	if (0 == name.compare(L"readCameras"))
	{
		std::string window_title("Choose file");
		std::string filter("Agisoft XML (*.xml)");
		std::string initial_path("");

		std::string filename = UI::FILECHOOSER::getOpenFileName(window_title, initial_path, filter);
		if (filename == "") return;
		Fotogrametria::loadAgisoftCamerasXML(filename); //kamery_buzka2.xml
	}
	else
	{
	}

	UI::updateAllViews();
}




