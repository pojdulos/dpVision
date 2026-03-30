#pragma once

#include "Wektor3D.h"
#include "RGBA.h"
#include <string>

#include "Object.h"
#include "Transform.h"
#include "OrderedIdList.h"

#include "QColor"

class CModel3D;
//class CTransform;
class QDomElement;

class DPVISION_EXPORT CAnnotation : public CBaseObject
{
public:
	typedef std::map<int, std::shared_ptr<CAnnotation>> Annotations;
private:
	Annotations m_annotations;
	mutable OrderedIdList m_annotationOrder;
public:
	CRGBA m_color;
	CRGBA m_selcolor;

	//unused, always empty, may be used in inheriting class for subAnnotations
	CTransform m_transform;

	CAnnotation( std::shared_ptr<CBaseObject> parent = nullptr);
	CAnnotation( int parentId );
	CAnnotation(const CAnnotation& a);

	virtual void updateChildrenParentPointers(const std::shared_ptr<CBaseObject>& self) override;
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

	const Annotations& annotations() const { return m_annotations; };
	const std::vector<int>& orderedAnnotationIds();

	static int addAnnotation(std::shared_ptr<CAnnotation> parent, std::shared_ptr<CAnnotation> ad);

	std::shared_ptr<CAnnotation> removeAnnotation(int id);
	bool moveAnnotationBefore(int movedId, int anchorId);
	bool moveAnnotationAfter(int movedId, int anchorId);

	CAnnotation* annotation(int id);

	virtual std::shared_ptr<CBaseObject> getSomethingWithId(int id);
	//bool applyParentTransform() override;

	virtual void toDomElement(QDomElement& /*child*/) {};

	std::wstring infoRow() override { return getInfoRow(); };

	virtual inline void clear() override { m_annotations.clear(); m_annotationOrder.clear(); }

	virtual std::wstring getInfoRow() { return m_label.toStdWString(); };
	virtual std::wstring getTypeWSTR() { return L"annotation"; };

private:
	void ensureAnnotationOrder();
};

typedef CAnnotation* PtrAnnotation;
typedef CAnnotation& RefAnnotation;
