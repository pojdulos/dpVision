#include "BaseObject.h"
#include "AP.h"
#include "Workspace.h"

#include <QMetaType>

// Zarejestruj typ globalnie przy starcie aplikacji
static int _registerCBaseObjectPtrMetaType()
{
	qRegisterMetaType<CBaseObject*>("CBaseObject*");
	return 0;
}

// Wykonuje się przy załadowaniu modułu
static const int _dummy = _registerCBaseObjectPtrMetaType();

// konstruktor ze wskazaniem rodzica
CBaseObject::CBaseObject(CBaseObject *p)
{
	m_Id = AP::getUniqueId();
	
	m_label = "baseObject";
	m_descr = "";
	m_path = "";
	m_parent = p;

	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;
};

// konstruktor ze wskazaniem rodzica
CBaseObject::CBaseObject(int objId)
{
	m_Id = AP::getUniqueId();
	
	m_label = "baseObject";
	m_descr = "";
	m_path = "";
	m_parent = AP::WORKSPACE::getModel(objId);

	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;
};


// konstruktor kopiuj�cy
CBaseObject::CBaseObject(const CBaseObject &b)
{
	m_Id = AP::getUniqueId();

	setLabel("* " + b.m_label);

	setDescr(b.m_descr);
	
	m_path = b.m_path;

	m_parent = b.m_parent;

	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;
};


void CBaseObject::render()
{
	glPushMatrix();
	renderTransform();
	if (m_showSelf) renderSelf();
	if (m_showKids) renderKids();
	glPopMatrix();
}

void CBaseObject::setParent(int objId)
{
	m_parent = AP::WORKSPACE::getModel(objId);
}

bool CBaseObject::switchSelfVisibility()
{
	setSelfVisibility(!m_showSelf);
	return m_showSelf;
}

bool CBaseObject::switchKidsVisibility()
{
	setKidsVisibility(!m_showKids);
	return m_showKids;
}


void testMM(CBaseObject* obj)
{
	Eigen::Matrix4d aktualnaMacierz = obj->getTransformationMatrix();

	CBaseObject* parent = obj->getParent();
	while (parent != nullptr)
	{
		if (parent->hasTransformation())
		{
			Eigen::Matrix4d kolejnaMacierzZlisty = parent->getTransformationMatrix();
			aktualnaMacierz = kolejnaMacierzZlisty * aktualnaMacierz;
		}
		parent = parent->getParent();
	}

	std::cout << std::endl << "TESTOWA:" << std::endl << aktualnaMacierz << std::endl;
}

Eigen::Matrix4d CBaseObject::getGlobalTransformationMatrix()
{
	Eigen::Matrix4d aktualnaMacierz = Eigen::Matrix4d::Identity();

	std::vector<CBaseObject*> fifo;

	fifo.push_back(this);

	CBaseObject* parent = this->getParent();
	while (parent != nullptr)
	{
		fifo.push_back(parent);
		parent = parent->getParent();
	}

	for (int i = 0; i < fifo.size(); i++)
	{
		CBaseObject* currentObj = fifo[i];

		if (currentObj->hasTransformation())
		{
			Eigen::Matrix4d kolejnaMacierzZlisty = currentObj->getTransformationMatrix();
			aktualnaMacierz = kolejnaMacierzZlisty * aktualnaMacierz;
		}
	}

	//std::cout << std::endl << aktualnaMacierz << std::endl;
	//testMM(this);

	return aktualnaMacierz;
}

