#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include "BoundVector.h"
#include <list>

class __declspec(dllexport) CAnnotationBoundVectors : public CAnnotation
{
public:
	typedef CBoundVector _BoundVector;
	
	std::list<_BoundVector> m_data;

	CAnnotationBoundVectors( CModel3D *m = nullptr) :CAnnotation( m )
	{
		setLabel("bound_vectors");
		m_data.clear();
	};

	~CAnnotationBoundVectors() override { m_data.clear(); };

	void clear() { m_data.clear(); };

	void addBoundVector(const CPoint3d &p, const CVector3d &v) {
		m_data.push_back( _BoundVector(p, v) );
	};

	void addPairOfPoints(const CPoint3d &p0, const CPoint3d &p1) {
		m_data.push_back( _BoundVector(p0, p1) );
	};

	virtual void applyTransformation(CTransform &prevT, CTransform &newT) override;

	virtual int type() override { return CAnnotation::BOUNDVECTORS; }

	virtual std::wstring getTypeWSTR() override { return L"bound_vectors"; };
	virtual std::wstring getInfoRow() override;
	
	virtual void renderSelf() override;
};
