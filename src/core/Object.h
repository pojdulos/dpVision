#pragma once

#ifndef _Object_H_BY_DP_INCLUDED_
#define _Object_H_BY_DP_INCLUDED_

#include "BaseObject.h"
#include "BoundingBox.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "Global.h"

#include <map>

class CModel3D;
class CFileInfo;
class CAnnotation;
class QString;

class DPVISION_EXPORT CObject	: public CBaseObject, public CBoundingBox
{
public:
	typedef std::map<int, std::shared_ptr<CBaseObject>> Children;
	typedef std::map<int, std::shared_ptr<CAnnotation>> Annotations;

	bool bDrawBB;

	const bool toggleDrawBB() { return bDrawBB = !bDrawBB; };
	const bool DrawBB() const { return bDrawBB; };
	const bool DrawBB(const bool b) { return (bDrawBB = b); };

	Children m_data;
	Annotations m_annotations;

	// konstruktor ze wskazaniem rodzica
	CObject(std::shared_ptr<CBaseObject> p = nullptr);
	CObject(int parentId);

	// konstruktor kopiuj¹cy
	CObject(const CObject &b);


	virtual void updateChildrenParentPointers(const std::shared_ptr<CBaseObject>& self) override;

	virtual std::shared_ptr<CBaseObject> getCopy() override;


	//virtual ~CObject(void);

	//virtual unsigned int getNewId();
	virtual void applyTransformation(CTransform &from, CTransform &to) override;

	virtual inline CBaseObject::Category category() override { return CBaseObject::Category::OBJECT; };
	virtual inline int type() override { return CObject::Type::GENERIC; };

	std::wstring infoRow() override { return L". Object type: Generic CObject()"; };

	void info( std::wstring i[4] ) override;
	
	virtual std::shared_ptr<CBaseObject> getSomethingWithId(int id);

	Annotations& annotations() { return m_annotations; };
	CAnnotation* annotation(int id);

	static int addAnnotation(std::shared_ptr<CObject> parent, std::shared_ptr<CAnnotation> ad);
	std::shared_ptr<CAnnotation> removeAnnotation(int);

	bool removeAnnotation(std::shared_ptr<CAnnotation> an);


	inline bool hasChildren() override { return ! m_data.empty(); };

	virtual void showChildren(bool show, QSet<CBaseObject::Type> inc = {}, QSet<CBaseObject::Type> exc = {}) override;

	
	CBoundingBox & getBoundingBox() { return *this; };
	CBoundingBox & expandBoundingBox(const CPoint3d &p) { CBoundingBox::expand(p); return *this; };
	inline void resetBoundingBox(CBoundingBox::InitialValues v = CBoundingBox::InitialValues::InvalidBB) { CBoundingBox::reset(v); };

	//deprecated, use: expandBoundingBox( point )
	inline void calcMinMax(const CPoint3d &v) { expandBoundingBox(v); };

	static int addChild(std::shared_ptr<CObject> parent, std::shared_ptr<CBaseObject> child );
	bool removeChild( int id = 0 );
	
	/*
	removes the links from this object to all child objects
	and annotations and permanently deletes all these objects
	*/
	void removeAllChilds();
	
	//void removeChild( CBaseObject *d );

	std::shared_ptr<CBaseObject> getChild(int id = 0);
	std::shared_ptr<CBaseObject> getChild(const QString& label);
	//int getChildId( CBaseObject *d );

	Children & children() { return m_data; };
	std::vector<unsigned int> getChildrenIds();

	int countChildren(CBaseObject::Type type);
	std::vector<CBaseObject*> getChildren();
	std::vector<CBaseObject*> getChildren(CBaseObject::Type type);

	virtual std::shared_ptr<CBaseObject> findId( int id ) override;

	static void renderBoundingBox(std::shared_ptr<CObject>);

	//virtual void renderTransform() override;
	//virtual void renderKids() override;
	//virtual void render() override;
};

typedef CObject* PtrObject;
typedef CObject& RefObject;

#endif /* _Object_H_BY_DP_INCLUDED_ */