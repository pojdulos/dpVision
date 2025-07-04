#include "Global.h"
#include "AnnotationPoint.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "MainApplication.h"

//void CAnnotationPoint::applyTransform(CTransform prevT, CTransform newT)
//{
//	CPoint3d p0( *this );
//	CPoint3d p1( p0 + m_direction );
//	
//	p0 = newT.w2l(prevT.l2w(p0));
//	this->Set(p0);
//
//	if (m_direction.length())
//	{
//		p1 = newT.w2l(prevT.l2w(p1));
//
//		m_direction = CVector3d(p0, p1).getNormalized();
//	}
//}

void CAnnotationPoint::applyTransformation(CTransform& from, CTransform& to)
{
	CPoint3d p0 = to.w2l(from.l2w(*this));
	CPoint3d p1 = to.w2l(from.l2w(*this + m_direction));

	*this = p0;
	m_direction = p1 - p0;
	m_direction.normalize();
}

std::wstring CAnnotationPoint::getInfoRow()
{
	std::wstring info = L"Coordinates: [ X=" + std::to_wstring((long double)x) + L", Y=" + std::to_wstring((long double)y) + L", Z=" + std::to_wstring((long double)z) + L" ]";
	info.append( L", vector: [ vX=" + std::to_wstring((long double)m_direction.X()) + L", vY=" + std::to_wstring((long double)m_direction.Y()) + L", vZ=" + std::to_wstring((long double)m_direction.Z()) + L" ]" );

	info.append( L", selected face: Id(" + std::to_wstring((long long)m_faceId) + L"): " );
	info.append( L"[ " + std::to_wstring((long double)m_faceA.X()) + L", " + std::to_wstring((long double)m_faceA.Y()) + L", " + std::to_wstring((long double)m_faceA.Z()) + L" ] - " );
	info.append( L"[ " + std::to_wstring((long double)m_faceB.X()) + L", " + std::to_wstring((long double)m_faceB.Y()) + L", " + std::to_wstring((long double)m_faceB.Z()) + L" ] - " );
	info.append( L"[ " + std::to_wstring((long double)m_faceC.X()) + L", " + std::to_wstring((long double)m_faceC.Y()) + L", " + std::to_wstring((long double)m_faceC.Z()) + L" ]" );

	return info;
}

void CAnnotationPoint::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	


	if ( m_faceId > -1 )
	{
		if ( m_selected )
		{
			glColor4f( 0.7f, 0.0f, 0.0f, 1.0f );
		}
		else
		{
			glColor4f( 0.0f, 0.0f, 0.7f, 1.0f );
		}

		glLineWidth(5);
	
		glBegin(GL_TRIANGLES);
		glVertex3f( m_faceA.X(), m_faceA.Y(), m_faceA.Z());
		glVertex3f( m_faceB.X(), m_faceB.Y(), m_faceB.Z());
		glVertex3f( m_faceC.X(), m_faceC.Y(), m_faceC.Z());
		glEnd();
	}



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

	glTranslated( x, y, z );

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

	if (m_showNorm && (0.0 != m_direction.length()))
	{
		//glColor3f(1.0, 1.0, 0.0);
		glColor3f(0.0, 0.5, 0.0);

		glLineWidth(3.0f);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(10 * this->m_direction.X(), 10 * this->m_direction.Y(), 10 * this->m_direction.Z());
		glEnd();

		CVector3d vn = m_direction.getNormalized();

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
