#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <list>

typedef std::list<CPoint3d> ListOfPoints;

class DPVISION_EXPORT CAnnotationPath : public CAnnotation, public ListOfPoints
{
public:
	CAnnotationPath(std::shared_ptr<CBaseObject> m = nullptr);;

	~CAnnotationPath() override { clear(); };

	void addPoint(const CPoint3d &t) { push_back(t); };
	void addPoint(double x, double y, double z) { push_back(CPoint3d(x, y, z)); };

	virtual int type() { return CAnnotation::PATH; }

	virtual size_t size() { return ListOfPoints::size(); }
	
	virtual inline void clear() override
	{
		CAnnotation::clear();
		ListOfPoints::clear();
	}

	CAnnotationPath::iterator cyclicNext(CAnnotationPath::iterator current);
	CAnnotationPath::iterator cyclicPrev(CAnnotationPath::iterator current);

	virtual std::wstring getTypeWSTR() { return L"path"; };
	virtual std::wstring getInfoRow();
	virtual void renderSelf() override;
};
