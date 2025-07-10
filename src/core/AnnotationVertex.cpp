#include "Global.h"
#include "AnnotationVertex.h"
#include "Mesh.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>

std::wstring CAnnotationVertex::getInfoRow()
{
	std::wstring info = L"Vertex index: " + std::to_wstring(m_index);

	return info;
}

CVertex *CAnnotationVertex::getVertex()
{
	if (m_parent == nullptr) return nullptr;

	if (m_parent->category() != CBaseObject::Category::OBJECT) return nullptr;

	std::shared_ptr<CMesh> mesh = std::dynamic_pointer_cast<CMesh>(((CObject*)m_parent)->getChild());

	if (mesh->vertices().size() <= m_index) return nullptr;

	return &mesh->vertices()[m_index];
};


void CAnnotationVertex::renderSelf()
{
	if (m_cloud == nullptr)
	{
		if (m_parent == nullptr) return;
		if (m_parent->category() != CBaseObject::Category::OBJECT) return;

		if (m_parent->hasType(CBaseObject::Type::CLOUD) || m_parent->hasType(CBaseObject::Type::ORDEREDCLOUD) || m_parent->hasType(CBaseObject::Type::MESH))
			m_cloud = std::dynamic_pointer_cast<CPointCloud>(this->getParentPtr());
		else
		{
			std::shared_ptr<CBaseObject> child = ((CObject*)m_parent)->getChild();
			if (child == nullptr) return;
			else if (child->hasType(CBaseObject::Type::CLOUD) || child->hasType(CBaseObject::Type::ORDEREDCLOUD) || child->hasType(CBaseObject::Type::MESH))
			{
				m_cloud = std::dynamic_pointer_cast<CPointCloud>(child);
			}
			else
			{
				return;
			}
		}

	}

	if (m_cloud->vertices().size() <= m_index) return;

	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( m_selected )
	{
		glPointSize(9);
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glPointSize(9);
		glColor4ubv(m_color.V());
	}

	glTranslatef( 0.0, 0.0, 0.0 );

	glEnable( GL_POINT_SMOOTH );
	glBegin(GL_POINTS);
		glVertex3fv(m_cloud->vertices()[m_index].toVector());
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
