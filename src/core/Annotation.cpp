#include "Annotation.h"

// konstruktor kopiuj�cy

CAnnotation::CAnnotation(const CAnnotation& a) : CBaseObject(a)
{
	m_color = a.m_color;
	m_selcolor = a.m_selcolor;

	m_annotations.clear();
	for (Annotations::const_iterator it = a.m_annotations.begin(); it != a.m_annotations.end(); it++)
	{
		std::shared_ptr<CAnnotation> child = std::dynamic_pointer_cast<CAnnotation>(it->second->getCopy());
		child->setParent(this->shared_from_this());
		//child->setId(getNewId());
		addAnnotation(child);
	}
}

std::shared_ptr<CBaseObject> CAnnotation::getSomethingWithId(int id)
{
	if (m_Id == id) return shared_from_this();
	else
	{
		for (const auto& a : m_annotations)
		{
			std::shared_ptr<CBaseObject> result = a.second->getSomethingWithId(id);
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
