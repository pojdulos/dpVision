#include "Global.h"
#include "AnnotationVPath.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


#include "../renderers/IAnnotationVPathRenderer.h"

CAnnotationVPath::CAnnotationVPath(std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices()
{
	setLabel("v_path");
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(const CAnnotationVPath& p) : CAnnotation(p), ListOfVIndices(p)
{
	setLabel("v_path");
	//m_list = p.m_list;
	m_length = p.m_length;
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(ListOfVIndices& list, double d, std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices(list)
{
	setLabel("v_path");
	//m_list = list;
	m_length = d;
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(ListOfVIndices& list, std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices(list)
{
	setLabel("v_path");
	if (auto o = std::dynamic_pointer_cast<CObject>(m))
	{
		if (auto mesh = std::dynamic_pointer_cast<CMesh>(o->getChild()))
		{
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
	}
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}




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
		auto mesh = std::dynamic_pointer_cast<CMesh>(((CObject*)m_parent)->getChild());

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
