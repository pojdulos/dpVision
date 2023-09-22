#include "Annotation.h"


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
