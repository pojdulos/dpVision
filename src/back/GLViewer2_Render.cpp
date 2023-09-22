#include "AP.h"
#include "GLViewer.h"
#include "Rotacja.h"

void GLViewer::cameraTransformations()
{
	// Macierz obrotu
	double rotationMatrix[16];	

	// Konwertuj kwaternion do macierzy obrotu
	m_transform.rotation().GetMatrixD(rotationMatrix);

	//glScalef( Scale, Scale, Scale );
  
	//------------------------------------
	// jeœli obracamy kamerê, to tutaj
	if ( m_floatingCamera )
	{
		//m_transform.renderRotation();

		glMultMatrixd(rotationMatrix);
	}
	//-------------------------------------


	//m_transform.renderTranslation();
	glTranslatef( m_transform.translation().X(), m_transform.translation().Y(), m_transform.translation().Z() );

	//sta³e przesuniecie sceny wzgl widza - przeniesione do lookAt()
	//glTranslatef( 0.0f, 0.0f, -200.0f );

	//------------------------------------
	// jeœli obracamy scenê to tutaj:
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
	applyProjection();	// ustawia perspektywê dla okna

	glMatrixMode(GL_MODELVIEW);		// wybierz macierz modeli
	glLoadIdentity();				// zresetuj j¹

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
}


void GLViewer::RenderSceneAnaglyph()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	glClearColor( _fBgColor, _fBgColor, _fBgColor, 0.0f );

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
	if ( shaderOn ) {
		shaderProgram.initialize();
		shaderProgram.addVertexShader( "vshader.glsl" );
		shaderProgram.addFragmentShader( "fshader.glsl" );

		shaderProgram.use();
	}
	else {
		shaderProgram.disable();
	}
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
	// NP. AP::getApp()->GetBGplaneRef().Render( SINGLE_IMAGE ); 

	cameraTransformations();

	AP::getWorkspace()->renderLights( false ); // false == workspace (mobile) lights
	AP::getWorkspace()->render();

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

	//AP::getApp()->GetBGplaneRef().Render( LEFT_IMAGE );
	
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

	//AP::getApp()->GetBGplaneRef().Render( RIGHT_IMAGE );
	
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

