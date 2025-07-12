#include "Global.h"
#include "AnnotationPolygon2D.h"

#include "AP.h"
#include "Model3D.h"
#include "Mesh.h"

//#include <Windows.h>
#include <GL/gl.h>

#include "../renderers/IAnnotationPolygon2DRenderer.h"

CAnnotationPolygon2D::CAnnotationPolygon2D(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("polygon2d");
	renderer_ = std::make_shared<IAnnotationPolygon2DRenderer>();
}

CAnnotationPolygon2D::CAnnotationPolygon2D(CAnnotationPolygon2D& p) : CAnnotation(p)
{
	setLabel("polygon2d");
	m_data = p.m_data;
	renderer_ = std::make_shared<IAnnotationPolygon2DRenderer>();
}

std::wstring CAnnotationPolygon2D::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)m_data.size());

	return info;
}


void CAnnotationPolygon2D::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	


	if ( m_selected )
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	//glViewport(0, 0, 1920, 1080);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0, 1920, 1080, 20, -1, 1);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glLineWidth(5);

	CModel3D* obj = (CModel3D*)m_parent;
	CMesh* mesh = (CMesh*) obj->getChild().get();

	glBegin(GL_POLYGON);
	for (auto pt2d : m_data )
	{

		CPoint3d MousePt(pt2d.first, pt2d.second, 5);
		CPoint3d Pt;

		UI::CAMERA::convertWinToWorld(MousePt, Pt);
		Pt = obj->getTransform().world2local( Pt );

		glVertex3dv( Pt.toVector() );
	}
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
