#include "Global.h"
#include "AnnotationVertex.h"
#include "Mesh.h"

#include "../api/AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationVertexRenderer.h"

CAnnotationVertex::CAnnotationVertex() : CAnnotation() {
	setLabel("vertex");
	m_cloud = nullptr;
	renderer_ = std::make_shared<IAnnotationVertexRenderer>();
}

CAnnotationVertex::CAnnotationVertex(CAnnotationVertex& p) : CAnnotation(p) {
	m_index = p.m_index;
	m_label = p.m_label;
	m_cloud = nullptr;
	renderer_ = std::make_shared<IAnnotationVertexRenderer>();
}

CAnnotationVertex::CAnnotationVertex(std::shared_ptr<CBaseObject> p, const size_t idx) : CAnnotation(p) {
	m_index = idx;
	setLabel("vertex");
	m_cloud = nullptr;
	renderer_ = std::make_shared<IAnnotationVertexRenderer>();
}

CAnnotationVertex::CAnnotationVertex(int objId, const size_t idx) : CAnnotation(objId) {
	m_index = idx;
	setLabel("vertex");
	m_cloud = nullptr;
	renderer_ = std::make_shared<IAnnotationVertexRenderer>();
}


std::wstring CAnnotationVertex::getInfoRow()
{
	std::wstring info = L"Vertex index: " + std::to_wstring(m_index);

	return info;
}


CVertex *CAnnotationVertex::getVertex()
{
	std::shared_ptr<CPointCloud> cloud = std::dynamic_pointer_cast<CPointCloud>(m_parent.lock());

	if (cloud && (cloud->vertices().size() > m_index)) return &cloud->vertices()[m_index];

	if (auto shp = std::dynamic_pointer_cast<CObject>(m_parent.lock())) {
		cloud = std::dynamic_pointer_cast<CPointCloud>(shp->getChild()); // only first child 
		if (cloud && (cloud->vertices().size() > m_index)) return &cloud->vertices()[m_index];
	}
	return nullptr;
};

