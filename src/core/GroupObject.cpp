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
		Children::iterator child = m_data.find(m_selectedChildId);
		if (child != m_data.end())
		{
			((CModel3D*)child->second.get())->setChecked(false);
		}
		m_selectedChildId = -1;
	}
	return CModel3D::setChecked(b);
};

const bool CGroupObject::selectChild(int id, bool b)
{
	if (m_selectedChildId != -1)
	{
		((CModel3D*)m_data.find(m_selectedChildId)->second.get())->setChecked(false);
		m_selectedChildId = -1;
	}

	if (b)
	{
		Children::iterator it = m_data.find(id);
		if (it != m_data.end())
		{
			((CModel3D*)it->second.get())->setChecked(b);
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
		Children::iterator it = m_data.find(m_selectedChildId);
		if (it != m_data.end())
		{
			return it->second.get();
		}
	}
	return NULL;
}
