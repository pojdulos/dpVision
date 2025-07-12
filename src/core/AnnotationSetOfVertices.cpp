#include "Global.h"
#include "AnnotationSetOfVertices.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>


#include "../renderers/IAnnotationSetOfVerticesRenderer.h"

CAnnotationSetOfVertices::CAnnotationSetOfVertices(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("set of vertices");
	m_cloud = nullptr;
	m_pointSize = 5;
	renderer_ = std::make_shared<IAnnotationSetOfVerticesRenderer>();
}

CAnnotationSetOfVertices::CAnnotationSetOfVertices(const CAnnotationSetOfVertices& a) :CAnnotation((CAnnotation&)a), SetOfVIndices(a)
{
	setLabel("copy_of_" + a.m_label);
	//m_list.clear();
	m_cloud = a.m_cloud;
	m_pointSize = 5;
	renderer_ = std::make_shared<IAnnotationSetOfVerticesRenderer>();
}


std::wstring CAnnotationSetOfVertices::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)SetOfVIndices::size()) + L" points.";

	return info;
}


QString CAnnotationSetOfVertices::toString(QString prefix, QString suffix, QString separator)
{
	QString text(prefix);

	SetOfVIndices::iterator it = this->begin();

	if (it != this->end())
	{
		text.append(QString::number(*it));

		while (++it != this->end())
		{
			text.append(separator);
			text.append(QString::number(*it));
		}
	}

	text.append(suffix);
	return text;
}


void CAnnotationSetOfVertices::getDest()
{
	if (m_cloud == nullptr)
	{
		if (m_parent == nullptr) return;
		if (m_parent->category() != CBaseObject::Category::OBJECT) return;

		if (m_parent->hasType(CBaseObject::Type::CLOUD) || m_parent->hasType(CBaseObject::Type::ORDEREDCLOUD) || m_parent->hasType(CBaseObject::Type::MESH))
			m_cloud = std::static_pointer_cast<CPointCloud>(this->getParentPtr());
		else
		{
			for (const CObject::Children::value_type &c : ((CObject*)m_parent)->children())
			{
				std::shared_ptr<CBaseObject> child = c.second;
				if ((child != nullptr) &&
					 child->hasType(CBaseObject::Type::CLOUD) || child->hasType(CBaseObject::Type::ORDEREDCLOUD) || child->hasType(CBaseObject::Type::MESH))
				{
					m_cloud = std::static_pointer_cast<CPointCloud>(child);
					return;
				}
			}
		}

	}
}




void CAnnotationSetOfVertices::renderSelf()
{
	getDest();

	if ( SetOfVIndices::size() > 0 )
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		glPointSize(m_pointSize);

		glColor4ubv(m_selected ? m_selcolor.V() : m_color.V());

		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);

		for (auto idx : *this)
		{
			glVertex3fv(m_cloud->vertices()[idx].toVector());
		}

		glEnd();

		glDisable(GL_COLOR_MATERIAL);

		glPopAttrib();
		glPopMatrix();
	}
}