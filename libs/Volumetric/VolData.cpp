#include "stdafx.h"

#include <Windows.h>
#include <GL\GL.h>
#include "VolData.h"
#include "RGBA.h"
//#include "Model3D.h"

CVolData3D::CVolData3D(CBaseObject *p ):CObject(p)
{
	m_volX = m_volY = m_volZ = 0;
	m_type = 0;
}

CVolData3D::~CVolData3D(void){}

void CVolData3D::render()
{
//	unsigned int options = MESHRENDEROPTIONS_NONE;

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	float buf[5];

	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, CRGBA(0.0f,0.0f,0.0f,1.0f).fV(buf) );	
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );	

//	if ( options & MESHRENDEROPTIONS_RENDERFACESASPOINTS )
//	{
//		glPolygonMode(GL_FRONT, GL_POINT);
//		glPolygonMode(GL_BACK, GL_POINT);
//	}
//	else if ( options & MESHRENDEROPTIONS_RENDERFACESASEDGES )
//	{
//		glPolygonMode(GL_FRONT, GL_LINE);
//		glPolygonMode(GL_BACK, GL_LINE);
//	}
//	else
//	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
//	}

	//if ( m_type == 0 )
	//	volumRender1( mesh, options );
	//else
		volumRender();
	
	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}

/*
void CVolData3D::volumAddBox( CMesh &mesh, float ix, float iy, float iz, float w, CRGBA color )
{
	unsigned char cc = 64;

	int s = mesh.vertices().size();

	int A, B, C, D, E, F, G, H;

	A = B = C = D = E = F = G = H = -1;

//	SVolData *v = this->at(iz).at(iy).at(ix);
	SVolData *v = &m_data.at(CTriple<int>(ix,iy,iz));

	// czy istnieje choæ jedna œciana zawieraj¹ca wierzcho³ek A
	int tst = v->n.bitRightZ + v->n.bitLeftX + v->n.bitLeftY;
	if ( tst > 0 )  
	{
		A = s++;
		mesh.vertices().push_back( CVertex( ix-w, iy-w, iz+w ) );
	}
	
	// czy istnieje choæ jedna œciana zawieraj¹ca wierzcho³ek B
	tst = v->n.bitRightX + v->n.bitRightZ + v->n.bitLeftY;
	if ( tst > 0 )  
	{
		B = s++;
		mesh.vertices().push_back( CVertex( ix+w, iy-w, iz+w ) );
	}

	tst = v->n.bitRightX + v->n.bitRightY + v->n.bitRightZ;
	if ( tst > 0 )  
	{
		C = s++;
		mesh.vertices().push_back( CVertex( ix+w, iy+w, iz+w ) );
	}

	tst = v->n.bitRightY + v->n.bitRightZ + v->n.bitLeftX;
	if ( tst > 0 )  
	{
		D = s++;
		mesh.vertices().push_back( CVertex( ix-w, iy+w, iz+w ) );
	}

	tst = v->n.bitRightY + v->n.bitLeftX + v->n.bitLeftZ;
	if ( tst > 0 )  
	{
		E = s++;
		mesh.vertices().push_back( CVertex( ix-w, iy+w, iz-w ) );
	}

	tst = v->n.bitRightX + v->n.bitRightY + v->n.bitLeftZ;
	if ( tst > 0 )  
	{
		F = s++;
		mesh.vertices().push_back( CVertex( ix+w, iy+w, iz-w ) );
	}

	tst = v->n.bitRightX + v->n.bitLeftY + v->n.bitLeftZ;
	if ( tst > 0 )  
	{
		G = s++;
		mesh.vertices().push_back( CVertex( ix+w, iy-w, iz-w ) );
	}

	tst = v->n.bitLeftX + v->n.bitLeftY + v->n.bitLeftZ;
	if ( tst > 0 )  
	{
		H = s++;
		mesh.vertices().push_back( CVertex( ix-w, iy-w, iz-w ) );
	}

	//prawa (x+)
	if ( v->n.bitRightX )
	{
		myFaces.push_back( CFace4( B, G, F, C ) ); 
		mesh.colors().push_back( color );
	}

	//góra (y+)
	if ( v->n.bitRightY )
	{
		myFaces.push_back( CFace4( D, C, F, E ) ); 
		mesh.colors().push_back( color );
	}

	// przód (z+)
	if ( v->n.bitRightZ )
	{
		myFaces.push_back( CFace4( A, B, C, D ) ); 
		mesh.colors().push_back( color );
	}

	//lewa (x-)
	if ( v->n.bitLeftX )
	{
		myFaces.push_back( CFace4( E, H, A, D ) ); 
		mesh.colors().push_back( color );
	}

	//dó³ (y-)
	if ( v->n.bitLeftY )
	{
		myFaces.push_back( CFace4( H, G, B, A ) ); 
		mesh.colors().push_back( color );
	}

	// ty³ (z-)
	if ( v->n.bitLeftZ )
	{
		myFaces.push_back( CFace4( H, E, F, G ) ); 
		mesh.colors().push_back( color );
	}
}

void CVolData3D::volumReGenerate( CMesh &mesh )
{
	CVector3f wkrot(0,0,0);// = AP::getWorkspace()->GetRotation();
	CVector3f obrot;
	if ( NULL != m_parent )
		obrot = m_parent->getRotation();
	else
		obrot = CVector3f( 0.0f, 0.0f, 0.0f );

	CVector3f rot = ( wkrot + obrot ) % 360;
	float rx = rot.X();
	float ry = rot.Y();
	float rz = rot.Z();

	bool tstx = ( rx > 90 ) && ( rx < 270 );
	bool tsty = ( ry > 90 ) && ( ry < 270 );
	bool tstz = ( rz > 90 ) && ( rz < 270 );

	mesh.vertices().clear();
	mesh.colors().clear();

	myFaces.clear();

	bool bz = ( ( tstx && !tsty ) || ( !tstx && tsty ) );
	bool by = ( ( tstx && !tstz ) || ( !tstx && tstz ) );
	bool bx = ( ( tsty && !tstz ) || ( !tsty && tstz ) );

	int iz = bz ? max.Z()+1 : min.Z()-1;
	int ez = bz ? min.Z() : max.Z();

	do
	{
		iz = bz ? iz-1 : iz+1;

		int iy = by ? max.Y()+1 : min.Y()-1;
		int ey = by ? min.Y() : max.Y();

		do
		{
			iy = by ? iy-1 : iy+1;

			int ix = bx ? max.X()+1 : min.X()-1;
			int ex = bx ? min.X() : max.X();

			do
			{
				ix = bx ? ix-1 : ix+1;
				
				SVolData *v = &m_data.at(CTriple<int>(ix,iy,iz));
				if ( NULL != v )
				{
					CRGBA c;
					if ( m_type == 0 )
						c.Set( v->c, v->c, v->c, m_alpha );
					else if ( v->c < 126 )
						c.Set( 0, 255 - v->c, 0, m_alpha );
					else if ( v->c > 128 )
						c.Set( v->c, 0, 0, m_alpha );

					//volumAddBox( mesh, ix, iy, iz, 0.5, CRGBA( v->r, v->g, v->b, m_alpha ) );
					volumAddBox( mesh, ix, iy, iz, 0.5, c );
				}
			}
			while ( ix != ex );
		}
		while ( iy != ey );
	}
	while ( iz != ez );
}

void CVolData3D::volumRender1( CMesh &mesh, unsigned int options )
{
	CMesh::Colors::iterator color = mesh.colors().begin();

	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK );
	glFrontFace(GL_CCW);

	glBegin(GL_QUADS);

	for ( face4 = myFaces.begin(); face4 != myFaces.end(); face4++ )
	{
		glColor4ubv( color->V() );
		color++;

		CVector3f vn = face4->GetNormal( mesh.vertices() );
		glNormal3f( vn.X(), vn.Y(), vn.Z() );

		glVertex3f( mesh.vertices()[ face4->A() ].X(), mesh.vertices()[ face4->A() ].Y(), mesh.vertices()[ face4->A() ].Z() );
		glVertex3f( mesh.vertices()[ face4->B() ].X(), mesh.vertices()[ face4->B() ].Y(), mesh.vertices()[ face4->B() ].Z() );
		glVertex3f( mesh.vertices()[ face4->C() ].X(), mesh.vertices()[ face4->C() ].Y(), mesh.vertices()[ face4->C() ].Z() );
		glVertex3f( mesh.vertices()[ face4->D() ].X(), mesh.vertices()[ face4->D() ].Y(), mesh.vertices()[ face4->D() ].Z() );
	}

	glEnd();

	glDisable( GL_CULL_FACE );
}

*/

void CVolData3D::addVoxel1( int ix, int iy, int iz, SVolData v )
{
		v.n.bitLeftX = 1;
		v.n.bitLeftY = 1;
		v.n.bitLeftZ = 1;
		v.n.bitRightX = 1;
		v.n.bitRightY = 1;
		v.n.bitRightZ = 1;

		if ( ix != 0 )
		{
			try {
				SVolData &neigbour = m_data.at(CTriple<int>(ix-1,iy,iz));
				if ( neigbour.c == v.c )
				{
					v.n.bitLeftX = 0;
					neigbour.n.bitRightX = 0;

					if ( neigbour.isInside() ) m_data.erase( CTriple<int>(ix-1,iy,iz) );
				}
			}
			catch (...) {}
		}

		if ( iy != 0 )
		{
			try {
				SVolData &neigbour = m_data.at(CTriple<int>(ix,iy-1,iz));
				if ( neigbour.c == v.c )
				{
					v.n.bitLeftY = 0;
					neigbour.n.bitRightY = 0;

					if ( neigbour.isInside() ) m_data.erase( CTriple<int>(ix,iy-1,iz) );
				}
			}
			catch (...) {}
		}

		if ( iz != 0 )
		{
			try {
				SVolData &neigbour = m_data.at(CTriple<int>(ix,iy,iz-1));
				if ( neigbour.c == v.c )
				{
					v.n.bitLeftZ = 0;
					neigbour.n.bitRightZ = 0;

					if ( neigbour.isInside() ) m_data.erase( CTriple<int>(ix,iy,iz-1) );
				}
			}
			catch (...) {}
		}

	//this->insert( CVolData3D::value_type( CTriple<int>(ix,iy,iz), *v ) );
	m_data[CTriple<int>(ix,iy,iz)] = v;
}

void CVolData3D::addVoxel( int ix, int iy, int iz, SVolData *v )
{
	if ( v != NULL )
	{
		v->n.bitLeftX = 1;
		v->n.bitLeftY = 1;
		v->n.bitLeftZ = 1;
		v->n.bitRightX = 1;
		v->n.bitRightY = 1;
		v->n.bitRightZ = 1;

		if ( ix != 0 )
		{
			try {
				SVolData *neigbour = &m_data.at(CTriple<int>(ix-1,iy,iz));
				if ( neigbour != NULL )
				{
					v->n.bitLeftX = 0;
					neigbour->n.bitRightX = 0;

					if ( neigbour->isInside() ) m_data.erase( CTriple<int>(ix-1,iy,iz) );
				}
			}
			catch (...) {}
		}

		if ( iy != 0 )
		{
			try {
				SVolData *neigbour = &m_data.at(CTriple<int>(ix,iy-1,iz));
				if ( neigbour != NULL )
				{
					v->n.bitLeftY = 0;
					neigbour->n.bitRightY = 0;

					if ( neigbour->isInside() ) m_data.erase( CTriple<int>(ix,iy-1,iz) );
				}
			}
			catch (...) {}
		}

		if ( iz != 0 )
		{
			try {
				SVolData *neigbour = &m_data.at(CTriple<int>(ix,iy,iz-1));
				if ( neigbour != NULL )
				{
					v->n.bitLeftZ = 0;
					neigbour->n.bitRightZ = 0;

					if ( neigbour->isInside() ) m_data.erase( CTriple<int>(ix,iy,iz-1) );
				}
			}
			catch (...) {}
		}
	}

	//this->insert( CVolData3D::value_type( CTriple<int>(ix,iy,iz), *v ) );
	m_data[CTriple<int>(ix,iy,iz)] = *v;
}

void CVolData3D::drawVoxel( float ix, float iy, float iz, SVolData *v )
{
	float w = 0.5;

	float left = (float)ix-w;
	float right = (float)ix+w;
	float bottom = (float)iy-w;
	float top = (float)iy+w;
	float back = (float)iz+w;
	float front = (float)iz-w;

	if ( NULL != v )
	{
		if ( m_type == 0 )
			glColor4ub( v->c, v->c, v->c, m_alpha );
		else if ( v->c < 126 )
			glColor4ub( 0, 255 - v->c, 0, m_alpha );
		else if ( v->c > 128 )
			glColor4ub( v->c, 0, 0, m_alpha );

		if ( v->n.bitLeftX ) //LEWO
		{
			glVertex3f( left, bottom, front );
			glVertex3f( left, bottom, back );
			glVertex3f( left, top, back );
			glVertex3f( left, top, front );
		}
		
		if ( v->n.bitRightX ) //PRAWO
		{
			glVertex3f( right, top, back );
			glVertex3f( right, bottom, back );
			glVertex3f( right, bottom, front );
			glVertex3f( right, top, front );
		}
		

		if ( v->n.bitLeftY ) //DÓ£
		{
			glVertex3f( left, bottom, front );
			glVertex3f( right, bottom, front );
			glVertex3f( right, bottom, back );
			glVertex3f( left, bottom, back );
		}

		if ( v->n.bitRightY ) //GÓRA
		{
			glVertex3f( right, top, back );
			glVertex3f( right, top, front );
			glVertex3f( left, top, front );
			glVertex3f( left, top, back );
		}


		if ( v->n.bitLeftZ ) //PRZOD
		{
			glVertex3f( left, bottom, front );
			glVertex3f( left, top, front );
			glVertex3f( right, top, front );
			glVertex3f( right, bottom, front );
		}

		if ( v->n.bitRightZ ) //TY£
		{
			glVertex3f( right, top, back );
			glVertex3f( left, top, back );
			glVertex3f( left, bottom, back );
			glVertex3f( right, bottom, back );
		}
	
	}
}

void CVolData3D::volumRender()
{
	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK );
	glFrontFace(GL_CCW);

	glBegin(GL_QUADS);

	CVolData3D::DataType::iterator itz;

	for ( itz = m_data.begin(); itz != m_data.end(); itz++ )
	{
		drawVoxel( itz->first.X(), itz->first.Y(), itz->first.Z(), &itz->second );
	}

	glEnd();

	glDisable( GL_CULL_FACE );
}
