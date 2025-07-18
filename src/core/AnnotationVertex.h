#pragma once
#include "Annotation.h"

#include "Vertex.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

class CPointCloud;

class DPVISION_EXPORT CAnnotationVertex : public CAnnotation
{
	size_t m_index;

public:
	std::shared_ptr<CPointCloud> m_cloud;

	CAnnotationVertex();;

	CAnnotationVertex(CAnnotationVertex &p);;

	CAnnotationVertex(std::shared_ptr<CBaseObject> p, const size_t idx);;

	CAnnotationVertex( int objId, const size_t idx );;


	~CAnnotationVertex(void){};

	void setDest(std::shared_ptr<CPointCloud> m) { m_cloud = m; };

	virtual int type() { return CAnnotation::VERTEX; }

	void set(size_t idx) { m_index = idx; };
	size_t get() { return m_index; };

	CVertex *getVertex();

	virtual std::wstring getInfoRow();
};

