#include "StdAfx.h"

#include "LidarData.h"

//using namespace std;

//#include "gltools.h"
#include <QtOpenGL>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGLExtensions/QtOpenGLExtensions>
#include <gl/glu.h>

CLidarData::CLidarData():CObject(),m_lidarData(NULL)
{
	m_type = 0;
	m_alpha = 255;
	numCol = 0;
	numRow = 0;
	renderMode = 0;
}

CLidarData::CLidarData(CBaseObject *p):CObject(p), m_lidarData(NULL)
{
	m_type = 0;
	m_alpha = 255;
	numCol = 0;
	numRow = 0;
	renderMode = 0;
}


CLidarData::~CLidarData(void)
{
	if ( NULL != m_lidarData ) {
		for ( size_t i=0; i<numCol; i++ )
		{
			delete[] m_lidarData[i];
		}
		delete[] m_lidarData;
		numCol = 0;
		numRow = 0;
	}
}


void CLidarData::render()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glEnable ( GL_COLOR_MATERIAL );
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//GLfloat kol[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, kol );// FrontColor.specular.w );
		//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, kol );// FrontColor.emission.w );
		//glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0f );// FrontColor.shininess);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glShadeModel(GL_SMOOTH);



		glBegin(GL_POINTS);
		for (size_t c = 0; c<numCol; c++ )
		{
			if ( c % 5 ) continue;
			CLidarData::PointPtr column = getColumn( c );
			for (size_t r = 0; r<numRow; r++ )
			{
				if ( r % 5 ) continue;

				glColor4ubv( column[r].color.V() );
				
				if ( renderMode == 0 )
					glVertex3f( column[r].x, column[r].y, column[r].z );
				else
					glVertex3f( (float)c/1000.0f, (float)r/1000.0f, 0.0f );
			}
		}
		glEnd();

		GLenum errCode;
		const GLubyte *errString;

		if ((errCode = glGetError()) != GL_NO_ERROR)
		{
			errString = gluErrorString(errCode);
			std::cout << "OpenGL Error: " << errString << std::endl;
		}

	glPopAttrib();
}
