#pragma once

#include "Wektor3D.h"
#include "RGBA.h"
#include <string>

#include "Object.h"
#include "Transform.h"

#include "QColor"

class CModel3D;
//class CTransform;
class QDomElement;

class DPVISION_EXPORT CAnnotation : public CBaseObject
{
public:
	CRGBA m_color;
	CRGBA m_selcolor;

	typedef std::map<int, std::shared_ptr<CAnnotation>> Annotations;
	//unused, always empty, may be used in inheriting class for subAnnotations
	Annotations m_annotations; 

	CTransform m_transform;

	CAnnotation( std::shared_ptr<CBaseObject> parent = nullptr) : CBaseObject(parent)
	{
		m_annotations.clear();
		setLabel("annotation");
		m_color = CRGBA(0.0f, 0.0f, 1.0f, 0.4f);
		m_selcolor = CRGBA(1.0f, 0.0f, 0.0f, 0.4f);
	};

	CAnnotation( int parentId ) : CBaseObject( parentId )
	{
		m_annotations.clear();
		setLabel("annotation");
		m_color = CRGBA(0.0f, 0.0f, 1.0f, 0.4f);
		m_selcolor = CRGBA(1.0f, 0.0f, 0.0f, 0.4f);
	};

	// konstruktor kopiuj�cy
	CAnnotation(const CAnnotation& a);

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		auto an = std::make_shared<CAnnotation>(*this);
		return an;
	}

	virtual ~CAnnotation()
	{
		this->clear();
	};

	virtual inline CBaseObject::Category category() { return CBaseObject::Category::ANNOTATION; };
	virtual int type() { return CAnnotation::Type::BASEANNOTATION; }

	inline void setColor(CRGBA c) { m_color = c; };
	inline CRGBA &getColor() { return m_color; };
	inline void setColor(QColor c) { m_color = CRGBA( (unsigned char)c.red(), c.green(), c.blue(), c.alpha() ); };

	inline void setSelColor(CRGBA c) { m_selcolor = c; };
	inline CRGBA &getSelColor() { return m_selcolor; };

	/*deprecated*/ inline CTransform& getTransform() { return m_transform; };
	inline CTransform& transform() { return m_transform; };

	inline Annotations& annotations() { return m_annotations; };

	int addAnnotation(std::shared_ptr<CAnnotation> ad)
	{
		if (ad == nullptr) return NO_CURRENT_MODEL;

		m_annotations[ad->id()] = ad;
		ad->setParent(this->shared_from_this());

		return ad->id();
	}

	std::shared_ptr<CAnnotation> removeAnnotation(int id)
	{
		Annotations::iterator it = m_annotations.find(id);
		if (it != m_annotations.end())
		{
			std::shared_ptr<CAnnotation> an = it->second;
			m_annotations.erase(id);
			return an;
		}
		return nullptr;
	}

	CAnnotation* annotation(int id)
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

	virtual std::shared_ptr<CBaseObject> getSomethingWithId(int id);

	virtual void toDomElement(QDomElement& /*child*/) {};

	std::wstring infoRow() override { return getInfoRow(); };

	virtual inline void clear() override
	{
		//for (auto& a : m_annotations)
		//{
		//	delete a.second;
		//}
		m_annotations.clear();
	}

	virtual std::wstring getInfoRow() { return m_label.toStdWString(); };
	virtual std::wstring getTypeWSTR() { return L"annotation"; };

	virtual void renderTransform() override;
	virtual void renderKids() override;
};

typedef CAnnotation* PtrAnnotation;
typedef CAnnotation& RefAnnotation;
