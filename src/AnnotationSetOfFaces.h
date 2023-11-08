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
	CMesh* m_mesh;

public:
	CAnnotationSetOfFaces( CModel3D *m = nullptr) :CAnnotation( m )
	{
		setLabel("f-indices set");
		m_mesh = nullptr;
	};

	CAnnotationSetOfFaces( const CAnnotationSetOfFaces& p ) :CAnnotation( p ), SetOfFIndices(p)
	{
		setLabel("copy of " + p.m_label);
		m_mesh = p.m_mesh;
	};

	CAnnotationSetOfFaces( const SetOfFIndices& list ) :CAnnotation(), SetOfFIndices(list)
	{
		setLabel("f-indices set");
		m_mesh = nullptr;
	};

	~CAnnotationSetOfFaces() override { SetOfFIndices::clear(); } // m_data.clear(); };

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

	void setDest(CMesh* m) { m_mesh = m; };
	CMesh* getDest();

	inline CMesh*& dstMesh() { return m_mesh; }

	virtual inline void clear() override
	{
		CAnnotation::clear();
		SetOfFIndices::clear();
	};


	CMesh* toMesh();

	virtual int type() { return CAnnotation::Type::SETOFFACES; }

	virtual std::wstring getTypeWSTR() { return L"set of f-indices"; };
	virtual std::wstring getInfoRow();
	
	QString toString(QString prefix = "", QString suffix = "", QString separator = " ");
	
	virtual void renderSelf() override;
};


// dla zachowania zgodnoœci
typedef CAnnotationSetOfFaces CAnnotationVFaces;
