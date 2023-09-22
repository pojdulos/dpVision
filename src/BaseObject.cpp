#include "BaseObject.h"
#include "AP.h"
#include "Workspace.h"


// konstruktor ze wskazaniem rodzica
CBaseObject::CBaseObject(CBaseObject *p)
{
	m_Id = AP::getUniqueId();
	
	m_label = "baseObject";
	m_descr = "";
	m_parent = p;

	//m_visible = Visibility::SHOW;
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
	m_parent = AP::WORKSPACE::getModel(objId);

	//m_visible = Visibility::SHOW;
	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;
};


// konstruktor kopiuj¹cy
CBaseObject::CBaseObject(const CBaseObject &b)
{
	m_Id = AP::getUniqueId();

	setLabel("copy of " + b.m_label);
	setDescr(b.m_descr);

	m_parent = b.m_parent;

	//m_visible = Visibility::SHOW;
	m_selected = false;

	m_showSelf = true;
	m_showKids = true;
	m_modified = true;
};


inline void CBaseObject::render()
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

//CBaseObject::Visibility CBaseObject::setVisibility(CBaseObject::Visibility v)
//{
//	return (m_visible = v);
//}
//
//inline CBaseObject::Visibility CBaseObject::getVisibility()
//{
//	return m_visible;
//}
//
//CBaseObject::Visibility CBaseObject::switchVisibility()
//{
//	if (m_visible == HIDE_ALL)
//		return m_visible = HIDE_SELF;
//	else if (m_visible == HIDE_SELF)
//		return m_visible = SHOW;
//	else
//		return m_visible = HIDE_ALL;
//}

