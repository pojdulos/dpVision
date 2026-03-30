#include "GroupObject.h"

CGroupObject::CGroupObject():CModel3D()
{
	setLabel("nowa grupa");
}


CGroupObject::~CGroupObject()
{
}


const bool CGroupObject::select(const bool b)
{
	if (!b)
	{
		if (std::shared_ptr<CBaseObject> child = getChild(m_selectedChildId))
		{
			((CModel3D*)child.get())->setChecked(false);
		}
		m_selectedChildId = -1;
	}
	return CModel3D::setChecked(b);
};

const bool CGroupObject::selectChild(int id, bool b)
{
	if (m_selectedChildId != -1)
	{
		if (std::shared_ptr<CBaseObject> child = getChild(m_selectedChildId))
		{
			((CModel3D*)child.get())->setChecked(false);
		}
		m_selectedChildId = -1;
	}

	if (b)
	{
		if (std::shared_ptr<CBaseObject> child = getChild(id))
		{
			((CModel3D*)child.get())->setChecked(b);
			m_selectedChildId = id;
		}
		else
		{
			b = false;
		}
	}

	return CModel3D::setChecked(b);
};

CBaseObject * CGroupObject::getSelectedChild()
{
	if (m_selectedChildId != -1)
	{
		if (std::shared_ptr<CBaseObject> child = getChild(m_selectedChildId))
		{
			return child.get();
		}
	}
	return NULL;
}
