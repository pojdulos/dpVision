#include "Global.h"
#include "AnnotationSetOfVertices.h"

#include "AP.h"

#include <Windows.h>
#include <GL/GL.h>

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
			m_cloud = (CPointCloud*)m_parent;
		else
		{
			for (const CObject::Children::value_type &c : ((CObject*)m_parent)->children())
			{
				CBaseObject* child = c.second;
				if ((child != nullptr) &&
					 child->hasType(CBaseObject::Type::CLOUD) || child->hasType(CBaseObject::Type::ORDEREDCLOUD) || child->hasType(CBaseObject::Type::MESH))
				{
					m_cloud = (CPointCloud*)child;
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