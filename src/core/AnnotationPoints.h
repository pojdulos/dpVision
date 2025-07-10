#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <list>

typedef std::list<CPoint3d> SetOfPoints;
//typedef std::set<CPoint3d> SetOfPoints;

class DPVISION_EXPORT CAnnotationPoints : public CAnnotation
{
public:
	SetOfPoints m_list;

	CAnnotationPoints(std::shared_ptr<CBaseObject> m = nullptr) :CAnnotation( m )
	{
		setLabel("points");
		m_list.clear();
	};

	~CAnnotationPoints() override { m_list.clear(); };

	void clear() { m_list.clear(); };

	void addPoint(const CPoint3d &t) { m_list.push_back(t); };
	void addPoint(double x, double y, double z) { m_list.push_back(CPoint3d(x, y, z)); };

	virtual void applyTransformation(CTransform &prevT, CTransform &newT) override;

	virtual int type() override { return CAnnotation::POINTS; }

	virtual std::wstring getTypeWSTR() override { return L"points"; };
	virtual std::wstring getInfoRow() override;
	virtual void renderSelf() override;
};
