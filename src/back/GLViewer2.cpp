#include "AP.h"
#include "GLViewer.h"
#include "QTgui.h"

#include "childwindow.h"

#include <QMouseEvent>

extern CMainApplication myApp;

GLViewer::GLViewer(QWidget *parent, const QOpenGLWidget *shared) :QOpenGLWidget(parent)
{
	m_floatingCamera = false;
	//Rot.Set( 0.0, 0.0, 0.0 ); //xRot = yRot = zRot = 0.0;
	m_transform.setTranslation( CWektor3D( 0.0, 0.0, 0.0 ) ); //xTrans = yTrans = zTrans = 0.0;
	m_transform.setScale( 1.0 );
	m_transform.rotation().SetIdentity();


	_fBgColor = 0.3f;
	_dViewingAngle = 35.0;
	_near = 1.0f;
	_far = 10001.0f;

	_projection = GLViewer::Projection::PERSPECTIVE;

	bStereo = false;
	iStereoMode = STEREO_REDCYAN;

	//--- STEREO_PROJECTION_SETTINGS ------------//
	FCameraSeparation = 0.07; // - zmiany powoduj¹ rozsuwanie widoków
	FStereoMagnitudeAdjustment = 1.0;
	FParallaxBalanceAdjustment = 0.0;
	FDistance = 200.0; //14.5;
	FPlan = -200.0;
	StereoLeftBorder = -10.0;
	StereoRightBorder = 10.0;
	FVertical = 10.0;   //4.8;
	StereoBottomBorder = -10.0;
	FNear = 0.0; //6.0;
	FFar = -10.0; //-6.0;
	//--- /STEREO_PROJECTION_SETTINGS ------------//

	//parent->setWindowTitle("TESTTTTTT");

	shaderOn = false;

	QSurfaceFormat format;
	//format.setVersion(3, 0);
	//format.setSamples(4);
	//format.setProfile(QSurfaceFormat::NoProfile);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	//format.setStereo(true);
	format.setDepthBufferSize(24);
	setFormat(format);
}

GLViewer::~GLViewer()
{
}

void GLViewer::resetGeometry()
{
	m_transform.setTranslation(CWektor3D(0.0, 0.0, 0.0));
	m_transform.setScale(1.0);
	m_transform.rotation().SetIdentity();

	cam.reset();

	//m_velocity.SetIdentity();

	UI::DOCK::MODELPROPERTIES::update();
	this->updateGL();
}

void GLViewer::initializeGL()
{
	initializeOpenGLFunctions();
//	glutInitDisplayMode( GLUT_STEREO | GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH );
}


void GLViewer::resizeGL(int w, int h)
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	sL=sB=0;
	sW=w;
	sH=h;

	glViewport( 0, 0, w, h );
	glScissor( 0, 0, w, h );

	_fAspect = (GLfloat)w / (GLfloat)h;

	recalcView();

	glMatrixMode( GL_MODELVIEW );

	 // Reset projection
    //projection.setToIdentity();

    // Set perspective projection
    //projection.perspective(_dViewingAngle, _fAspect, _near, _far);
}


void GLViewer::paintGL()
{
	GLclampf c = _fBgColor;
	glClearColor( c, c, c, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);	// wybierz macierz projekcji
	glLoadIdentity();				// zresetuj j¹
	

	// Calculate model view transformation
    //QMatrix4x4 matrix;
    //matrix.translate(0.0, 0.0, 10.0);
    //matrix.rotate(rotation);

    // Set modelview-projection matrix
    //program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
    //program.setUniformValue("texture", 0);

	// Tell OpenGL programmable pipeline how to locate vertex position data
    //int vertexLocation = program.attributeLocation("a_position");
    //program.enableAttributeArray(vertexLocation);
	//int texcoordLocation = program.attributeLocation("a_texcoord");
    //program.enableAttributeArray(texcoordLocation);

	Render();
}

static void qNormalizeAngle( int &angle )
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360)
		angle -= 360 * 16;
}

//**********************************************************************
// Set the rotation angle of the object to \e degrees around the X axis.
//void GLViewer::setXRotation( int degrees )
//{   Rot.X( (GLfloat)(degrees % 360) );
//    updateGL();

//	qNormalizeAngle( degrees );
//	if ( degrees != Rot.X() ) {
//		Rot.X( degrees );
//		emit xRotationChanged( degrees );
//		updateGL();
//	}
//}

//**********************************************************************
// Set the rotation angle of the object to \e degrees around the Y axis.
//void GLViewer::setYRotation( int degrees )
//{   Rot.Y( (GLfloat)(degrees % 360) );
//    updateGL();
//}

//**********************************************************************
// Set the rotation angle of the object to \e degrees around the Z axis.
//void GLViewer::setZRotation( int degrees )
//{   Rot.Z( (GLfloat)(degrees % 360) );
//    updateGL();
//}

//****************************************************************************
// The following are 3 functions for dealing with mouse events.


/***************************************************************************
  When the mouse button is pressed ...

  A QMouseEvent is a Qt class that describe moouse events. Events
  occur when a mouse button is pressed and/or released.
 */
void GLViewer::mousePressEvent( QMouseEvent* event )
{
  //get the information about the position of the mouse.
    lastPos = event->pos();

	if(event->button()==Qt::MouseButton::LeftButton)
    {
		if(Qt::ShiftModifier == QApplication::keyboardModifiers())
		{
			PickObject( event->pos().x(), event->pos().y() );
			UI::DOCK::MODELPROPERTIES::update();
			//UserInterface::pOkno->updateModelPropertiesPanel();
		}
		else if(Qt::ControlModifier == QApplication::keyboardModifiers())
		{
			PickPoint( event->pos().x(), event->pos().y() );
			UI::DOCK::MODELPROPERTIES::update();
			//UserInterface::pOkno->updateModelPropertiesPanel();
		}
	}
	 //if the middle mouse button is pressed ...
    if(event->button()==Qt::MouseButton::MiddleButton)
    {
		float dx = event->pos().x();
		float dy = event->pos().y();
		//get the x and y (screen) coordinates of the mouse and output the value.
		//std::cout << "mouse position: " << dx << ", " << dy << std::endl;
    }
}

/***********************************************************************
  when the mouse button is released ...
 */
void GLViewer::mouseReleaseEvent( QMouseEvent* event )
{
    if(event->button()==Qt::MouseButton::MidButton)
    {
		float dx = event->pos().x();
		float dy = event->pos().y();
		//get the x and y (screen) coordinates of the mouse and output the value.
		//std::cout << "mouse position: " << dx << ", " << dy << std::endl;
    }
  //update the OpenGL, this is an OpenGL command.
	this->updateGL();
}

/************************************************************************
  if the mouse is moved ...
 */
void GLViewer::mouseMoveEvent( QMouseEvent* event )
{
    double dx = double(event->pos().x()) - double(lastPos.x());
    double dy = double(event->pos().y()) - double(lastPos.y());

	double		matrix[16];
	m_transform.rotation().GetInvertedMatrixD(matrix);

    if(event->buttons() & Qt::MouseButton::LeftButton)
    {
		if (Qt::ControlModifier != QApplication::keyboardModifiers() )
		{
			float xAngle = 0.0f, yAngle = 0.0f, zAngle = 0.0f;

			if (Qt::AltModifier != QApplication::keyboardModifiers())
			{
				xAngle = DEG_TO_RAD(dy) * 0.3f;
				yAngle = DEG_TO_RAD(dx) * 0.3f;
			}
			else
			{
				xAngle = DEG_TO_RAD(dy) * 0.3f;
				zAngle = DEG_TO_RAD(dx) * 0.3f;
			}

			CVector3<float>	xAxis(1.0f, 0.0f, 0.0f);
			CVector3<float>	yAxis(0.0f, 1.0f, 0.0f);
			CVector3<float>	zAxis(0.0f, 0.0f, 1.0f);

			xAxis = xAxis.transformByMatrixD(matrix);
			yAxis = yAxis.transformByMatrixD(matrix);
			zAxis = zAxis.transformByMatrixD(matrix);

			if ( NULL != AP::WORKSPACE::getSelected() )
			{
				AP::WORKSPACE::getSelected()->getTransform().rotate( xAxis, xAngle, yAxis, yAngle, zAxis, zAngle );
				UI::DOCK::MODELPROPERTIES::update();
			}
			else
			{
				CQuaternion qRotX, qRotY, qRotZ;

				qRotX.FromRotationAxis( xAngle, xAxis.X(), xAxis.Y(), xAxis.Z() );
				qRotY.FromRotationAxis( yAngle, yAxis.X(), yAxis.Y(), yAxis.Z() );
				qRotZ.FromRotationAxis( zAngle, zAxis.X(), zAxis.Y(), zAxis.Z() );

				m_transform.rotation().MultAndSet( qRotX * qRotY * qRotZ );
				m_transform.rotation().Normalize();

				UI::STATUSBAR::printf(L"rotacja uk³adu wspó³rzêdnych"); // : [%f, %f, %f]", wX.X(), wX.Y(), wX.Z());
				UI::DOCK::CAMERAPROPERTIES::update();
			}
		}
    }
    else if(event->buttons() & Qt::MouseButton::MidButton)
    {
		if ( NULL != AP::WORKSPACE::getSelected() )
		{
			double factor = AP::WORKSPACE::getSelected()->getTransform().scale();

			CWektor3D t = CWektor3D( 0.0, 0.0, dy / factor).transformByMatrixD(matrix);

			AP::WORKSPACE::getSelected()->getTransform().translate(t);

			UI::STATUSBAR::printf(L"translacja obiektu: [%f,%f,%f]", t.X(), t.Y(), t.Z());
			UI::DOCK::MODELPROPERTIES::update();
		}
		else
		{
			double factor = 5.0;
			CWektor3D t = CWektor3D( 0.0, 0.0, dy/factor );
						
			m_transform.translate( t );

			UI::STATUSBAR::printf(L"translacja œrodka uk³adu wspó³rzêdnych: [%f,%f,%f]", t.X(), t.Y(), t.Z());
			UI::DOCK::CAMERAPROPERTIES::update();
		}
    }
    else if(event->buttons() & Qt::MouseButton::RightButton)
    {
		if ( NULL != AP::WORKSPACE::getSelected() )
		{
			double factor = AP::WORKSPACE::getSelected()->getTransform().scale();

			CWektor3D t = CWektor3D(dx / factor, -dy / factor, 0.0).transformByMatrixD(matrix);

			AP::WORKSPACE::getSelected()->getTransform().translate( t );

			emit translationChanged( t.X(), t.Y(), t.Z() );

			UI::STATUSBAR::printf(L"translacja obiektu: [%f,%f,%f]", t.X(), t.Y(), t.Z());
			UI::DOCK::MODELPROPERTIES::update();
		}
		else
		{
			double factor = 5.0;
			CWektor3D t = CWektor3D(dx / factor, -dy / factor, 0.0);
			
			m_transform.translate( t );

			UI::STATUSBAR::printf(L"translacja œrodka uk³adu wspó³rzêdnych: [%f,%f,%f]", t.X(), t.Y(), t.Z());
			UI::DOCK::CAMERAPROPERTIES::update();
		}
    }

	lastPos = event->pos();

	 //If the mouse is moving we want to keep a note of this, will be
	 //useful for other interactions and events in future.
    moving = true;
	UI::updateAllViews();
    moving = false;
}


void GLViewer::fullScreen()
{
    if(isFullScreen())
    {
        setWindowFlags(Qt::Widget);
        showNormal();
    }
    else
    {
        setWindowFlags(Qt::Window);
        showFullScreen();
    }
}

void GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	fullScreen();
}

void GLViewer::keyPressEvent( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_F11:
			fullScreen();
			this->updateGL();
			break;
		//case Qt::Key_Escape:
		//	close();
		case Qt::Key_H:
			AP::WORKSPACE::getSelected()->switchOption( CModel3D::Opt::optShow, CModel3D::Switch::switchToggle );
			this->updateGL();
			break;
	}
}

void GLViewer::applyProjection()
{
	if ( _projection != GLViewer::Projection::ORTHOGONAL )
	{
		//gluPerspective( _dViewingAngle, _fAspect, _near, _far );
		glFrustum( _left, _right, _bottom, _top, _near, _far );
	}
	else
	{
		glOrtho( _left, _right, _bottom, _top, _near, _far );
	}
}

void GLViewer::setProjection( GLViewer::Projection p )
{
	_projection = p;

	recalcView();
}

