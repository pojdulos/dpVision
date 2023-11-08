#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <vector>

class DPVISION_EXPORT CAnnotationBezier : public CAnnotation
{
public:
	std::vector<CPoint3d> m_list;
	std::vector<CPoint3d> m_spline;

	int m_type;

	CAnnotationBezier( CModel3D *m = nullptr) :CAnnotation( m )
	{
		setLabel("bezier");
		m_type = 0;
	};

	~CAnnotationBezier() override { m_list.clear();  m_spline.clear(); };

	void clear() { m_list.clear(); m_spline.clear(); };

	CPoint3d deCastelJau(double u, int count, int startIndex);

	std::vector<CPoint3d> krzywa();
	void drawBeziers( int deg, int den, int cut );

	void createTinySpline( int deg, int den );
	void drawTinySpline();

	void addPoint(const CPoint3d &t) { m_list.push_back(t); };
	void addPoint(double x, double y, double z) { m_list.push_back(CPoint3d(x, y, z)); };

	virtual void applyTransformation(CTransform &prevT, CTransform &newT) override;

	virtual int type() override { return CAnnotation::PATH; }

	virtual std::wstring getTypeWSTR() override { return L"bezier"; };
	virtual std::wstring getInfoRow() override;
	
	virtual void renderSelf() override;
};
