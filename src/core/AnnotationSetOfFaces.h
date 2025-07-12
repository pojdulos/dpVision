#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"

class CMesh;

typedef std::set<INDEX_TYPE> SetOfFIndices;

class DPVISION_EXPORT CAnnotationSetOfFaces : public CAnnotation, public SetOfFIndices
{
	friend class PropAnnotationSetOfFaces;
	std::shared_ptr<CMesh> m_mesh;

public:
	CAnnotationSetOfFaces(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationSetOfFaces( const CAnnotationSetOfFaces& p );;

	CAnnotationSetOfFaces( const SetOfFIndices& list );;

	~CAnnotationSetOfFaces() override { SetOfFIndices::clear(); } // m_data.clear(); };

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		return std::make_shared<CAnnotationSetOfFaces>(*this);
	}

	CAnnotationSetOfFaces& operator=(const CAnnotationSetOfFaces& p)
	{
		SetOfFIndices::operator=(p);
		
		setLabel("copy of " + p.m_label);
		m_mesh = p.m_mesh;
		return *this;
	};

	CAnnotationSetOfFaces& operator=(const SetOfFIndices& list)
	{
		SetOfFIndices::operator=(list);

		setLabel("f-indices set");
		m_mesh = nullptr;
		return *this;
	};

	void setDest(std::shared_ptr<CMesh> m) { m_mesh = m; };
	std::shared_ptr<CMesh> getDest();

	inline std::shared_ptr<CMesh>& dstMesh() { return m_mesh; }

	virtual inline void clear() override
	{
		CAnnotation::clear();
		SetOfFIndices::clear();
	};


	std::shared_ptr<CMesh> toMesh();

	virtual int type() { return CAnnotation::Type::SETOFFACES; }

	virtual std::wstring getTypeWSTR() { return L"set of f-indices"; };
	virtual std::wstring getInfoRow();
	
	QString toString(QString prefix = "", QString suffix = "", QString separator = " ");
	
	virtual void renderSelf() override;
};


// dla zachowania zgodnoœci
typedef CAnnotationSetOfFaces CAnnotationVFaces;
