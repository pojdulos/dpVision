#pragma once
#ifndef _ANIMATION_H_BY_DP_INCLUDED_
#define _ANIMATION_H_BY_DP_INCLUDED_

#include "Global.h"

#include "Mesh.h"
#include "Quaternion.h"
#include "Transform.h"

#include "DPFileInfo.h"

#include "BaseObject.h"

#include "Mesh.h"


#include <set>

class CAnnotation;

//enum _animDir {
//	_ANIM_LEFT = 0,
//	_ANIM_STOPPED_LEFT = 1,
//	_ANIM_RIGHT = 2,
//	_ANIM_STOPPED_RIGHT = 3,
//};

class DPVISION_EXPORT CAnimation : public CObject
{
public:
	typedef enum {
		switchOn,
		switchOff,
		switchToggle } Switch;

	typedef enum {
		//optNone = 0,
		optRenderAsPoints,
		optRenderAsEdges,
		optRenderWithTexture,
		optSmoothVertices } Opt;

protected:
	bool m_bOK;
	std::string m_sError;

	CTransform m_transform;

	bool bDrawBB;


	CFileInfo plikSiatki;

	void renderAxes();
	void renderBoundingBox();

public:
	CAnimation();
	CAnimation( const CAnimation &m );
	virtual ~CAnimation();

	//------------------------------------------------------------------------------------
	// metody dziedziczone:
	//

	inline int type() override { return CObject::Type::ANIMATION; };
	CAnimation *getCopy() override { return new CAnimation(*this); };

	void info(std::wstring i[4]) override;
	std::wstring infoRow() override;

	//------------------------------------------------------------------------------------

	static CAnimation* load(const QString fext, const QString path, bool synchronous = true);

	static CAnimation* load(const QString path, bool synchronous = true);
	static CAnimation* load(const char* path, bool synchronous = true) { return load(QString(path), synchronous); }
	static CAnimation* load(const std::wstring path, bool synchronous = true) { return load(QString::fromStdWString(path), synchronous); }
	static CAnimation* load(const std::string path, bool synchronous = true) { return load(QString::fromStdString(path), synchronous); }

	bool save(const char* path);
	bool save(const QString path);
	bool save(const std::string path);
	bool save(const std::wstring path);

	/*deprecated*/ inline CTransform& getTransform() { return m_transform; };
	inline CTransform& transform() { return m_transform; };
	inline CTransform &setTransform( const CTransform & t=CTransform()) { return m_transform = t; };

	void applyTransform(CTransform to=CTransform());

	//CBaseObject * getSomethingWithId(int id);

	//deprecated, use getChild()
	inline CBaseObject * getData(int id = 0) { return this->getChild(id); };

	bool setLocked(bool b) { return m_transform.lock(b); };
	bool isLocked() { return m_transform.isLocked(); };

	//deprecated, use: setLabel( std::wstring label )
	void setName(std::wstring n) { setLabel(n); }
	
	//deprecated, use: std::wstring getLabel()
	std::wstring getName() { return m_label.toStdWString(); }

	//virtual CBaseObject::Visibility setVisibility(CBaseObject::Visibility v) override;
	//virtual CBaseObject::Visibility switchVisibility() override;

	bool isOK() { return m_bOK; }
	std::string lastError() { return m_sError; }

	const bool toggleDrawBB() { return bDrawBB = !bDrawBB; };
	const bool DrawBB() const { return bDrawBB; };
	const bool DrawBB(const bool b) { return (bDrawBB = b); };

	//using CBaseObject::render;
	virtual void renderTransform() override;
	virtual void renderSelf() override;

	inline CFileInfo &fileInfo(void) { return plikSiatki; }

	inline QString path(void) { return plikSiatki.absolutePath(); };
	inline std::string pathA(void) { return plikSiatki.absolutePath().toStdString(); }
	inline std::wstring pathW(void) { return plikSiatki.absolutePath().toStdWString(); }

	inline void setPath(const QString path) { plikSiatki.setFile(path); };
	inline void setPath(const char* path) { plikSiatki.setFile(QString(path)); };
	inline void setPath(const std::string path) { plikSiatki.setFile(QString::fromStdString(path)); }
	inline void setPath(const std::wstring path) { plikSiatki.setFile(QString::fromStdWString(path)); }

	bool switchOption( CAnimation::Opt iOption, CAnimation::Switch iSet );
	bool testOption( CAnimation::Opt iOption );

	void importChildrenGeometry();
};

//typedef CAnimation* PtrModel3D;
//typedef CAnimation& RefModel3D;

#endif /* _ANIMATION_H_BY_DP_INCLUDED_ */