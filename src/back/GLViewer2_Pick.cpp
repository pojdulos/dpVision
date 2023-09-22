#include "AP.h"
#include "GLViewer.h"
#include "childwindow.h"


//template<typename _W> bool GLViewer::convertWorldToWin(Trio<_W> worldCoords, Trio<_W> &winCoords)
bool GLViewer::convertWorldToWin( Trio<double> worldCoords, Trio<double> &winCoords)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	//float depth;
	//glReadPixels( winX, (double)viewport[3]-winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );

	double winX, winY, winZ;
	int res1 = gluProject(worldCoords.X(), worldCoords.Y(), worldCoords.Z(), modelview, projection, viewport, &winX, &winY, &winZ);

	winCoords.Set( winX, winY, winZ );

	return (GL_TRUE == res1);
}


//template<typename _W> bool GLViewer::convertWinToWorld(Trio<_W> winCoords, Trio<_W> &worldCoords)
bool GLViewer::convertWinToWorld( Trio<double> winCoords, Trio<double> &worldCoords)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double worldX, worldY, worldZ;
	int res1 = gluUnProject(winCoords.X(), (double)viewport[3] - winCoords.Y(), winCoords.Z(), modelview, projection, viewport, &worldX, &worldY, &worldZ);

	worldCoords.Set( worldX, worldY, worldZ );

	return (GL_TRUE == res1);
}

bool GLViewer::convertCoords(GLdouble winX, GLdouble winY, CPunkt3D &pkt0, CPunkt3D &pkt1 )
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	double worldX, worldY, worldZ;
	int res1 = gluUnProject( winX, (double)viewport[3]-winY, 0.0, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	pkt0.Set( worldX, worldY, worldZ );

	int res2 = gluUnProject( winX, (double)viewport[3]-winY, 1.0, modelview, projection, viewport, &worldX, &worldY, &worldZ);
	
	pkt1.Set( worldX, worldY, worldZ );

	return ( ( GL_TRUE == res1 ) && ( GL_TRUE == res2 ) );
}


void GLViewer::PickPoint(int x, int y)
{
	double wx=0.0, wy=0.0, wz=0.0;
	double wx0=0.0, wy0=0.0, wz0=0.0;

	CPunkt3D pkt0, pkt1;

	int sel = AP::WORKSPACE::select( readSelectBuffer( x, y ) );

	UI::updateSelection(sel);

	if ( -1 != sel )
	{
		CModel3D *obj = AP::WORKSPACE::getSelected();

		if (obj->getFormat() != CSimpleData::MESH)
		{
			UI::MESSAGEBOX::information(L"Operation not supported", L"I'm sorry. In the current version of the software, this feature is only available for meshes.");
		}
		else if ( convertCoords( x, y, /*ref*/pkt0, /*ref*/pkt1 ) )
		{
			// najbli¿szy punkt od ekranu
			pkt0 = obj->getTransform().world2local( pkt0 );

			// najdalszy punkt od ekranu
			pkt1 = obj->getTransform().world2local( pkt1 );
			
			CWektor3D vRay( pkt0, pkt1 );
			vRay.normalize();
			
			CPunkt3D IntersectionPoint;
			size_t faceIdx;

			CMesh *mesh = (CMesh*) obj->getData();
			
			ULONGLONG t0=GetTickCount64();
			if ( mesh->getClosestFace( pkt0, vRay, /*ref*/IntersectionPoint, /*ref*/faceIdx ) )
			{
				CAnnotationPoint pt( IntersectionPoint );
				
				pt.setRay( vRay );
				
				pt.setFace(
					faceIdx,
					mesh->vertices()[ mesh->faces()[faceIdx].A() ],
					mesh->vertices()[ mesh->faces()[faceIdx].B() ],
					mesh->vertices()[ mesh->faces()[faceIdx].C() ] );

				for ( std::map<unsigned int,Plugin*>::iterator pit = AP::getApp()->plugins.begin(); pit != AP::getApp()->plugins.end(); pit++ )
				{
					pit->second->pickSlot( obj->Id(), pt );
				}

				if ( 0 == AP::getApp()->pickBlocker )
				{
					unsigned int ptId = AP::addAnnotationPoint( obj->Id(), pt );
				}

				ULONGLONG t1=GetTickCount64();
				UI::STATUSBAR::printf( "Selected point [%lf,%lf,%lf] on object %d (time:%ld)", IntersectionPoint.X(), IntersectionPoint.Y(), IntersectionPoint.Z(), sel, t1-t0 );
				
			}
			else
			{
				ULONGLONG t1=GetTickCount64();
				UI::STATUSBAR::printf( "Object clicked (%d) but intersection not found !!! (time:%ld)", sel, t1-t0 );
			}
		}
		else
		{
			UI::STATUSBAR::printf( "Something wrong in PickPoint() procedure. Object clicked (%d)", sel );
		}
	}
	else
	{
		UI::STATUSBAR::printf( "No objects selected (%d)", sel );
	}

//	UI::getMainWindow().updateListViewSelection( sel );
}


void GLViewer::PickObject(int x, int y)
{
	int id = AP::WORKSPACE::select( readSelectBuffer( x, y ) );

	if ( -1 != id )
	{
		UI::STATUSBAR::printf( L"Selected (%d) %s", id, AP::WORKSPACE::getSelected()->getName().c_str() );
	}
	else
	{
		UI::STATUSBAR::printf( L"None selected (%d)", id );
	}

	UI::updateSelection(id);
}


int GLViewer::readSelectBuffer(int x, int y)
{
    const int BUFFER_LENGTH = 64;
    GLuint select_buffer[ BUFFER_LENGTH ];
	memset( select_buffer, 0, 64 );

	glSelectBuffer( BUFFER_LENGTH, select_buffer );

    int viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    int width = viewport [2];
    int height = viewport [3];

    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();

	glLoadIdentity ();
	gluPickMatrix( x, height-y, 1, 1, viewport );

	glRenderMode( GL_SELECT );

	// generowanie sceny 3D
	Render();

	glFlush();

	GLint hits = glRenderMode( GL_RENDER );

	glMatrixMode( GL_PROJECTION );
    glPopMatrix ();

	int sel = -1;
	unsigned int selmin = UINT_MAX;
	int indeks = 0;

	for ( int i = 0; i < hits; i++ )
	{
		if ( select_buffer[ indeks ] > 0 )
		{
			for ( int n = 0; n < select_buffer[ indeks ]; n++ )
			{
				if ( select_buffer[ indeks + 3 + n ] >= MODEL_ID_OFFSET )
				{
					if ( select_buffer[ indeks + 1 ] < selmin )
					{
						selmin = select_buffer[ indeks + 1 ];
						sel = select_buffer[ indeks + 3 + n ];
					}
				}
			}
		}
		indeks += select_buffer[ indeks ]+3;
	}

	return sel;
}

