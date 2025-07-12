#include "IAnnotationPointRenderer.h"
#include <qopengl.h>
#include "AnnotationPoint.h"

void IAnnotationPointRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPoint* obj = (CAnnotationPoint*)_obj;

    glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	


	if ( obj->getFaceId() > -1 )
	{
		if ( obj->isSelected() )
		{
			glColor4f( 0.7f, 0.0f, 0.0f, 1.0f );
		}
		else
		{
			glColor4f( 0.0f, 0.0f, 0.7f, 1.0f );
		}

		glLineWidth(5);
	
		glBegin(GL_TRIANGLES);
		glVertex3f( obj->getFaceA().X(), obj->getFaceA().Y(), obj->getFaceA().Z());
		glVertex3f( obj->getFaceB().X(), obj->getFaceB().Y(), obj->getFaceB().Z());
		glVertex3f( obj->getFaceC().X(), obj->getFaceC().Y(), obj->getFaceC().Z());
		glEnd();
	}



	if ( obj->isSelected() )
	{
		glPointSize(9);
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glPointSize(9);
		glColor4ubv(obj->getColor().V());
	}

	glTranslated( obj->x, obj->y, obj->z );

	//GLUquadric* q = gluNewQuadric();
	//gluSphere( q, 1.0, 32, 32 );
	//gluDeleteQuadric( q );

	//glutSolidSphere( 1.0, 32, 32 );
	
	glEnable( GL_POINT_SMOOTH );
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();
	glDisable(GL_POINT_SMOOTH);

	float lineWidth[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);

	//qInfo() << lineWidth[0] << " - " << lineWidth[1] << endl;

	if (obj->m_showNorm && (0.0 != obj->getDirection().length()))
	{
		//glColor3f(1.0, 1.0, 0.0);
		glColor3f(0.0, 0.5, 0.0);

		glLineWidth(3.0f);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(10 * obj->getDirection().X(), 10 * obj->getDirection().Y(), 10 * obj->getDirection().Z());
		glEnd();

		CVector3d vn = obj->getDirection().getNormalized();

		//glLineWidth(3.0f);
		//
		//glColor3f(1.0, 0.0, 0.0);

		//glBegin(GL_LINES);
		//glVertex3f(0, 0, 0);
		////glVertex3f(10.0 * abs(vn.x), 0, 0);
		//glVertex3f(10.0, 0, 0);
		//glEnd();

		//glColor3f(0.0, 0.5, 0.0);

		//glBegin(GL_LINES);
		//glVertex3f(0, 0, 0);
		////glVertex3f(0, 10.0 * abs(vn.y), 0);
		//glVertex3f(0, 10.0, 0);
		//glEnd();

		//glColor3f(0.0, 0.0, 1.0);

		//glBegin(GL_LINES);
		//glVertex3f(0, 0, 0);
		////glVertex3f(0, 0, 10.0 *abs(vn.z));
		//glVertex3f(0, 0, 10.0);
		//glEnd();

		//glLineWidth(3.0f);
		//glBegin(GL_LINES);
		//glVertex3f(9.6 * this->m_direction.X(), 9.6 * this->m_direction.Y(), 9.6 * this->m_direction.Z());
		//glVertex3f(9.8 * this->m_direction.X(), 9.8 * this->m_direction.Y(), 9.8 * this->m_direction.Z());
		//glEnd();

		//glLineWidth(5.0f);
		//glBegin(GL_LINES);
		//glVertex3f(9.4 * this->m_direction.X(), 9.4 * this->m_direction.Y(), 9.4 * this->m_direction.Z());
		//glVertex3f(9.6 * this->m_direction.X(), 9.6 * this->m_direction.Y(), 9.6 * this->m_direction.Z());
		//glEnd();
	}
	
	//glDrawString(this->x, this->y, this->m_label.toStdString().c_str());

	glPopAttrib();
	glPopMatrix();
}
