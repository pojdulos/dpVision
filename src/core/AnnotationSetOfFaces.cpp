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
	if (m_mesh) return m_mesh;
	
	if (auto parent_mesh = std::dynamic_pointer_cast<CMesh>(m_parent.lock())) return m_mesh = parent_mesh;

	if (auto parent_obj = std::dynamic_pointer_cast<CObject>(m_parent.lock())) {
		for (auto c : parent_obj->children())
		{
			if (auto first_child_mesh = std::dynamic_pointer_cast<CMesh>(c.second)) return m_mesh = first_child_mesh;
		}
	}
	
	return nullptr;
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


