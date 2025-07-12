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

	CAnnotation( std::shared_ptr<CBaseObject> parent = nullptr);
	CAnnotation( int parentId );
	CAnnotation(const CAnnotation& a);

	virtual std::shared_ptr<CBaseObject> getCopy() override;

	virtual ~CAnnotation() { this->clear(); };

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

	int addAnnotation(std::shared_ptr<CAnnotation> ad);

	std::shared_ptr<CAnnotation> removeAnnotation(int id);

	CAnnotation* annotation(int id);

	virtual std::shared_ptr<CBaseObject> getSomethingWithId(int id);

	virtual void toDomElement(QDomElement& /*child*/) {};

	std::wstring infoRow() override { return getInfoRow(); };

	virtual inline void clear() override { m_annotations.clear(); }

	virtual std::wstring getInfoRow() { return m_label.toStdWString(); };
	virtual std::wstring getTypeWSTR() { return L"annotation"; };
};

typedef CAnnotation* PtrAnnotation;
typedef CAnnotation& RefAnnotation;
