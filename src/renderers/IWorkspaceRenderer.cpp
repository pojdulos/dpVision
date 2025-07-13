#include "IWorkspaceRenderer.h"
#include "Workspace.h"

#include <QtOpenGL>

void IWorkspaceRenderer::render(const CWorkspace* _obj)
{
	glPushMatrix();
    renderKids(_obj);
	glPopMatrix();
}

void IWorkspaceRenderer::renderKids(const CWorkspace* _obj)
{
    CWorkspace* obj = (CWorkspace*)_obj;

    for (auto &model : obj->children() )
		if (model.second) model.second->render();
}

void IWorkspaceRenderer::renderLights(const CWorkspace* _obj, bool perm)
{
    CWorkspace* obj = (CWorkspace*)_obj;

    //	GLUquadric* q = gluNewQuadric();
	for (int iL=0; iL<8; iL++)
	{
        auto &light = obj->getLightRef(iL);
		
        if ( perm != light.fixed )
			continue;
		else if ( light.active )
		{
			glDisable( light.light );
			glLightfv( light.light, GL_AMBIENT, light.ambient );
			glLightfv( light.light, GL_DIFFUSE, light.diffuse );
			glLightfv( light.light, GL_SPECULAR, light.specular );
			glLightfv( light.light, GL_POSITION, light.position );
			CVector3f w( light.spot_direction[0], light.spot_direction[1], light.spot_direction[2] );
			w.normalize();
			float tmp[3];
			tmp[0] = w.X();
			tmp[1] = w.Y();
			tmp[2] = w.Z();
			glLightfv( light.light, GL_SPOT_DIRECTION, tmp );
			glLightf( light.light, GL_SPOT_CUTOFF, light.spot_cut_off );
			glEnable( light.light );
/*
			if ( 0.0 != m_lights[iL].position[3] ) // jesli 0.0 to swiatlo kierunkowe z nieskonczonosci
			{
				glPushMatrix();

				glMaterialfv( GL_FRONT, GL_SPECULAR, m_lights[iL].specular );
				glMaterialfv( GL_FRONT, GL_AMBIENT, m_lights[iL].ambient );
				glMaterialfv( GL_FRONT, GL_DIFFUSE, m_lights[iL].diffuse );
				glMateriali(GL_FRONT, GL_SHININESS,255);

				glTranslatef( m_lights[iL].position[0], m_lights[iL].position[1], m_lights[iL].position[2] ); 
				gluSphere( q, 1.0, 32, 32 );

				glPopMatrix();
			}
*/
		}
		else
		{
			glDisable( light.light );
		}

	}
//	gluDeleteQuadric( q );
}



/*
void CWorkspace::RysujOsie()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	float g0 = 0.5f, g1 = 1.0f, a=1.0f;
	int s0 = 32;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	//glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
	//glColorMaterial(GL_FRONT_AND_BACK,GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( this->m_idOfSelected < 0 ) glColor4f(0.8f, 0.2f, 0.2f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);

//	GLUquadric* q = gluNewQuadric();
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 0, 50 );
	glEnd();
//	glPushMatrix();
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

	if ( this->m_idOfSelected < 0 ) glColor4f(0.2f, 0.8f, 0.2f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 55, 0 );
	glEnd();
//	glPushMatrix();
//		glRotatef( 270.0, 1.0, 0.0, 0.0 ); 
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

	if ( this->m_idOfSelected < 0 ) glColor4f(0.2f, 0.2f, 0.8f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 55, 0, 0 );
	glEnd();
//	glPushMatrix();
//		glRotatef( 90.0, 0.0, 1.0, 0.0 ); 
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

//	gluDeleteQuadric( q );

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}
*/
