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
	m_parent = p.get();

	m_selected = false;

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
	m_parent = AP::WORKSPACE::getModel(objId).get();

	m_selected = false;

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

	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;

	renderer_ = std::make_shared<IBaseObjectRenderer>();
};


void CBaseObject::render() {
	if (renderer_) renderer_->render(this);
}

void CBaseObject::setParent(int objId)
{
	m_parent = AP::WORKSPACE::getModel(objId).get();
}

std::vector<std::shared_ptr<CBaseObject>> CBaseObject::getPathToRoot() {
	std::vector<std::shared_ptr<CBaseObject>> path;
	std::shared_ptr<CBaseObject> curr = std::shared_ptr<CBaseObject>(this);
	while (curr->getParent() != nullptr) {
		curr = curr->getParentPtr();
		path.push_back(curr);
	}
	return path;
}

std::shared_ptr<CBaseObject> CBaseObject::getRoot() {
	std::vector<std::shared_ptr<CBaseObject>> path = getPathToRoot();
	
	if (path.empty()) // this object is root
		return this->shared_from_this();
	
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

Eigen::Matrix4d CBaseObject::getGlobalTransformationMatrix()
{
	Eigen::Matrix4d aktualnaMacierz = Eigen::Matrix4d::Identity();

	std::vector<std::shared_ptr<CBaseObject>> fifo;

	fifo.push_back(this->shared_from_this());

	std::shared_ptr<CBaseObject> parent = this->getParentPtr();
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

	return aktualnaMacierz;
}

