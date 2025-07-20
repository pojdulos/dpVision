#include "Global.h"
#include "AnnotationEdges.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


#include "../renderers/IAnnotationEdgesRenderer.h"

CAnnotationEdges::CAnnotationEdges(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("edge_set");
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}

CAnnotationEdges::CAnnotationEdges(CAnnotationEdges& p) : CAnnotation(p)
{
	setLabel("edge_set");
	m_list = p.m_list;
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}

CAnnotationEdges::CAnnotationEdges(std::list<CAnnotationEdge>& list, std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("edge_set");
	m_list = list;
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}



void CAnnotationEdges::addEdge(CPoint3f f, CPoint3f s, size_t fId)
{
	CAnnotationEdge e;
	e.first = f;
	e.second = s;
	e.faces.insert(fId);

	m_list.push_back(e);
}

void CAnnotationEdges::addEdge(CPoint3f f, CPoint3f s)
{
	CAnnotationEdge e;
	e.first = f;
	e.second = s;

	m_list.push_back(e);
}

//void CAnnotationEdges::addEdge(CEdge ed)
//{
//	CAnnotationEdge e;
//	e.first = ed.first;
//	e.second = ed.seconds;
//	e.faces.insert(ed.face());
//
//	m_list.push_back(e);
//}

std::wstring CAnnotationEdges::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_list.size()) + L" edges.";

	return info;
}

void CAnnotationEdges::renderSelf()
{
	if (this->m_list.size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (m_checked)
		{
			glLineWidth(1.0);
			glColor4ubv( m_selcolor.V() );
		}
		else
		{
			glLineWidth(1.0);
			glColor4ubv( m_color.V() );
		}

		glBegin(GL_LINES);

		std::list<CAnnotationEdge>::iterator it;
		for ( it = m_list.begin(); it != m_list.end(); it++ )
		{
			glVertex3fv(it->first.toVector());
			glVertex3fv(it->second.toVector());
			//CVertex( it->first  ).render();
			//CVertex( it->second ).render();
			//glVertex3fv( it->first.toVector() );
			//glVertex3fv( it->second.toVector() );
		}
		glEnd();

		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
