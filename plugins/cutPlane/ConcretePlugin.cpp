#include "stdafx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"

#include "OrderedPointCloud.h"

#include "AnnotationPoint.h"
#include "AnnotationPoints.h"
#include "AnnotationVertex.h"
#include "AnnotationPath.h"
#include "AnnotationPlane.h"
#include "AnnotationBoundVectors.h"
#include "AnnotationPyramid.h"
#include "AnnotationSetOfVertices.h"

ConcretePlugin::ConcretePlugin(void)
{
	setName(PLUGIN_NAME);
	CommonDLL::Instance().AddName(PLUGIN_NAME);

	m_picking = false;// true;
}

ConcretePlugin::~ConcretePlugin(void)
{
}

void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create(m_ID, L"Cut Plane");

	UI::PLUGINPANEL::addButton(m_ID, L"makeXZ", L"Płaszczyzna XZ", 5, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"makePlane", L"Płaszczyzna (z punktów)", 6, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"ciach", L"Tnij płaszczyzną", 7, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"scalanie", L"Scal chmury punktów", 23, 0);

	UI::PLUGINPANEL::addButton(m_ID, L"ciach4", L"Wytnij policzek", 40, 0);

}

void ConcretePlugin::onUnload()
{
}


void cieciePlaszczyzna(CModel3D* obj, CAnnotationPlane* m_cutPlane)
{
	if (NULL != obj)
	{
		CPlane p = *m_cutPlane;

		CModel3D* parent = (CModel3D*)m_cutPlane->getParent();

		if (parent != obj)
		{
			CPoint3d p1 = p.m_center;
			CPoint3d p2 = p1 + p.m_normal;

			if (parent != nullptr)
			{
				Eigen::Matrix4d T0 = parent->transform().toEigenMatrix4d();
				p1 = T0 * p1; // do wsp. workspace
				p2 = T0 * p2;
			}

			Eigen::Matrix4d T1 = obj->transform().toEigenMatrix4d().inverse();
			p1 = T1 * p1; // do wsp. obj
			p2 = T1 * p2;

			p.m_center = p1;
			p.m_normal = CVector3d(p1, p2).getNormalized();
		
		}
		
		if ( (obj->getChild()->hasType(CObject::CLOUD)) || (obj->getChild()->hasType(CObject::ORDEREDCLOUD)) )
		{
			CPointCloud* cloud = (CPointCloud*)obj->getChild();

			CPointCloud* rest = new CPointCloud(); // nullptr;

			cloud->cutPlane(&p, rest);

			if (rest != nullptr)
			{
				CModel3D* obj2 = new CModel3D();
				obj2->addChild(rest);
				obj2->importChildrenGeometry();
				obj2->setLabel(obj->getLabel() + ".cut");
				obj2->transform() = obj->transform();
				AP::WORKSPACE::addModel(obj2);
			}
		}
		else if ( obj->getChild()->hasType(CObject::MESH) )
		{
			CMesh* mesh = (CMesh*)obj->getChild();

			if (mesh->hasFaces())
			{
				CMesh* reszta = new CMesh(); // nullptr;
				CMesh* krawedz = new CMesh();  //nullptr;

				CAnnotationPoints* pts = new CAnnotationPoints();

				mesh->cutPlane( p, reszta, krawedz, pts );

				if (krawedz != nullptr)
				{
					CModel3D* obj3 = new CModel3D();
					obj3->addChild(krawedz);
					obj3->importChildrenGeometry();
					obj3->setLabel(obj->getLabel() + ".edge");
					obj3->addAnnotation(pts);
					obj3->transform() = obj->transform();
					AP::WORKSPACE::addModel(obj3);
				}

				if (reszta != nullptr)
				{
					CModel3D* obj2 = new CModel3D();
					obj2->addChild(reszta);
					obj2->importChildrenGeometry();
					obj2->setLabel(obj->getLabel() + ".cut");
					obj2->transform() = obj->transform();
					AP::WORKSPACE::addModel(obj2);
				}
			}
			else
			{
				CPointCloud* rest = new CPointCloud();

				((CPointCloud * )mesh)->cutPlane(m_cutPlane, rest);

				CModel3D* obj2 = new CModel3D();
				obj2->addChild(rest);
				obj2->importChildrenGeometry();
				obj2->setLabel(obj->getLabel() + ".cut");
				obj2->transform() = obj->transform();
				AP::WORKSPACE::addModel(obj2);
			}
		}
		UI::updateAllViews();
	}
}

CPointCloud* scalChmury(CPointCloud* c1, CPointCloud* c2)
{
	CPointCloud* dst = new CPointCloud();

	size_t c1size = 0;
	size_t c2size = 0;
	bool copyColors = false;
	bool copyNormals = false;

	if (NULL != c1)
	{
		c1size = c1->vertices().size();
		copyColors = !c1->vcolors().empty();
		copyNormals = !c1->vnormals().empty();
	}

	if (NULL != c2)
	{
		c2size = c2->vertices().size();
		copyColors &= !c2->vcolors().empty();
		copyNormals &= !c2->vnormals().empty();
	}

	for (int i = 0; i < c1size; i++)
	{
		dst->vertices().push_back(c1->vertices()[i]);

		if (copyColors)
			dst->vcolors().push_back(c1->vcolors()[i]);

		if (copyNormals)
			dst->vnormals().push_back(c1->vnormals()[i]);
	}

	for (int i = 0; i < c2size; i++)
	{
		dst->vertices().push_back(c2->vertices()[i]);

		if (copyColors)
			dst->vcolors().push_back(c2->vcolors()[i]);

		if (copyNormals)
			dst->vnormals().push_back(c2->vnormals()[i]);
	}

	return dst;
}

bool getPoints(CModel3D* obj, std::vector<CPoint3d>& npts)
{
	npts.clear();
	if (NULL != obj)
	{
		for (CModel3D::Annotations::iterator ita = obj->annotations().begin(); ita != obj->annotations().end(); ita++)
		{
			if (ita->second->isSelected())
			{
				if (ita->second->hasType(CAnnotation::VERTEX))
				{
					CMesh* mesh = (CMesh*)obj->getChild();
					npts.push_back(*((CAnnotationVertex*)ita->second)->getVertex());
				}
				else if (ita->second->hasType(CAnnotation::POINT))
				{
					npts.push_back(*((CAnnotationPoint*)ita->second));
				}
				else if (ita->second->hasType(CAnnotation::POINTS))
				{
					CAnnotationPoints* pts = (CAnnotationPoints*)ita->second;
					//std::vector<CPoint3d> tmppts{ std::make_move_iterator(std::begin(pts->m_list)),
					//			  std::make_move_iterator(std::end(pts->m_list)) };

					std::copy(pts->m_list.begin(), pts->m_list.end(), std::back_inserter(npts));
				}
			}
		}
		return !npts.empty();
	}
	return false;
}

CAnnotationPlane* ConcretePlugin::findFirstCheckedPlane()
{
	for (auto m : *AP::WORKSPACE::instance())
	{
		for (CModel3D::Annotations::iterator ita = m.second->annotations().begin(); ita != m.second->annotations().end(); ita++)
		{
			if (ita->second->isSelected())
			{
				if (ita->second->hasType(CAnnotation::PLANE))
				{
					return (CAnnotationPlane*)ita->second;
				}
			}
		}
	}
	return nullptr;
}

void ConcretePlugin::onButton(std::wstring name)
{
	if (0 == name.compare(L"makeXZ"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();

		if (NULL != obj)
		{
			std::vector<CPoint3d> npts;

			m_cutPlane = new CAnnotationPlane( CPoint3d(0,0,0), CVector3d(0,1,0) );

			m_cutPlane->setSize(100.0);
			AP::MODEL::addAnnotation(obj, m_cutPlane);
			UI::updateAllViews();
		}
	}
	else if (0 == name.compare(L"makePlane"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();

		if (NULL != obj)
		{
			std::vector<CPoint3d> npts;

			m_cutPlane = new CAnnotationPlane(obj->id());

			if (getPoints(obj, npts))
			{
				if (npts.size() > 2)
					m_cutPlane->regresionFromPoints(npts);
			}
			AP::MODEL::addAnnotation(obj, m_cutPlane);
			UI::updateAllViews();
		}
	}
	else if (0 == name.compare(L"ciach"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();

		m_cutPlane = findFirstCheckedPlane();

		cieciePlaszczyzna( obj, m_cutPlane );
	}
	else if (0 == name.compare(L"ciach4"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();

		cieciePlaszczyzna(obj, new CAnnotationPlane(CPoint3d(0, 0, 0), CVector3d(0, 1, 0)));
		cieciePlaszczyzna(obj, new CAnnotationPlane(CPoint3d(0, 30, 0), CVector3d(0, -1, 0)));
		cieciePlaszczyzna(obj, new CAnnotationPlane(CPoint3d(0, 0, 70), CVector3d(0, 0, 1)));
		cieciePlaszczyzna(obj, new CAnnotationPlane(CPoint3d(-22, 0, 0), CVector3d(-1, 0, 0)));
	}
	else if (0 == name.compare(L"scalanie"))
	{
		std::list<int> sel = AP::WORKSPACE::SELECTION::getList();

		if (sel.size() > 1)
		{
			std::list<int>::iterator isel = sel.begin();

			CModel3D* modelA = AP::WORKSPACE::getModel(*isel);
			isel++;
			CModel3D* modelB = AP::WORKSPACE::getModel(*isel);

			if (!((modelA->getChild()->hasType(CObject::CLOUD)) || (modelA->getChild()->hasType(CObject::ORDEREDCLOUD)) || (modelA->getChild()->hasType(CObject::MESH))))
			{
				UI::STATUSBAR::setText(L"Obiekt " + modelA->getLabelW() + L" nie jest chmurą punktów (lub siatką).");
				return;
			}

			if (!((modelB->getChild()->hasType(CObject::CLOUD)) || (modelB->getChild()->hasType(CObject::ORDEREDCLOUD)) || (modelB->getChild()->hasType(CObject::MESH))))
			{
				UI::STATUSBAR::setText(L"Obiekt " + modelB->getLabelW() + L" nie jest chmurą punktów (lub siatką).");
				return;
			}

			CPointCloud* meshA = (CPointCloud*)(modelA->getChild());
			CPointCloud* meshB = (CPointCloud*)(modelB->getChild());

			CPointCloud* meshC = scalChmury(meshA, meshB);

			CModel3D* modelC = new CModel3D();

			modelC->addChild(meshC);
			modelC->importChildrenGeometry();

			modelC->setLabel(modelA->getLabel() + "_scalony_z_" + modelB->getLabel());

			AP::WORKSPACE::addModel(modelC);
		}
	}
}
