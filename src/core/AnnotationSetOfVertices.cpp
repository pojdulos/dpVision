#include "Global.h"
#include "AnnotationSetOfVertices.h"

#include "../api/AP.h"

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

#include "OrderedPointCloud.h"

void CAnnotationSetOfVertices::getDest()
{
	if (m_cloud) return;

	if (auto parent_mesh = std::dynamic_pointer_cast<CMesh>(m_parent.lock())) { m_cloud = parent_mesh; return; }
	else if (auto parent_mesh = std::dynamic_pointer_cast<CPointCloud>(m_parent.lock())) { m_cloud = parent_mesh; return; }
	else if (auto parent_mesh = std::dynamic_pointer_cast<COrderedPointCloud>(m_parent.lock())) { m_cloud = parent_mesh; return; }

	if (auto parent_obj = std::dynamic_pointer_cast<CObject>(m_parent.lock())) {
		for (auto c : parent_obj->children())
		{
			if (auto first_child_mesh = std::dynamic_pointer_cast<CMesh>(c.second)) { m_cloud = first_child_mesh; return; }
			else if (auto first_child_mesh = std::dynamic_pointer_cast<CPointCloud>(c.second)) { m_cloud = first_child_mesh; return; }
			else if (auto first_child_mesh = std::dynamic_pointer_cast<COrderedPointCloud>(c.second)) { m_cloud = first_child_mesh; return; }
		}
	}
}


