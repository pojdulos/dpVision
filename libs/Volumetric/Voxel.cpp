#include "StdAfx.h"

#include "Voxel.h"

CVoxel::CVoxel()
{
	//m_pos = v;
	m_size.Set(1.0f, 1.0f, 1.0f);
	m_color.Set(128,128,128,255);
	m_type = 0;
}

CVoxel::CVoxel( CTriple<float> v, CRGBA c )
{
	//m_pos = v;
	m_size.Set(1.0f, 1.0f, 1.0f);
	m_color = c;
	m_type = 0;
}

#include <Windows.h>
#include <GL\GL.h>
/*
void CVoxel::render()
{
	glColor4f( m_color.fR(), m_color.fG(), m_color.fB(), m_color.fA() );
	glVertex3f( m_pos.X(), m_pos.Y(), m_pos.Z() );
}
*/

void CVoxel::render(CTriple<float> pos)
{
	glColor4f(m_color.fR(), m_color.fG(), m_color.fB(), m_color.fA());
	//glVertex3f(pos.X(), pos.Y(), pos.Z());
	drawVoxel(pos.X(), pos.Y(), pos.Z());
}



void CVoxel::drawVoxel(float ix, float iy, float iz)
{
	float left = (float)ix - m_size.X()/2;
	float right = (float)ix + m_size.X()/2;
	float bottom = (float)iy - m_size.Y()/2;
	float top = (float)iy + m_size.Y()/2;
	float back = (float)iz + m_size.Z()/2;
	float front = (float)iz - m_size.Z()/2;


//		if (m_type == 0)
//			glColor4ub(v->c, v->c, v->c, m_alpha);
//		else if (v->c < 126)
//			glColor4ub(0, 255 - v->c, 0, m_alpha);
//		else if (v->c > 128)
//			glColor4ub(v->c, 0, 0, m_alpha);

//		if (v->n.bitLeftX) //LEWO
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(left, iy, iz));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(left, bottom, front);
			glVertex3f(left, bottom, back);
			glVertex3f(left, top, back);
			glVertex3f(left, top, front);
		}

//		if (v->n.bitRightX) //PRAWO
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(right, iy, iz));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(right, top, back);
			glVertex3f(right, bottom, back);
			glVertex3f(right, bottom, front);
			glVertex3f(right, top, front);
		}


//		if (v->n.bitLeftY) //DÓ£
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(ix, bottom, iz));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(left, bottom, front);
			glVertex3f(right, bottom, front);
			glVertex3f(right, bottom, back);
			glVertex3f(left, bottom, back);
		}

//		if (v->n.bitRightY) //GÓRA
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(ix, top, iz));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(right, top, back);
			glVertex3f(right, top, front);
			glVertex3f(left, top, front);
			glVertex3f(left, top, back);
		}


//		if (v->n.bitLeftZ) //PRZOD
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(ix, iy, front));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(left, bottom, front);
			glVertex3f(left, top, front);
			glVertex3f(right, top, front);
			glVertex3f(right, bottom, front);
		}

//		if (v->n.bitRightZ) //TY£
		{
			CVector3f vn(CTriple<float>(ix, iy, iz), CTriple<float>(ix, iy, back));
			vn.normalize();
			glNormal3f(vn.X(), vn.Y(), vn.Z());

			glVertex3f(right, top, back);
			glVertex3f(left, top, back);
			glVertex3f(left, bottom, back);
			glVertex3f(right, bottom, back);
		}
}


CVoxels::CVoxels(CBaseObject *p ):CObject(p)
{
}


void CVoxels::render()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	
	//glColorMaterial(GL_FRONT_AND_BACK);// , GL_AMBIENT_AND_DIFFUSE);

	float buf[5];
	CRGBA emission, specular, ambient, diffuse;
	emission.Set(_DEF_EMISSION);
	specular.Set(_DEF_SPECULAR);
	ambient.Set(_DEF_AMBIENT);
	diffuse.Set(_DEF_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient.fV(buf));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse.fV(buf));
	glMaterialfv(GL_FRONT, GL_EMISSION, emission.fV(buf));
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular.fV(buf));
	glMaterialf(GL_FRONT, GL_SHININESS, _DEF_SHININESS );

	glEnable(GL_COLOR_MATERIAL);

	//glPointSize(5);

	//glBegin(GL_POINTS);
	
	/**/	//glEnable(GL_CULL_FACE);
	/**/	//glCullFace(GL_BACK);
	/**/	//glFrontFace(GL_CCW);
	/**/	glBegin(GL_QUADS);

	CVoxels::DataType::iterator vit;
	for ( vit = m_voxels.begin(); vit != m_voxels.end(); vit++ )
	{
		(*vit).second->render( (*vit).first );
	}
	
	glEnd();

	/**/	glDisable(GL_CULL_FACE);

	glPopAttrib();
	glPopMatrix();
}

void CVoxels::clear()
{
	CVoxels::DataType::iterator vit;
	for ( vit = m_voxels.begin(); vit != m_voxels.end(); vit++ )
	{
		delete (*vit).second;
	}
	m_voxels.clear();
}