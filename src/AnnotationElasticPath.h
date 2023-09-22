#pragma once
#include "AnnotationPath.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <list>

class __declspec(dllexport) CAnnotationElasticPath : public CAnnotationPath
{
public:
	std::list<CPoint3f> m_data;

	CAnnotationElasticPath() { setLabel("path"); setParent(nullptr); setColor(CRGBA(1.0f, 0.0f, 0.0f, 1.0f)); m_selected = false; };
	
	CAnnotationElasticPath( CModel3D *m ) { setLabel("path"); m_parent = m; m_color = CRGBA(1.0f, 0.0f, 0.0f, 1.0f); m_selected = false; };

	~CAnnotationElasticPath(void) { m_data.clear(); };

	virtual int type() { return CAnnotation::ELASTICPATH; }

	virtual std::wstring getInfoRow();
	virtual void renderSelf() override;
};

//class  __declspec(dllexport) CPathsMap : public std::map<int, CAnnotationPath*> {};
