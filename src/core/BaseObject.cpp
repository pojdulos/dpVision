#include "BaseObject.h"
#include "AP.h"
#include "Workspace.h"

#include <QMetaType>

#include "../renderers/IBaseObjectRenderer.h"

// Zarejestruj typ globalnie przy starcie aplikacji
static int _registerCBaseObjectPtrMetaType()
{
	qRegisterMetaType<CBaseObject*>("CBaseObject*");
	qRegisterMetaType<std::shared_ptr<CBaseObject>>("std::shared_ptr<CBaseObject>");
	return 0;
}

// Wykonuje się przy załadowaniu modułu
static const int _dummy = _registerCBaseObjectPtrMetaType();

CBaseObject::CBaseObject(std::shared_ptr<CBaseObject> p)
{
	m_Id = AP::getUniqueId();

	m_label = "baseObject";
	m_descr = "";
	m_path = "";
	m_parent = p;

	m_checked = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;

	renderer_ = std::make_shared<IBaseObjectRenderer>();

};

// konstruktor ze wskazaniem rodzica
CBaseObject::CBaseObject(int objId)
{
	m_Id = AP::getUniqueId();
	
	m_label = "baseObject";
	m_descr = "";
	m_path = "";
	m_parent = CWorkspace::instance()->getSomethingWithId(objId);

	m_checked = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;

	renderer_ = std::make_shared<IBaseObjectRenderer>();
};


// konstruktor kopiuj�cy
CBaseObject::CBaseObject(const CBaseObject &b)
{
	m_Id = AP::getUniqueId();

	setLabel("* " + b.m_label);

	setDescr(b.m_descr);
	
	m_path = b.m_path;

	m_parent = b.m_parent;

	m_checked = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;

	renderer_ = std::make_shared<IBaseObjectRenderer>();
};


void CBaseObject::render() {
	if (renderer_) renderer_->render(this);
}



//void setParent(CBaseObject *p) { m_parent = p; };
void CBaseObject::setParent(int objId) { m_parent = CWorkspace::instance()->getSomethingWithId(objId); }

std::vector<std::shared_ptr<CBaseObject>> CBaseObject::getPathToRoot(std::shared_ptr<CBaseObject> obj) {
	std::vector<std::shared_ptr<CBaseObject>> path;
	std::shared_ptr<CBaseObject> curr = obj;
	while (curr->getParent() != nullptr) {
		curr = curr->getParentPtr();
		path.push_back(curr);
	}
	return path;
}

std::shared_ptr<CBaseObject> CBaseObject::getRoot(std::shared_ptr<CBaseObject> obj) {
	std::vector<std::shared_ptr<CBaseObject>> path = CBaseObject::getPathToRoot(obj);
	
	if (path.empty()) // this object is root
		return obj;
	
	return path.back();
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

	std::shared_ptr<CBaseObject> parent = obj->getParentPtr();
	while (parent != nullptr)
	{
		if (parent->hasTransformation())
		{
			Eigen::Matrix4d kolejnaMacierzZlisty = parent->getTransformationMatrix();
			aktualnaMacierz = kolejnaMacierzZlisty * aktualnaMacierz;
		}
		parent = parent->getParentPtr();
	}

	std::cout << std::endl << "TESTOWA:" << std::endl << aktualnaMacierz << std::endl;
}

Eigen::Matrix4d CBaseObject::getGlobalTransformationMatrix(std::shared_ptr<CBaseObject> obj)
{
	Eigen::Matrix4d aktualnaMacierz = Eigen::Matrix4d::Identity();

	if (obj) {
		std::vector<std::shared_ptr<CBaseObject>> fifo;

		fifo.push_back(obj);

		std::shared_ptr<CBaseObject> parent = obj->getParentPtr();
		while (parent != nullptr)
		{
			fifo.push_back(parent);
			parent = parent->getParentPtr();
		}

		for (int i = 0; i < fifo.size(); i++)
		{
			std::shared_ptr<CBaseObject> currentObj = fifo[i];

			if (currentObj->hasTransformation())
			{
				Eigen::Matrix4d kolejnaMacierzZlisty = currentObj->getTransformationMatrix();
				aktualnaMacierz = kolejnaMacierzZlisty * aktualnaMacierz;
			}
		}

		//std::cout << std::endl << aktualnaMacierz << std::endl;
		//testMM(this);
	}
	return aktualnaMacierz;
}


#include "dpLog.h"
CBaseObject::~CBaseObject(void) {
	dpDebug() << "destuctor ~CBaseObject, id=" << m_Id;
};

