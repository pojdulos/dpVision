#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"

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
}


void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, L"Morfologia" );

	UI::PLUGINPANEL::addEditBox(m_ID, L"editBoxD", L"Rozepchaj o:", L"0.0", 4, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"rozepchaj", L"Zrób rozepchaną kopię", 5, 0);
}


#include "AnnotationPlane.h"
#include "AnnotationPoint.h"
#include <vector>

bool ConcretePlugin::onMousePick(Plugin::PickEvent pickEvent)
{
	if (m_picking)
	{
		CAnnotationPoint *pt = new CAnnotationPoint(pickEvent.x, pickEvent.y, pickEvent.z);
		pt->setLabel(L"punkt");
		
		AP::MODEL::addAnnotation(pickEvent.objId, new CAnnotationPoint(pt) );

		std::wstring lab(L"Picked: " + pt->getLabelW() + L" [" + std::to_wstring(pt->X()) + L"," + std::to_wstring(pt->Y()) + L"," + std::to_wstring(pt->Z()) + L"]");
		UI::PLUGINPANEL::setLabel(m_ID, L"label01", lab);

		CAnnotationPlane *pl = new CAnnotationPlane( *pt, CVector3d(0, 1, 1) );
		pl->setSize(100);
		AP::MODEL::addAnnotation(pickEvent.objId, pl);
	}

	return true;
}

//#include "RGBA.h"

void  ConcretePlugin::rozepchaj(CModel3D *o, float d)
{
	CMesh* dstMesh = (CMesh*)(o->getChild());

	dstMesh->calcVN();

	for (int i = 0; i < dstMesh->vnormals().size(); i++)
	{
		CVertex *v1 = &dstMesh->vertices()[i];
		CVector3f *vn1 = &dstMesh->vnormals()[i];

		v1->Set(v1->X() + d*vn1->X(), v1->Y() + d*vn1->Y(), v1->Z() + d*vn1->Z());
		AP::processEvents();
	}
}

void ConcretePlugin::onButton( std::wstring name )
{
	if (0 == name.compare(L"rozepchaj"))
	{
		double d = std::stod(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxD"));

		CModel3D *stary = AP::WORKSPACE::getCurrentModel();

		if (stary != NULL)
		{
			CModel3D *nowy = stary->getCopy();

			nowy->setLabel(L"rozepchany_" + std::to_wstring(d));

			AP::WORKSPACE::addModel(nowy);
			int id = nowy->id();

			rozepchaj(nowy, d);
		}

		AP::WORKSPACE::setCurrentModel(NO_CURRENT_MODEL);
	}
	else
	{
	}
}

