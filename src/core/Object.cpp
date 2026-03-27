#include "Object.h"
#include "Object.h"
#include "Object.h"
#include "Annotation.h"

#include "../renderers/IObjectRenderer.h"
#include "dpLog.h"

// konstruktor ze wskazaniem rodzica
CObject::CObject(std::shared_ptr<CBaseObject> p) : CBaseObject(p), CBoundingBox()//, CMinMax()
{
	setLabel("object"); 
	bDrawBB = true;
	m_data.clear();
	m_annotations.clear();
	m_childOrder.clear();
	m_annotationOrder.clear();
	renderer_ = std::make_shared<IObjectRenderer>();
};

// konstruktor ze wskazaniem rodzica
CObject::CObject(int parentId) : CBaseObject(parentId), CBoundingBox()
{
	setLabel("object");
	bDrawBB = true;
	m_data.clear();
	m_annotations.clear();
	m_childOrder.clear();
	m_annotationOrder.clear();
	renderer_ = std::make_shared<IObjectRenderer>();
};

// konstruktor kopiuj�cy
CObject::CObject(const CObject &b) : CBaseObject(b), CBoundingBox(b) //CMinMax(b)
{
	m_data.clear();
	m_annotations.clear();
	m_childOrder.clear();
	m_annotationOrder.clear();

	for (Children::const_iterator it = b.m_data.begin(); it != b.m_data.end(); it++)
	{
		std::shared_ptr<CBaseObject> child = it->second->getCopy();
		if (child)
		{
			//child->setParent(this_shared);
			this->m_data[child->id()] = child;
			this->m_childOrder.append(child->id());
		}
	}

	for (Annotations::const_iterator it = b.m_annotations.begin(); it != b.m_annotations.end(); it++)
	{
		std::shared_ptr<CAnnotation> child = std::dynamic_pointer_cast<CAnnotation>(it->second->getCopy());
		if (child)
		{
			//child->setParent(this_shared);
			this->m_annotations[child->id()] = child;
			this->m_annotationOrder.append(child->id());
		}
	}
	
	bDrawBB = true;
	renderer_ = std::make_shared<IObjectRenderer>();
};




#include "MessageBoxManager.h"

void CObject::updateChildrenParentPointers(const std::shared_ptr<CBaseObject>& self) {
	for (auto& child : m_data)
		child.second->setParent(self);
	for (auto& child : m_annotations)
		child.second->setParent(self);
	// Możesz dodać tutaj inne pola specyficzne dla CObject
}

std::shared_ptr<CBaseObject> CObject::getCopy()
{
	if (!this->hasType(Type::GENERIC))
		MessageBoxManager::warning("WARNING: you are calling getCopy() function from CObject!!!\nTo avoid unpleasant surprises override this function in inherited class, please.");

	auto obj = std::make_shared<CObject>(*this);

	updateChildrenParentPointers(obj);

	return obj;
}




//CObject::~CObject()
//{
//	m_pairs.clear();
//	while (!m_pairs.empty())
//	{
		//try {
		//	delete m_pairs.begin()->second;
		//} catch (...) {
		//	std::cout << "B��d podczas kasowania obiektu potomnego\n";
		//}
//		m_pairs.erase(m_pairs.begin());
//	}

	//auto it = m_pairs.rbegin();
	//while (it != m_pairs.rend())
	//{
	//	delete (*it).second;
	//	it++;
	//}
	//m_pairs.clear();
//}

void CObject::applyTransformation(CTransform& from, CTransform& to)
{
	for (auto &c : m_data)
	{
		c.second->applyTransformation(from, to);
	}
}





#include "Annotation.h"
#include <memory>

std::shared_ptr<CBaseObject> CObject::getSomethingWithId(int id)
{
	for (const auto& d : m_data)
	{
		if (id == d.second->id()) return d.second;
		else if (auto result = d.second->getSomethingWithId(id)) return result;
	}

	for (const auto& d : m_annotations)
	{
		if (id == d.second->id()) return d.second;
		else if (auto result = d.second->getSomethingWithId(id)) return result;
	}
	
	return nullptr;
}

int CObject::addAnnotation(std::shared_ptr<CObject> parent, std::shared_ptr<CAnnotation> ad)
{
	if (parent == nullptr) return NO_CURRENT_MODEL;
	if (ad == nullptr) return NO_CURRENT_MODEL;

	parent->m_annotations[ad->id()] = ad;
	parent->m_annotationOrder.append(ad->id());
	ad->setParent(parent);

	return ad->id();
}

std::shared_ptr<CAnnotation> CObject::removeAnnotation(int id)
{
	auto it = m_annotations.find(id);
	if ( it != m_annotations.end()) 
	{
		std::shared_ptr<CAnnotation> an = it->second;
		m_annotations.erase(id);
		m_annotationOrder.remove(id);
		return an;
	}
	else
	{
		std::shared_ptr<CAnnotation> an = std::dynamic_pointer_cast<CAnnotation>(this->getSomethingWithId(id));
		if (an != nullptr)
			if ( auto p = std::dynamic_pointer_cast<CObject>(an->getParentPtr()) )
				return p->removeAnnotation(id);
			else if (auto p = std::dynamic_pointer_cast<CAnnotation>(an->getParentPtr()))
				return p->removeAnnotation(id);
	}

	return nullptr;
}

bool CObject::moveAnnotationBefore(int movedId, int anchorId)
{
	ensureAnnotationOrder();
	return (m_annotations.find(movedId) != m_annotations.end())
		&& (m_annotations.find(anchorId) != m_annotations.end())
		&& m_annotationOrder.moveBefore(movedId, anchorId);
}

bool CObject::moveAnnotationAfter(int movedId, int anchorId)
{
	ensureAnnotationOrder();
	return (m_annotations.find(movedId) != m_annotations.end())
		&& (m_annotations.find(anchorId) != m_annotations.end())
		&& m_annotationOrder.moveAfter(movedId, anchorId);
}


bool CObject::removeAnnotation(std::shared_ptr<CAnnotation> an)
{
	if (an == nullptr) return false;
	
	Annotations::iterator it = m_annotations.find(an->id());

	if (it != m_annotations.end())
	{
		m_annotations.erase(it);
		m_annotationOrder.remove(an->id());

		return true;
	}
	
	return false;
}

void CObject::showChildren(bool show, QSet<CBaseObject::Type> inc, QSet<CBaseObject::Type> exc)
{
	if (inc.contains((CBaseObject::Type)this->type()) || (inc.empty() && !exc.contains((CBaseObject::Type)this->type())))
		this->setSelfVisibility(show);

	for (auto& kid : this->m_data)
	{
		if (inc.contains((CBaseObject::Type)kid.second->type()) || (inc.empty() && !exc.contains((CBaseObject::Type)kid.second->type())))
		{
			kid.second->setSelfVisibility(show);
		}
		kid.second->showChildren(show, inc, exc);
	}
}



CAnnotation* CObject::annotation(int id)
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



void CObject::info(std::wstring i[4])
{
	i[0] = L"object";
	i[1] = L"???";
	i[2] = L"generic";
	i[3] = L"Object type: Generic CObject()";
}


int CObject::addChild(std::shared_ptr<CObject> parent, std::shared_ptr<CBaseObject> child)
{
	if (parent == nullptr) return NO_CURRENT_MODEL;
	if (child == nullptr) return NO_CURRENT_MODEL;

	child->setParent(parent);

	parent->m_data[child->id()] = child;
	parent->m_childOrder.append(child->id());
	
	return child->id();
}

std::shared_ptr<CBaseObject> CObject::getChild(int id)
{
	if (m_data.empty()) return nullptr;

	if (id == 0)
	{
		ensureChildOrder();
		for (int orderedId : m_childOrder.ids())
		{
			Children::iterator it = m_data.find(orderedId);
			if (it != m_data.end())
			{
				return it->second;
			}
		}
		return nullptr;
	}

	if (m_data.find(id) == m_data.end()) return nullptr;
	else return m_data.at(id);
}

std::shared_ptr<CBaseObject> CObject::getChild(const QString& label)
{
	if (m_data.empty()) return nullptr;

	for (const auto& c : m_data)
	{
		if (c.second->getLabel() == label) return c.second;
	}
	
	return nullptr;
}


bool CObject::removeChild(int id)
{
	if (id == 0)
	{
		ensureChildOrder();
		if (m_childOrder.ids().empty())
		{
			return false;
		}

		const int firstId = m_childOrder.ids().front();
		m_data.erase(firstId);
		m_childOrder.remove(firstId);
		return true;
	}
	else
	{
		Children::iterator it = m_data.find(id);
		if (it != m_data.end())
		{
			m_data.erase(it);
			m_childOrder.remove(id);
			return true;
		}
	}
	return false;
}

bool CObject::moveChildBefore(int movedId, int anchorId)
{
	ensureChildOrder();
	return (m_data.find(movedId) != m_data.end())
		&& (m_data.find(anchorId) != m_data.end())
		&& m_childOrder.moveBefore(movedId, anchorId);
}

bool CObject::moveChildAfter(int movedId, int anchorId)
{
	ensureChildOrder();
	return (m_data.find(movedId) != m_data.end())
		&& (m_data.find(anchorId) != m_data.end())
		&& m_childOrder.moveAfter(movedId, anchorId);
}

void CObject::removeAllChilds()
{
	m_data.clear();
	m_annotations.clear();
	m_childOrder.clear();
	m_annotationOrder.clear();
}


std::shared_ptr<CBaseObject> CObject::findId( int id )
{
	Children::iterator it = m_data.find(id);
	
	if (it != m_data.end())
		return it->second;
	else
	{
		for ( Children::iterator it = m_data.begin(); it != m_data.end(); it++ )
		{
			std::shared_ptr<CBaseObject> res = it->second->findId(id);
			if (res != nullptr) return res;
		}
	}

	return nullptr;
}


std::vector<unsigned int> CObject::getChildrenIds()
{
	std::vector<unsigned int> ids;

	for (int id : orderedChildIds())
	{
		ids.push_back((unsigned int)id);
	}

	return ids;
}

int CObject::countChildren(CBaseObject::Type type)
{
	int counter = 0;
	for (auto child : m_data)
	{
		if ((child.second != nullptr) && child.second->hasType(type))
			counter++;
	}
	return counter;
}

std::vector<CBaseObject*> CObject::getChildren()
{
	std::vector<CBaseObject*> children;
	for (int id : orderedChildIds())
	{
		Children::iterator it = m_data.find(id);
		if ((it != m_data.end()) && (it->second != nullptr))
			children.push_back(it->second.get());
	}
	return children;
}

std::vector<CBaseObject*> CObject::getChildren(CBaseObject::Type type)
{
	std::vector<CBaseObject*> children;
	for (int id : orderedChildIds())
	{
		Children::iterator it = m_data.find(id);
		if ((it != m_data.end()) && (it->second != nullptr) && it->second->hasType(type))
			children.push_back(it->second.get());
	}
	return children;
}

const std::vector<int>& CObject::orderedChildIds()
{
	ensureChildOrder();
	return m_childOrder.ids();
}

const std::vector<int>& CObject::orderedAnnotationIds()
{
	ensureAnnotationOrder();
	return m_annotationOrder.ids();
}

void CObject::ensureChildOrder()
{
	OrderedIdList normalized;
	for (int id : m_childOrder.ids())
	{
		if (m_data.find(id) != m_data.end())
		{
			normalized.append(id);
		}
	}

	for (const auto& child : m_data)
	{
		normalized.append(child.first);
	}

	m_childOrder = normalized;
}

void CObject::ensureAnnotationOrder()
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

