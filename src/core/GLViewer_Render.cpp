#include "Workspace.h"
#include "BoundVector.h"


#include "GLViewer.h"
#include <GL/glu.h>


void GLViewer::cameraTransformations()
{
	// Macierz obrotu
	double rotationMatrix[16];	

	// Konwertuj kwaternion do macierzy obrotu
	m_transform.rotation().toGLMatrixD(rotationMatrix);

	//glScalef( Scale, Scale, Scale );
  
	//------------------------------------
	// je�li obracamy kamer�, to tutaj
	if ( m_floatingCamera )
	{
		//m_transform.renderRotation();

		glMultMatrixd(rotationMatrix);
	}
	//-------------------------------------


	//m_transform.renderTranslation();
	glTranslatef( m_transform.translation().X(), m_transform.translation().Y(), m_transform.translation().Z() );

	//sta�e przesuniecie sceny wzgl widza - przeniesione do lookAt()
	//glTranslatef( 0.0f, 0.0f, -200.0f );

	//------------------------------------
	// je�li obracamy scen� to tutaj:
	if ( ! m_floatingCamera )
	{
		//m_transform.renderRotation();
		glMultMatrixd(rotationMatrix);
	}
	//-------------------------------------

	//glMatrixMode(GL_MODELVIEW);		// wybierz macierz modeli
}

void GLViewer::Render()
{
	if (im_rendering_now) return;
	
	im_rendering_now = true;

	applyProjection();	// ustawia perspektyw� dla okna

	glMatrixMode(GL_MODELVIEW);		// wybierz macierz modeli
	glLoadIdentity();				// zresetuj j�

	glInitNames();
	glPushName( 0 );


	if ( ! bStereo )
	{
		RenderSceneMono();
	}
	else if ( STEREO_QUADBUFF == iStereoMode )
		RenderSceneQuadBuff();
	else if ( STEREO_INTERLACED == iStereoMode )
		RenderSceneInterlaced();
	else if ( STEREO_COLUMNINTERLACED == iStereoMode )
		RenderSceneColumnInterlaced();
	else if ( STEREO_SIDEBYSIDE == iStereoMode )
		RenderSceneSideBySide();
	else if ( STEREO_ABOVEBELOW == iStereoMode )
		RenderSceneAboveBelow();
	else
	{
		RenderSceneAnaglyph();
	}

	glPopName();

	im_rendering_now = false;
}


void GLViewer::RenderSceneAnaglyph()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	glClearColor( _fBgColor.fR(), _fBgColor.fG(), _fBgColor.fB(), 0.0f );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch ( iStereoMode )
    {
	   case STEREO_REDBLUE:
	   case STEREO_REDGREEN:
	   case STEREO_REDCYAN:
		  glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
		  break;
	   case STEREO_BLUERED:
		  glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
		  break;
	   case STEREO_GREENRED:
		  glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
		  break;
	   case STEREO_CYANRED:
		  glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
		  break;
    }

	RenderLeftView();

	glClear(GL_DEPTH_BUFFER_BIT);

    switch ( iStereoMode )
    {
	   case STEREO_REDBLUE:
		  glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
		  break;
	   case STEREO_REDGREEN:
		  glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
		  break;
	   case STEREO_REDCYAN:
		  glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
		  break;
	   case STEREO_BLUERED:
	   case STEREO_GREENRED:
	   case STEREO_CYANRED:
		  glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
		  break;
    } 

	RenderRightView();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glPopAttrib();
}

void  GLViewer::useShaders( bool ) 
{
	//if ( shaderOn ) {
	//	shaderProgram.initialize();
	//	shaderProgram.addVertexShader( "vshader.glsl" );
	//	shaderProgram.addFragmentShader( "fshader.glsl" );

	//	shaderProgram.use();
	//}
	//else {
	//	shaderProgram.disable();
	//}
}

void GLViewer::RenderScene()
{
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);  

	glShadeModel( GL_SMOOTH );

	glEnable(GL_NORMALIZE);
	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_LIGHTING);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt(cam.m_pos.X(), cam.m_pos.Y(), cam.m_pos.Z(), cam.m_dir.X(), cam.m_dir.Y(), cam.m_dir.Z(), cam.m_up.X(), cam.m_up.Y(), cam.m_up.Z());

	AP::getWorkspace()->renderLights( true );

	// OPCJONALNIE TU MOZNA WYSWIETLAC OBRAZEK TLA
	// NP. AP::mainApp().GetBGplaneRef().Render( SINGLE_IMAGE ); 

	cameraTransformations();

	AP::getWorkspace()->renderLights( false ); // false == workspace (mobile) lights
	if (m_drawAxes) rysujOsie();
	
	AP::getWorkspace()->render();

	//rysujGimbal();

	glDisable(GL_LIGHTING);
	glDisable( GL_DEPTH_TEST );

	glDisable(GL_BLEND);
}

void GLViewer::RenderSceneMono()
{
	//glClearDepth(1.0f);

	glDrawBuffer(GL_BACK);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glAccum(GL_RETURN,1.0);


	RenderScene();
}


void GLViewer::RenderLeftView()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();

	StereoProjection( StereoLeftBorder, StereoRightBorder,
					  -FVertical, FVertical,
					   FNear, FFar, // X,Y,Z coord range
					   FPlan, FDistance, //Z-coord of zero-parallax "target", camera-target dist
					   FStereoMagnitudeAdjustment, FParallaxBalanceAdjustment, 
					   LEFT_EYE_PROJECTION);

	//AP::mainApp().GetBGplaneRef().Render( LEFT_IMAGE );
	
	RenderScene();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}

void GLViewer::RenderRightView()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();

	
	StereoProjection( StereoLeftBorder, StereoRightBorder,
			              -FVertical, FVertical,
						   FNear, FFar, // X,Y,Z coord range
                           FPlan, FDistance, //Z-coord of zero-parallax "target", camera-target dist
                           FStereoMagnitudeAdjustment, FParallaxBalanceAdjustment, 
                           RIGHT_EYE_PROJECTION);

	//AP::mainApp().GetBGplaneRef().Render( RIGHT_IMAGE );
	
	RenderScene();

	glMatrixMode( GL_PROJECTION );
    glPopMatrix();
}

void GLViewer::RenderSceneSideBySide()
{
	long vL = sL;
	long vB = sB;
	long vW = sW;
	long vH = sH; //this->geometry().height();


	glDrawBuffer( GL_BACK );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glViewport( vL, vB, (vW/2), vH );

	RenderLeftView();

	glClear( GL_DEPTH_BUFFER_BIT );

	glViewport( vL+(vW/2), vB, (vW/2), vH );

	RenderRightView();

    glViewport( vL, vB, vW, vH );
}

void GLViewer::RenderSceneColumnInterlaced()
{
	long vL = 0; //sL;
	long vB = 0; //sB;
	long vW = sL+sW;
	long vH = sB+sH;

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	RenderLeftView();

	while ( vW % 4 ) vW++;
    unsigned char *pRGB_L = new unsigned char [3L * vW * vH + 3];

	glReadBuffer( GL_BACK );
	glReadPixels( 0, 0, vW, vH, GL_RGB, GL_UNSIGNED_BYTE, pRGB_L );

	// clear Z-buffer
	glClear( GL_DEPTH_BUFFER_BIT );

	glDrawBuffer( GL_BACK_RIGHT );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	RenderRightView();

    unsigned char *pRGB_R = new unsigned char [3L * vW * vH + 3];
	glReadBuffer( GL_BACK );
	glReadPixels( 0, 0, vW, vH, GL_RGB, GL_UNSIGNED_BYTE, pRGB_R );

	for ( long line = 0; line < vH; line++ )
	{
		for ( long pixel = 0; pixel < vW; pixel++ )
		{
			if ( pixel % 2 )
			{
				pRGB_L[3L*(line*vW+pixel)] = pRGB_R[3L*(line*vW+pixel)];
				pRGB_L[3L*(line*vW+pixel)+1] = pRGB_R[3L*(line*vW+pixel)+1];
				pRGB_L[3L*(line*vW+pixel)+2] = pRGB_R[3L*(line*vW+pixel)+2];
			}
		}
	}

	delete[] pRGB_R;

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDrawPixels( vW, vH, GL_RGB, GL_UNSIGNED_BYTE, pRGB_L );

	delete[] pRGB_L;
}


void GLViewer::RenderSceneAboveBelow()
{
	long vL = sL;
	long vB = sB;
	long vW = sW;
	long vH = sH;

	glDrawBuffer( GL_BACK );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glViewport( vL, vB, vW, (vH/2) );

	RenderLeftView();

	glClear( GL_DEPTH_BUFFER_BIT );

	glViewport( vL, vB+(vH/2), vW, vH-(vH/2) );

	RenderRightView();

	glViewport( vL, vB, vW, vH );
}

void GLViewer::RenderSceneInterlaced()
{
	RenderSceneAboveBelow();

	long vL = 0; //sL;
	long vB = 0; //sB;
	long vW = sL+sW;
	long vH = sB+sH;

	while ( vW % 4 ) vW++;

	unsigned char *pSrc = new unsigned char [3L * vW * vH + 3];
	unsigned char *pDst = new unsigned char [3L * vW * vH + 3];

	glReadBuffer( GL_BACK );
	glReadPixels( vL, vB, vW, vH, GL_RGB, GL_UNSIGNED_BYTE, pSrc );

	long lineWidth = 3L*vW;
	long shift = (vH/2)*lineWidth; 

	unsigned char *dst = pDst;
	unsigned char *src = pSrc;
	unsigned char *src1 = pSrc+shift;

	if ( vH % 2 )
	{
		for ( long lineNumber = 0; lineNumber < vH; lineNumber++ )
		{
			if ( lineNumber % 2 )
			{
				memcpy( (void*) dst, (void*) src, lineWidth );
				src += lineWidth;
			}
			else
			{
				memcpy( (void*) dst, (void*) src1, lineWidth );
				src1 += lineWidth;
			}
			dst += lineWidth;
		}
	}
	else
	{
		for ( long lineNumber = 0; lineNumber < vH; lineNumber++ )
		{
			if ( lineNumber % 2 )
			{
				memcpy( (void*) dst, (void*) src1, lineWidth );
				src1 += lineWidth;
			}
			else
			{
				memcpy( (void*) dst, (void*) src, lineWidth );
				src += lineWidth;
			}
			dst += lineWidth;
		}
	}
	delete[] pSrc;

	glDrawBuffer( GL_BACK );
	glDrawPixels( vW, vH, GL_RGB, GL_UNSIGNED_BYTE, pDst );

	delete[] pDst;
}

void GLViewer::RenderSceneQuadBuff()
{
	glDrawBuffer( GL_BACK_LEFT );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	RenderLeftView();

	// clear Z-buffer
	glClear( GL_DEPTH_BUFFER_BIT );

	// Draw the right eye view
	glDrawBuffer( GL_BACK_RIGHT );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	RenderRightView();
}


void GLViewer::axis3D( double W1, double L1, double W2, double L2 )
{
	//GLUquadric* cyl = gluNewQuadric();

	//gluCylinder(cyl, W1, W1, L1, 16, 1);

	//glPushMatrix();
	//glTranslated(0.0, 0.0, L1);
	//gluCylinder(cyl, W2, 0.001, L2, 12, 1);
	//glPopMatrix();

	//gluDeleteQuadric(cyl);


	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0); glVertex3f(0, 0, 47);
	glEnd();

	glLineWidth(11);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 47); glVertex3f(0, 0, 47.5);
	glEnd();

	glLineWidth(9);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 47.5); glVertex3f(0, 0, 48);
	glEnd();

	glLineWidth(7);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 48); glVertex3f(0, 0, 48.5);
	glEnd();

	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 48.5); glVertex3f(0, 0, 49);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 49); glVertex3f(0, 0, 49.5);
	glEnd();

	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 49.5); glVertex3f(0, 0, 50);
	glEnd();
}

// -----------------------------------------------------------------------------

void GLViewer::triad3D(double W1, double L1, double W2, double L2, bool active)
{
	float xCol[] = { 0.8f, 0.2f, 0.2f };
	float yCol[] = { 0.2f, 0.8f, 0.2f };
	float zCol[] = { 0.2f, 0.2f, 0.8f };

	glColor3f( 0.2f, 0.2f, 0.2f );

	if (active) glColor3fv( zCol );    // Z axis in red.
	axis3D( W1, L1, W2, L2);

	if (active) glColor3fv( yCol );    // Y axis is green.
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	axis3D(W1, L1, W2, L2);
	glPopMatrix();

	if (active) glColor3fv( xCol );    // X axis is blue.
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	axis3D(W1, L1, W2, L2);
	glPopMatrix();

	//GLUquadric* sph = gluNewQuadric();
	if (active) glColor3f( 0.7f, 0.7f, 0.7f );
	//gluSphere(sph, W2, 16, 16);
	//gluDeleteQuadric(sph);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();

}

// -----------------------------------------------------------------------------

void GLViewer::glSetFont()
{
#ifdef _WIN32
	HFONT hFont;
	LOGFONTA logfont;

	HDC hDC = wglGetCurrentDC();

	logfont.lfHeight = -MulDiv(14, GetDeviceCaps(hDC, LOGPIXELSY), 96);
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_NORMAL; //FW_BOLD; //
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = EASTEUROPE_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = CLEARTYPE_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;

	strcpy(logfont.lfFaceName, "MS Sans Serif");

	// Create the font and display list
	hFont = CreateFontIndirectA(&logfont);
	SelectObject(hDC, hFont);
	//SelectObject (hDC, GetStockObject (SYSTEM_FONT));

	//Create display lists for glyphs 0 through 128
	iNfontList = glGenLists(255);
	wglUseFontBitmaps(hDC, 0, 255, iNfontList);

	DeleteObject(hFont);
#endif
}


void GLViewer::glDrawString(GLfloat x, GLfloat y, QString string)
{
	glRasterPos2f(x, y);
	glPushAttrib(GL_LIST_BIT);
	glListBase(iNfontList);
	glCallLists( string.size(), GL_UNSIGNED_BYTE, string.constData() );
	glPopAttrib();
}

void GLViewer::toolBar(void)
{
	cameraTriad();
	objectTriad();
	resizeGL(sW, sH);
}

void GLViewer::objectTriad(void)
{

	CModel3D *obj = AP::WORKSPACE::getCurrentModel();

	if (obj != NULL)
	{
		glViewport(0, 100, 100, 100);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluPerspective(65.0, 1.0, 1.0, 5.0);
		//gluPerspective( _dViewingAngle, 1.0, 1.0, 20.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);


		glTranslatef(0, 0, -2);   // Place small triad between clipping planes.

		glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
		glBegin(GL_QUADS);
		glVertex3f(-1.5, -1.5, -1.0);
		glVertex3f(1.5, -1.5, -1.0);
		glVertex3f(1.5, 1.5, -1.0);
		glVertex3f(-1.5, 1.5, -1.0);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glDrawString(-0.5, -0.5, "object");

		double rotationMatrix[16];
		m_transform.rotation().toGLMatrixD(rotationMatrix);
		glMultMatrixd(rotationMatrix);

		obj->getTransform().rotation().toGLMatrixD(rotationMatrix);
		glMultMatrixd(rotationMatrix);

		triad3D();

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

	}
}

void GLViewer::cameraTriad(void)
{
	glViewport(0, 0, 100, 100);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	gluPerspective(65.0, 1.0, 1.0, 5.0);
	//gluPerspective( _dViewingAngle, 1.0, 1.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);


	glTranslatef(0, 0, -2);   // Place small triad between clipping planes.

	glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
	glBegin(GL_QUADS);
	glVertex3f(-1.5, -1.5, -1.0);
	glVertex3f(1.5, -1.5, -1.0);
	glVertex3f(1.5, 1.5, -1.0);
	glVertex3f(-1.5, 1.5, -1.0);
	glEnd();

	//glRotatef(m_transform.rotation().eulerAngles().X(), 1, 0, 0);
	//glRotatef(m_transform.rotation().eulerAngles().Y(), 0, 1, 0);
	//glRotatef(m_transform.rotation().eulerAngles().Z(), 0, 0, 1);   // Viewing rotations.

//	CModel3D *obj = AP::WORKSPACE::getCurrentModel();

//	if (obj == NULL)
//	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glDrawString(-0.5, -0.5, "camera");
//	}
//	else
//	{
//		glColor3f(0.0f, 1.0f, 0.0f);
//		glDrawString(-0.5, -0.5, "object");
//	}

	double rotationMatrix[16];
	m_transform.rotation().toGLMatrixD(rotationMatrix);
	glMultMatrixd(rotationMatrix);

//	if (obj != NULL)
//	{
//		obj->getTransform().rotation().GetMatrixD(rotationMatrix);
//		glMultMatrixd(rotationMatrix);
//	}

	triad3D();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void GLViewer::rysujOsie()
{
	float xCol[] = { 0.8f, 0.2f, 0.2f };
	float yCol[] = { 0.2f, 0.8f, 0.2f };
	float zCol[] = { 0.2f, 0.2f, 0.8f };

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	float g0 = 0.5f, g1 = 1.0f, a = 1.0f;
	int s0 = 32;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	//glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
	//glColorMaterial(GL_FRONT_AND_BACK,GL_SPECULAR);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LINE_SMOOTH);

	if (m_drawAxes == AxesStyle_ARROWS)
	{
		triad3D( 0.1, 45.0, 0.4, 5.0, (AP::WORKSPACE::getCurrentModelId() < 0) );
	}
	else if (m_drawAxes == AxesStyle_PLANES)
	{
		bool active = (AP::WORKSPACE::getCurrentModelId() < 0);

		glColor3f(0.2f, 0.2f, 0.2f);

		// X
		if (active) glColor3fv(xCol);

		glLineWidth(0.3);
		glBegin(GL_LINES);
		for (auto n = -100; n < 101; n += 10)
		{
			glVertex3f(-100, 0, n); glVertex3f(100, 0, n);
		}
		glEnd();

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex3f(-1000, 0, 0); glVertex3f(1000, 0, 0);
		glEnd();

		// Y
		if (active) glColor3fv(yCol);

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex3f(0, -1000, 0); glVertex3f(0, 1000, 0);
		glEnd();

		// Z
		if (active) glColor3fv(zCol);

		glLineWidth(0.3);
		glBegin(GL_LINES);
		for (auto n = -100; n < 101; n += 10)
		{
			glVertex3f(n, 0, -100); glVertex3f(n, 0, 100);
		}
		glEnd();

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex3f(0, 0, -1000); glVertex3f(0, 0, 1000);
		glEnd();

	}
	else if (m_drawAxes == AxesStyle_LINES)
	{
		bool active = (AP::WORKSPACE::getCurrentModelId() < 0);

		glColor3f(0.2f, 0.2f, 0.2f);
		glLineWidth(1);
		glBegin(GL_LINES);

		// X
		if (active) glColor3fv(xCol);
		glVertex3f(-10000, 0, 0); glVertex3f(10000, 0, 0);

		// Y
		if (active) glColor3fv(yCol);
		glVertex3f(0, -10000, 0); glVertex3f(0, 10000, 0);

		// Z
		if (active) glColor3fv(zCol);
		glVertex3f(0, 0, -10000); glVertex3f(0, 0, 10000);

		glEnd();
	}

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}
