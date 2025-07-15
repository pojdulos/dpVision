#include "Object.h"
#include "Object.h"
#include "Object.h"
#include "Annotation.h"

#include "../renderers/IObjectRenderer.h"

// konstruktor ze wskazaniem rodzica
CObject::CObject(std::shared_ptr<CBaseObject> p) : CBaseObject(p), CBoundingBox()//, CMinMax()
{
	setLabel("object"); 
	bDrawBB = true;
	m_data.clear();
	m_annotations.clear();
	renderer_ = std::make_shared<IObjectRenderer>();
};

// konstruktor ze wskazaniem rodzica
CObject::CObject(int parentId) : CBaseObject(parentId), CBoundingBox()
{
	setLabel("object");
	bDrawBB = true;
	m_data.clear();
	m_annotations.clear();
	renderer_ = std::make_shared<IObjectRenderer>();
};

// konstruktor kopiuj�cy
CObject::CObject(const CObject &b) : CBaseObject(b), CBoundingBox(b) //CMinMax(b)
{
	m_data.clear();
	m_annotations.clear();

	for (Children::const_iterator it = b.m_data.begin(); it != b.m_data.end(); it++)
	{
		std::shared_ptr<CBaseObject> child = it->second->getCopy();
		if (child)
		{
			child->setParent(this);
			this->m_data[child->id()] = child;
		}
	}

	for (Annotations::const_iterator it = b.m_annotations.begin(); it != b.m_annotations.end(); it++)
	{
		std::shared_ptr<CAnnotation> child = std::dynamic_pointer_cast<CAnnotation>(it->second->getCopy());
		if (child)
		{
			child->setParent(this);
			this->m_annotations[child->id()] = child;
		}
	}
	
	bDrawBB = true;
	renderer_ = std::make_shared<IObjectRenderer>();
};



//CObject::~CObject()
//{
//	m_data.clear();
//	while (!m_data.empty())
//	{
		//try {
		//	delete m_data.begin()->second;
		//} catch (...) {
		//	std::cout << "B��d podczas kasowania obiektu potomnego\n";
		//}
//		m_data.erase(m_data.begin());
//	}

	//auto it = m_data.rbegin();
	//while (it != m_data.rend())
	//{
	//	delete (*it).second;
	//	it++;
	//}
	//m_data.clear();
//}

void CObject::applyTransformation(CTransform& from, CTransform& to)
{
	for (auto &c : m_data)
	{
		c.second->applyTransformation(from, to);
	}
}



#include "MessageBoxManager.h"

std::shared_ptr<CBaseObject> CObject::getCopy()
{
	if (!this->hasType(Type::GENERIC))
		MessageBoxManager::warning("WARNING: you are calling getCopy() function from CObject!!!\nTo avoid unpleasant surprises override this function in inherited class, please.");
	return std::make_shared<CObject>(*this);
}


#include "Annotation.h"
#include <memory>

std::shared_ptr<CBaseObject> CObject::getSomethingWithId(int id)
{
	if (m_Id == id) return shared_from_this();
	else
	{
		for (const auto& d : m_data)
		{
			if (d.second->hasCategory(CBaseObject::OBJECT))
			{
				std::shared_ptr<CBaseObject> result = ((CObject*)d.second.get())->getSomethingWithId(id);
				if (result != nullptr) return result;
			}
		}

		for (const auto& a : m_annotations)
		{
			if (a.second->hasCategory(CBaseObject::ANNOTATION))
			{
				std::shared_ptr<CBaseObject> result = a.second->getSomethingWithId(id);
				if (result != nullptr) return result;
			}
		}
	}
	
	return nullptr;
}

int CObject::addAnnotation(std::shared_ptr<CAnnotation> ad)
{
	if (ad == nullptr) return NO_CURRENT_MODEL;

	m_annotations[ad->id()] = ad;
	ad->setParent(this->shared_from_this());

	return ad->id();
}

std::shared_ptr<CAnnotation> CObject::removeAnnotation(int id)
{
	auto it = m_annotations.find(id);
	if ( it != m_annotations.end()) 
	{
		std::shared_ptr<CAnnotation> an = it->second;
		m_annotations.erase(id);
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


bool CObject::removeAnnotation(std::shared_ptr<CAnnotation> an)
{
	if (an == nullptr) return false;
	
	//bool isAnno = an->hasCategory(CBaseObject::Category::ANNOTATION);

	//if (! isAnno )
	//	return false;

	Annotations::iterator it = m_annotations.find(an->id());

	if (it != m_annotations.end())
	{
		m_annotations.erase(it);

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


int CObject::addChild(std::shared_ptr<CBaseObject> d)
{
	if (d == nullptr) return NO_CURRENT_MODEL;

	d->setParent(this->shared_from_this());

	this->m_data[d->id()] = d;

	return d->id();
}

std::shared_ptr<CBaseObject> CObject::getChild(int id)
{
	if (m_data.empty()) return nullptr;

	if (id == 0) return m_data.begin()->second;

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
		m_data.erase(m_data.begin());
		return true;
	}
	else
	{
		Children::iterator it = m_data.find(id);
		if (it != m_data.end())
		{
			m_data.erase(it);
			return true;
		}
	}
	return false;
}

void CObject::removeAllChilds()
{
	m_data.clear();
	m_annotations.clear();
}


//void CObject::removeChild(CBaseObject *d)
//{
//	if (d == NULL) return;
//}

//int CObject::getChildId(CBaseObject *d)
//{
//	if (d == NULL) return 0;
//}

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

#include "GLViewer.h"

//void CObject::render()
//{
//	if (m_visible != Visibility::HIDE_ALL)
//		renderKids();
//}

void CObject::renderBoundingBox()
{
	QVector<std::shared_ptr<CBaseObject>> objts = UI::DOCK::WORKSPACE::getSelectedObjects();

	CBoundingBox::Style style = objts.contains(this->shared_from_this()) ? CBoundingBox::Style::Unlocked : CBoundingBox::Style::NotSelected;
	
	CBoundingBox::draw(style, this->isSelected());
}

//void CObject::renderTransform()
//{
//	if (bDrawBB) renderBoundingBox();
//}
//
//void CObject::renderKids()
//{
//	//std::cout << "CObject::renderKids(): " << m_label.toStdString() << std::endl;
//	for (const auto &itd : m_data )
//	{
//		//if (itd.second->getVisibility() != Visibility::HIDE_ALL)
//		{
//			switch (itd.second->type())
//			{
//			case CObject::MODEL:
//				itd.second->render();
//				break;
//			default:
//				glLoadName(m_Id);
//				glPushName(m_Id);
//
//				itd.second->render();
//
//				glPopName();
//				glLoadName(0);
//				break;
//			}
//		}
//	}
//
//	for (const auto &it : m_annotations )
//	{
//		//if (it.second->getVisibility() != Visibility::HIDE_ALL)
//		{
//			it.second->render();
//		}
//	}
//}

std::vector<unsigned int> CObject::getChildrenIds()
{
	std::vector<unsigned int> ids;

	for (CObject::Children::iterator it = m_data.begin(); it != m_data.end(); it++)
	{
		ids.push_back(it->first);
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
	for (auto child : m_data)
	{
		if (child.second != nullptr)
			children.push_back(child.second.get());
	}
	return children;
}

std::vector<CBaseObject*> CObject::getChildren(CBaseObject::Type type)
{
	std::vector<CBaseObject*> children;
	for (auto child : m_data)
	{
		if ((child.second != nullptr) && child.second->hasType(type))
			children.push_back(child.second.get());
	}
	return children;
}

