#include "Annotation.h"

#include "../renderers/IAnnotationRenderer.h"
#include "dpLog.h"

CAnnotation::CAnnotation(std::shared_ptr<CBaseObject> parent) : CBaseObject(parent)
{
	m_annotations.clear();
	m_annotationOrder.clear();
	setLabel("annotation");
	m_color = CRGBA(0.0f, 0.0f, 1.0f, 0.4f);
	m_selcolor = CRGBA(1.0f, 0.0f, 0.0f, 0.4f);

	renderer_ = std::make_shared<IAnnotationRenderer>();
}

CAnnotation::CAnnotation(int parentId) : CBaseObject(parentId)
{
	m_annotations.clear();
	m_annotationOrder.clear();
	setLabel("annotation");
	m_color = CRGBA(0.0f, 0.0f, 1.0f, 0.4f);
	m_selcolor = CRGBA(1.0f, 0.0f, 0.0f, 0.4f);

	renderer_ = std::make_shared<IAnnotationRenderer>();
}

// konstruktor kopiuj�cy
CAnnotation::CAnnotation(const CAnnotation& a) : CBaseObject(a)
{
	m_color = a.m_color;
	m_selcolor = a.m_selcolor;

	m_annotations.clear();
	m_annotationOrder.clear();
	for (Annotations::const_iterator it = a.m_annotations.begin(); it != a.m_annotations.end(); it++)
	{
		std::shared_ptr<CAnnotation> child = std::dynamic_pointer_cast<CAnnotation>(it->second->getCopy());
		m_annotations[child->id()] = child;
		m_annotationOrder.append(child->id());
	}
	renderer_ = std::make_shared<IAnnotationRenderer>();
}

void CAnnotation::updateChildrenParentPointers(const std::shared_ptr<CBaseObject>& self) {
	for (auto& child : m_annotations)
		child.second->setParent(self);
}

std::shared_ptr<CBaseObject> CAnnotation::getCopy()
{
	auto obj = std::make_shared<CAnnotation>(*this);

	updateChildrenParentPointers(obj);

	return obj;
}


int CAnnotation::addAnnotation(std::shared_ptr<CAnnotation> parent, std::shared_ptr<CAnnotation> ad)
{
	if (parent == nullptr) return NO_CURRENT_MODEL;
	if (ad == nullptr) return NO_CURRENT_MODEL;

	parent->m_annotations[ad->id()] = ad;
	parent->m_annotationOrder.append(ad->id());
	ad->setParent(parent);

	return ad->id();
}

std::shared_ptr<CAnnotation> CAnnotation::removeAnnotation(int id)
{
	Annotations::iterator it = m_annotations.find(id);
	if (it != m_annotations.end())
	{
		std::shared_ptr<CAnnotation> an = it->second;
		m_annotations.erase(id);
		m_annotationOrder.remove(id);
		return an;
	}
	return nullptr;
}

bool CAnnotation::moveAnnotationBefore(int movedId, int anchorId)
{
	ensureAnnotationOrder();
	return (m_annotations.find(movedId) != m_annotations.end())
		&& (m_annotations.find(anchorId) != m_annotations.end())
		&& m_annotationOrder.moveBefore(movedId, anchorId);
}

bool CAnnotation::moveAnnotationAfter(int movedId, int anchorId)
{
	ensureAnnotationOrder();
	return (m_annotations.find(movedId) != m_annotations.end())
		&& (m_annotations.find(anchorId) != m_annotations.end())
		&& m_annotationOrder.moveAfter(movedId, anchorId);
}

const std::vector<int>& CAnnotation::orderedAnnotationIds()
{
	ensureAnnotationOrder();
	return m_annotationOrder.ids();
}

void CAnnotation::ensureAnnotationOrder()
{
	OrderedIdList normalized;
	for (int id : m_annotationOrder.ids())
	{
		if (m_annotations.find(id) != m_annotations.end())
		{
			normalized.append(id);
		}
	}

	for (const auto& annotation : m_annotations)
	{
		normalized.append(annotation.first);
	}

	m_annotationOrder = normalized;
}

CAnnotation* CAnnotation::annotation(int id)
{
	Annotations::iterator it;
	for (it = m_annotations.begin(); it != m_annotations.end(); it++)
	{
		if (it->first == id) return it->second.get();
		CAnnotation* obj = it->second->annotation(id);
		if (obj != nullptr) return obj;
	}
	return nullptr;
}

std::shared_ptr<CBaseObject> CAnnotation::getSomethingWithId(int id) 
{
	for (const auto& d : m_annotations)
	{
		if (id == d.second->id()) return d.second;
		else if (auto result = d.second->getSomethingWithId(id)) return result;
	}
	return nullptr;
}

