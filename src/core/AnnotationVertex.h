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

	CAnnotationVertex() : CAnnotation()	{
		setLabel("vertex");
		m_cloud = nullptr;
	};

	CAnnotationVertex(CAnnotationVertex &p) : CAnnotation(p) {
		m_index = p.m_index;
		m_label = p.m_label;
		m_cloud = nullptr;
	};

	CAnnotationVertex(std::shared_ptr<CBaseObject> p, const size_t idx) : CAnnotation(p) {
		m_index = idx;
		setLabel("vertex");
		m_cloud = nullptr;
	};

	CAnnotationVertex( int objId, const size_t idx ) : CAnnotation( objId ) {
		m_index = idx;
		setLabel("vertex");
		m_cloud = nullptr;
	};


	~CAnnotationVertex(void){};

	void setDest(std::shared_ptr<CPointCloud> m) { m_cloud = m; };

	virtual int type() { return CAnnotation::VERTEX; }

	void set(size_t idx) { m_index = idx; };
	size_t get() { return m_index; };

	CVertex *getVertex();

	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

