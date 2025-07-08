#include "Global.h"
#include "AnnotationVPath.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>

CAnnotationVPath::CAnnotationVPath(ListOfVIndices& list, CModel3D* m) :CAnnotation(m), ListOfVIndices(list)
{
	setLabel("v_path");

	if ( ((CObject*)m->getChild(0))->type() == CModel3D::MESH )
	{
		CMesh *mesh = (CMesh*)m->getChild();

		//m_list = list;

		m_length = 0.0;
		ListOfVIndices::iterator it = begin();
		size_t v1 = *(it++);

		while (it != end())
		{
			INDEX_TYPE v2 = *(it++);
			m_length += CVector3<double>(mesh->vertices()[v1], mesh->vertices()[v2]).length();
			v1 = v2;
		}
	}
};




std::wstring CAnnotationVPath::getInfoRow()
{
	std::wstring info = L"Length: " + std::to_wstring(m_length) + L" Size: " + std::to_wstring((long long) size()) + L" points.";

	return info;
}

void CAnnotationVPath::renderSelf()
{
	if (m_parent == nullptr) return;
	
	if (m_parent->category() != CBaseObject::Category::OBJECT) return;

	if (this->size() > 1)
	{
		CMesh *mesh = (CMesh*)((CObject*)m_parent.get())->getChild();

		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (m_selected)
		{
			glLineWidth(5.0);
			glColor4ubv( m_selcolor.V() );
		}
		else
		{
			glLineWidth(3.0);
			glColor4ubv( m_color.V() );
		}

		glBegin(GL_LINES);

		ListOfVIndices::iterator ip = this->begin();

		CVertex v1 = mesh->vertices()[*ip];

		while (++ip != this->end())
		{
			CVertex v2 = mesh->vertices()[*ip];

			glVertex3fv( v1.toVector() );
			glVertex3fv( v2.toVector() );

			v1 = v2;
		}

		glEnd();
		
		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}


ListOfVIndices::iterator CAnnotationVPath::cyclicNext(ListOfVIndices::iterator current)
{
	ListOfVIndices::iterator next = current;

	next++;
	if (next == end()) next = begin();

	return next;
}

ListOfVIndices::iterator CAnnotationVPath::cyclicPrev(ListOfVIndices::iterator current)
{
	ListOfVIndices::iterator prev = current;

	if (prev == begin()) prev = end();
	prev--;

	return prev;
}
