#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <list>

typedef std::set<INDEX_TYPE> SetOfVIndices;
#define SetOfVertices SetOfVIndices

class DPVISION_EXPORT CAnnotationSetOfVertices : public CAnnotation, public SetOfVIndices
{
public:
	CPointCloud* m_cloud;
	int m_pointSize;

	CAnnotationSetOfVertices(CModel3D* m = nullptr) :CAnnotation(m)
	{
		setLabel("set of vertices");
		m_cloud = nullptr;
		m_pointSize = 5;
	};

	CAnnotationSetOfVertices(const CAnnotationSetOfVertices& a) :CAnnotation((CAnnotation&)a), SetOfVIndices(a)
	{
		setLabel("copy_of_"+a.m_label);
		//m_list.clear();
		m_cloud = a.m_cloud;
		m_pointSize = 5;
	};

	~CAnnotationSetOfVertices() override { SetOfVIndices::clear(); };

	void setDest(CPointCloud* m) { m_cloud = m; };
	void getDest();

	virtual inline void clear() override
	{
		CAnnotation::clear();
		SetOfVIndices::clear();
	};

	virtual int type() override { return CAnnotation::SETOFVERTICES; }

	virtual std::wstring getTypeWSTR() override { return L"set of v-indices"; };
	virtual std::wstring getInfoRow() override;
	QString toString(QString prefix = "", QString suffix = "", QString separator = " ");
	virtual void renderSelf() override;
};
