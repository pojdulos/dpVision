#include "IAnnotationBezierRenderer.h"
#include <qopengl.h>
#include "AnnotationBezier.h"


void IAnnotationBezierRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBezier* obj = (CAnnotationBezier*)_obj;

	if (obj->m_list.size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(3.0);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (obj->isChecked())
		{
			glColor4ubv(obj->getSelColor().V());
		}
		else
		{
			glColor4ubv(obj->getColor().V());
		}

		if ( obj->m_type == 1 ) drawTinySpline(obj);
		else drawBeziers(obj, 8, 10, 34);

		glPointSize(10.0);
		glBegin(GL_POINTS);
		//for (int i = 0; i < m_list.size(); i++) glVertex3fv(&m_list[i].toVector()[0]);
		glColor4ubv( CRGBA( 0.0f, 1.0f, 0.0f, 1.0f ).V() );
		glVertex3dv( obj->m_spline[0].toVector() );
		glColor4ubv(CRGBA(1.0f, 0.0f, 0.0f, 1.0f).V());
		glVertex3dv( obj->m_spline[obj->m_spline.size()-1].toVector() );

		glEnd();



		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}


void IAnnotationBezierRenderer::drawTinySpline(CAnnotationBezier* obj)
{
	glLineWidth(2.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < obj->m_spline.size(); i++)
	{
		glVertex3dv( obj->m_spline[i].toVector() );
	}
	glEnd();

	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < obj->m_spline.size(); i++)
	{
		glVertex3dv(obj->m_spline[i].toVector());
	}
	glEnd();
}


void IAnnotationBezierRenderer::drawBeziers(CAnnotationBezier* obj, int deg, int den, int cut )
{
	int div = deg * den;

	for (int idx = 0; idx < obj->m_list.size(); idx ++)
	{
		glBegin(GL_LINE_STRIP);

		for (int i = cut; i <= div - cut; i++)
		{
			CPoint3d p = obj->deCastelJau((GLdouble)i / div, deg, idx);
			glVertex3dv(p.toVector());
		}

		glEnd();
	}
}
