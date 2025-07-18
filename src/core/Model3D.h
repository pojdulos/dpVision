#pragma once
#ifndef _MODEL_H_BY_DP_INCLUDED_
#define _MODEL_H_BY_DP_INCLUDED_

//#include "Global.h"

#include "Mesh.h"
//#include "Quaternion.h"
#include "Transform.h"

#include "DPFileInfo.h"

#include "BaseObject.h"

#include "Mesh.h"


#include <set>

class CAnnotation;
class IProgressListener;

//enum _animDir {
//	_ANIM_LEFT = 0,
//	_ANIM_STOPPED_LEFT = 1,
//	_ANIM_RIGHT = 2,
//	_ANIM_STOPPED_RIGHT = 3,
//};

class DPVISION_EXPORT CModel3D : public CObject
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


	//CFileInfo plikSiatki;

	//void renderAxes();
	//void renderBoundingBox();

public:
	CModel3D(std::shared_ptr<CBaseObject> p = nullptr);
	CModel3D( const CModel3D &m );
	//virtual ~CModel3D();

	//------------------------------------------------------------------------------------
	// metody dziedziczone:
	//

	inline int type() override { return CObject::Type::MODEL; };
	virtual std::shared_ptr<CBaseObject> getCopy() override;;

	void info(std::wstring i[4]) override;
	std::wstring infoRow() override;

	//------------------------------------------------------------------------------------

	static std::shared_ptr<CModel3D> load(const QString fext, const QString path, bool synchronous = true);

	static std::shared_ptr<CModel3D> load(const QString path, bool synchronous = true, std::shared_ptr<IProgressListener> prg = nullptr);
	static std::shared_ptr<CModel3D> load(const char* path, bool synchronous = true) { return load(QString(path), synchronous); }
	static std::shared_ptr<CModel3D> load(const std::wstring path, bool synchronous = true) { return load(QString::fromStdWString(path), synchronous); }
	static std::shared_ptr<CModel3D> load(const std::string path, bool synchronous = true) { return load(QString::fromStdString(path), synchronous); }

	bool save(const char* path);
	bool save(const QString path);
	bool save(const std::string path);
	bool save(const std::wstring path);

	/*deprecated*/ inline CTransform& getTransform() { return m_transform; };
	inline CTransform& transform() { return m_transform; };
	inline CTransform &setTransform( const CTransform & t=CTransform()) { return m_transform = t; };

	void applyTransform(CTransform to=CTransform());

	virtual bool hasTransformation() override { return true; };
	virtual Eigen::Matrix4d getTransformationMatrix() override { return this->m_transform.toEigenMatrix4d(); };


	//CBaseObject * getSomethingWithId(int id);

	//deprecated, use getChild()
	inline std::shared_ptr<CBaseObject> getData(int id = 0) { return this->getChild(id); };

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

	//using CBaseObject::render;
	//virtual void renderTransform() override;
	//virtual void renderSelf() override;

	//inline CFileInfo &fileInfo(void) { return CFileInfo(m_path); }

	inline virtual const QString& path() override { return m_path; }
	inline std::string pathA(void) { return m_path.toStdString(); }
	inline std::wstring pathW(void) { return m_path.toStdWString(); }

	inline virtual void setPath(const QString& path) override { m_path = path; }
	inline void setPath(const char* path) { m_path = QString(path); };
	inline void setPath(const std::string path) { m_path = QString::fromStdString(path); }
	inline void setPath(const std::wstring path) { m_path = QString::fromStdWString(path); }

	bool switchOption( CModel3D::Opt iOption, CModel3D::Switch iSet );
	bool testOption( CModel3D::Opt iOption );

	//kolejne do przemy�lenia:
	void prepare();
	
	/*deprecated*/ void calcVN();
	
	void PMeshEcoll( GLuint rzm, bool checkPoints=false );
	void PMeshVsplit( GLuint rzm );
	
	void importChildrenGeometry();
	/*deprecated*/ inline void importMeshGeometry() { importChildrenGeometry(); };

	void setOrigin(CTransform::Origin t);
};

typedef CModel3D* PtrModel3D;
typedef CModel3D& RefModel3D;

#endif /* _MODEL_H_BY_DP_INCLUDED_ */