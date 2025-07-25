#include "Global.h"
#include "PolygonGroup.h"
#include "GLViewer.h"

CPolygonGroup::CPolygonGroup( CBaseObject* p ) //: CBaseObject( p )
{
	m_parent = p;
	m_matIdx = 0;

	//initQGLShaderProgram();
}

CPolygonGroup::CPolygonGroup( CPolygonGroup &m ) //: CBaseObject( m )
{
	m_parent = m.m_parent;
	m_faces = CPolygonGroup::Faces(m.m_faces);
	m_colors = CPolygonGroup::Colors(m.m_colors);
	m_normals = CPolygonGroup::Normals(m.m_normals);

	m_matIdx = 0;

	//initQGLShaderProgram();
}


CPolygonGroup::~CPolygonGroup()
{
	m_faces.clear();
	m_colors.clear();
	m_normals.clear();

	//releaseQGLShaderProgram();
}

CPolygonGroup& CPolygonGroup::operator=(const CPolygonGroup& m)
{
	m_parent = m.m_parent;
	m_faces = CPolygonGroup::Faces(m.m_faces);
	m_colors = CPolygonGroup::Colors(m.m_colors);
	m_normals = CPolygonGroup::Normals(m.m_normals);

	m_matIdx = 0;

	return *this;
}

#include "Mesh.h"

INDEX_TYPE CPolygonGroup::addFace(CFace f)
{
	m_faces.push_back(f);
	if (!m_colors.empty() && m_colors.size() == m_faces.size() - 1)
		m_colors.push_back(CRGBA());
	return m_faces.size() - 1;
}

INDEX_TYPE CPolygonGroup::addFace(CFace f, CRGBA fColor)
{
	m_faces.push_back(f);
	if (m_colors.size() == m_faces.size() - 1)
		m_colors.push_back(fColor);
	else
	{
		m_colors.resize(m_faces.size(),CRGBA());
		m_colors[m_faces.size() - 1] = fColor;
	}
	return m_faces.size() - 1;
}

void CPolygonGroup::removeFace(INDEX_TYPE idx)
{
	m_faces.erase( m_faces.begin() + idx );
	
	if (hasColors())
	{
		m_colors.erase( m_colors.begin() + idx );
	}

	if (hasNormals())
	{
		m_normals.erase( m_normals.begin() + idx );
	}
}

void CPolygonGroup::removeFace(CPolygonGroup::Faces::iterator itf)
{
	size_t idx = itf - m_faces.begin();

	m_faces.erase( itf );

	if (hasColors())
	{
		m_colors.erase(m_colors.begin() + idx);
	}

	if (hasNormals())
	{
		m_normals.erase(m_normals.begin() + idx);
	}
}

void CPolygonGroup::removeFace(CPolygonGroup::Faces::reverse_iterator itfr)
{
	// &*(rit.base() - 1) == &*rit
	// https://en.cppreference.com/w/cpp/iterator/reverse_iterator/base

	removeFace( itfr.base() - 1 );
}

