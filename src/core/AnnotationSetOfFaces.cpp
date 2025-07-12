#include "Global.h"
#include "AnnotationSetOfFaces.h"

#include "AP.h"

#include "../renderers/IAnnotationSetOfFacesRenderer.h"

CAnnotationSetOfFaces::CAnnotationSetOfFaces(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("f-indices set");
	m_mesh = nullptr;
	renderer_ = std::make_shared<IAnnotationSetOfFacesRenderer>();
}

CAnnotationSetOfFaces::CAnnotationSetOfFaces(const CAnnotationSetOfFaces& p) :CAnnotation(p), SetOfFIndices(p)
{
	m_mesh = p.m_mesh;
	renderer_ = std::make_shared<IAnnotationSetOfFacesRenderer>();
}

CAnnotationSetOfFaces::CAnnotationSetOfFaces(const SetOfFIndices& list) : CAnnotation(), SetOfFIndices(list)
{
	setLabel("f-indices set");
	m_mesh = nullptr;
	renderer_ = std::make_shared<IAnnotationSetOfFacesRenderer>();
}

std::shared_ptr<CMesh> CAnnotationSetOfFaces::getDest()
{
	if (m_mesh == nullptr)
	{
		if (m_parent == nullptr) return nullptr;
		if (m_parent->category() != CBaseObject::Category::OBJECT) return nullptr;

		if (m_parent->hasType(CBaseObject::Type::MESH))
			m_mesh = std::dynamic_pointer_cast<CMesh>(this->getParentPtr());
		else
		{
			for (auto c : ((CObject*)m_parent)->children())
			{
				std::shared_ptr<CBaseObject> child = c.second;
				if ((child != nullptr) &&
					 child->hasType(CBaseObject::Type::MESH))
				{
					m_mesh = std::dynamic_pointer_cast<CMesh>(child);
				}
			}
		}

	}
	return m_mesh;
}

std::shared_ptr<CMesh> CAnnotationSetOfFaces::toMesh()
{
	if (m_mesh != nullptr)
	{
		std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();

		mesh->vertices() = m_mesh->vertices();

		for (INDEX_TYPE fId : *this)
		{
			mesh->addFace(m_mesh->faces()[fId]);
		}

		mesh->removeUnusedVertices();

		return mesh;
	}
	else
	{
		return std::make_shared<CMesh>();
	}
}

std::wstring CAnnotationSetOfFaces::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) this->size()) + L" faces.";

	return info;
}

QString CAnnotationSetOfFaces::toString(QString prefix, QString suffix, QString separator)
{
	QString text(prefix);

	SetOfFIndices::iterator it = this->begin();

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


void CAnnotationSetOfFaces::renderSelf()
{
	if (m_mesh == nullptr)
	{
		if (m_parent == nullptr) return;
		if (m_parent->category() != CBaseObject::Category::OBJECT) return;

		if ( m_parent->hasType(CBaseObject::Type::MESH) )
			m_mesh = std::dynamic_pointer_cast<CMesh>(this->getParentPtr());
		else
		{
			std::shared_ptr<CBaseObject> child = std::shared_ptr<CBaseObject>(((CObject*)m_parent)->getChild());
			if (child == nullptr) return;
			else if (child->hasType(CBaseObject::Type::MESH))
			{
				m_mesh = std::dynamic_pointer_cast<CMesh>(child);
			}
			else
			{
				return;
			}
		}

	}

	if (this->size() > 1)
	{
		//CMesh *mesh = (CMesh*) ((CObject*)m_parent)->getChild();

		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
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


		glBegin(GL_TRIANGLES);

		SetOfFIndices::iterator it;
		for ( it = this->begin(); it != this->end(); it++ )
		{
			//glNormal3fv( mesh->fnormals()[*it].toVector() );

			glVertex3fv( m_mesh->vertices()[m_mesh->faces()[*it].A()].toVector() );
			glVertex3fv( m_mesh->vertices()[m_mesh->faces()[*it].B()].toVector() );
			glVertex3fv( m_mesh->vertices()[m_mesh->faces()[*it].C()].toVector() );
		}
		glEnd();

		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
