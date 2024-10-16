#include "Annotation.h"

// konstruktor kopiuj¹cy

inline CAnnotation::CAnnotation(const CAnnotation& a) : CBaseObject(a)
{
	m_color = a.m_color;
	m_selcolor = a.m_selcolor;

	m_annotations.clear();
	for (Annotations::const_iterator it = a.m_annotations.begin(); it != a.m_annotations.end(); it++)
	{
		CAnnotation* child = it->second->getCopy();
		child->setParent(this);
		//child->setId(getNewId());
		addAnnotation(child);
	}
}

CBaseObject* CAnnotation::getSomethingWithId(int id)
{
	if (m_Id == id) return this;
	else
	{
		for (const auto& a : m_annotations)
		{
			CBaseObject* result = a.second->getSomethingWithId(id);
			if (result != nullptr) return result;
		}
	}

	return nullptr;
}

void CAnnotation::renderTransform()
{
	if (m_showSelf || m_showKids) m_transform.render();
}

void CAnnotation::renderKids()
{
	//std::cout << "CAnnotation::renderKids(): " << m_label.toStdString() << std::endl;

	for (const auto& it : m_annotations)
	{
		it.second->render();
	}
}
