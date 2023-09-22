#include "Object.h"

// konstruktor ze wskazaniem rodzica
CObject::CObject(CBaseObject *p) : CBaseObject(p), CBoundingBox()//, CMinMax()
{
	setLabel("object"); 
};

// konstruktor ze wskazaniem rodzica
CObject::CObject(int parentId) : CBaseObject(parentId), CBoundingBox()
{
	setLabel("object");
};

// konstruktor kopiuj¹cy
CObject::CObject(const CObject &b) : CBaseObject(b), CBoundingBox(b) //CMinMax(b)
{
	for (Children::const_iterator it = b.m_data.begin(); it != b.m_data.end(); it++)
	{
		CBaseObject *child = it->second->getCopy();
		child->setParent(this);
		//child->setId(getNewId());
		addChild(child);
	}
};

CObject::~CObject()
{
	while (!m_data.empty())
	{
		delete m_data.begin()->second;
		m_data.erase(m_data.begin());
	}
}

void CObject::applyTransformation(CTransform& from, CTransform& to)
{
	for (auto &c : m_data)
	{
		c.second->applyTransformation(from, to);
	}
}



#include "UI.h"

CObject* CObject::getCopy()
{
	if (!this->hasType(Type::GENERIC))
		UI::MESSAGEBOX::warning("WARNING: you are calling getCopy() function from CObject!!!\nTo avoid unpleasant surprises override this function in inherited class, please.");
	return new CObject(*this);
}


#include "Annotation.h"
CBaseObject* CObject::getSomethingWithId(int id)
{
	if (m_Id == id) return this;
	else
	{
		for (const auto& d : m_data)
		{
			CBaseObject* result = ((CObject*)d.second)->getSomethingWithId(id);
			if (result != nullptr) return result;
		}

		for (const auto& a : m_annotations)
		{
			CBaseObject* result = a.second->getSomethingWithId(id);
			if (result != nullptr) return result;
		}
	}
	
	return nullptr;
}

int CObject::addAnnotation(CAnnotation* ad)
{
	if (ad == nullptr) return NO_CURRENT_MODEL;

	m_annotations[ad->id()] = ad;
	ad->setParent(this);

	return ad->id();
}

CAnnotation* CObject::removeAnnotation(int id)
{
	CBaseObject* an = getSomethingWithId(id);

	if (an == nullptr) return nullptr;
	if (an->category() != CBaseObject::Category::ANNOTATION) return nullptr;

	CBaseObject* parent = an->getParent();

	if (parent == this)
	{
		m_annotations.erase(id);
		return (CAnnotation*)an;
	}
	else
	{
		return ((CAnnotation*)parent)->removeAnnotation(id);
	}

	return nullptr;
}


bool CObject::removeAnnotation(CAnnotation *an)
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



CAnnotation* CObject::annotation(int id)
{
	Annotations::iterator it;
	for (it = m_annotations.begin(); it != m_annotations.end(); it++)
	{
		if (it->first == id) return it->second;
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


int CObject::addChild(CBaseObject *d)
{
	if (d == nullptr) return NO_CURRENT_MODEL;

	d->setParent( this );

	m_data.insert( Children::value_type(d->id(), d) );

	return d->id();
}

CBaseObject * CObject::getChild(int id)
{
	if (m_data.empty()) return nullptr;

	if (id == 0) return m_data.begin()->second;

	if (m_data.find(id) == m_data.end()) return nullptr;
	else return m_data.at(id);
}

CBaseObject* CObject::getChild(const QString& label)
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


//void CObject::removeChild(CBaseObject *d)
//{
//	if (d == NULL) return;
//}

//int CObject::getChildId(CBaseObject *d)
//{
//	if (d == NULL) return 0;
//}

CBaseObject *CObject::findId( int id )
{
	Children::iterator it = m_data.find(id);
	
	if (it != m_data.end())
		return it->second;
	else
	{
		for ( Children::iterator it = m_data.begin(); it != m_data.end(); it++ )
		{
			CBaseObject * res = it->second->findId(id);
			if (res != NULL) return res;
		}
	}

	return NULL;
}

#include "GLViewer.h"

//void CObject::render()
//{
//	if (m_visible != Visibility::HIDE_ALL)
//		renderKids();
//}

void CObject::renderKids()
{
	//std::cout << "CObject::renderKids(): " << m_label.toStdString() << std::endl;
	for (const auto &itd : m_data )
	{
		//if (itd.second->getVisibility() != Visibility::HIDE_ALL)
		{
			switch (itd.second->type())
			{
			case CObject::MODEL:
				itd.second->render();
				break;
			default:
				glLoadName(m_Id);
				glPushName(m_Id);

				itd.second->render();

				glPopName();
				glLoadName(0);
				break;
			}
		}
	}

	for (const auto &it : m_annotations )
	{
		//if (it.second->getVisibility() != Visibility::HIDE_ALL)
		{
			it.second->render();
		}
	}
}

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
			children.push_back(child.second);
	}
	return children;
}

std::vector<CBaseObject*> CObject::getChildren(CBaseObject::Type type)
{
	std::vector<CBaseObject*> children;
	for (auto child : m_data)
	{
		if ((child.second != nullptr) && child.second->hasType(type))
			children.push_back(child.second);
	}
	return children;
}

