#ifndef GLVIEWER_H
#define GLVIEWER_H

#include "Global.h"
#include "UI.h"
#include "MainApplication.h"
#include "Shader.h"

#include "Wektor3D.h"

#include "Camera.h"

#define LEFT_EYE_PROJECTION -1
#define RIGHT_EYE_PROJECTION 1

#include <QTWidgets/QOpenGLWidget>
#include <QGLWidget>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QTGui/QMatrix4x4>

#include "Quaternion.h"

class __declspec(dllexport) GLViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	typedef enum { 
		STEREO_NONE=0,
		STEREO_REDCYAN,
		STEREO_REDBLUE,
		STEREO_REDGREEN,
		STEREO_CYANRED,
		STEREO_BLUERED,
		STEREO_GREENRED,
		STEREO_QUADBUFF=32,
		STEREO_INTERLACED,
		STEREO_COLUMNINTERLACED,
		STEREO_SIDEBYSIDE,
		STEREO_ABOVEBELOW
	} StereoType;

	typedef enum {
		switchOn,
		switchOff,
		switchToggle
	} Switch;

	typedef enum {
		PERSPECTIVE = 0,
		ORTHOGONAL = 128
	} Projection;

	explicit GLViewer(QWidget *parent = 0, const QOpenGLWidget *shared = 0);
    ~GLViewer();

	bool switchStereo( GLViewer::Switch option ) {
		if ( option == GLViewer::Switch::switchToggle )
			return bStereo = !bStereo; 
		else if ( option == GLViewer::Switch::switchOn )
			return bStereo = true;
		else
			return bStereo = false;
	};

	const GLViewer::StereoType setStereoMode( GLViewer::StereoType mode, GLViewer::Switch option = GLViewer::Switch::switchOn ) { bStereo = ( option == GLViewer::Switch::switchOn ); return iStereoMode = mode; };
	const GLViewer::StereoType getStereoMode() const { return iStereoMode; };
	const GLViewer::StereoType nextStereoMode() { if ( bStereo ) { (GLViewer::StereoType)(1+(int)iStereoMode); if ( iStereoMode > GLViewer::StereoType::STEREO_GREENRED ) iStereoMode = GLViewer::StereoType::STEREO_REDCYAN; } return iStereoMode; };

	void updateGL() { update();  };

	void resetGeometry();
	void fullScreen();

	inline CTransform &transform() { return m_transform; }
	
	void setProjection( GLViewer::Projection );
	void applyProjection();

	void setCameraFloating( bool f ) { m_floatingCamera = f; }

public slots:

	//the required function definitions for the mouse events.
    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );

	void setBGcolor( GLfloat col )
	{
		_fBgColor = col;
		glClearColor( _fBgColor, _fBgColor, _fBgColor, 0.0f);
	};

	void recalcView()
	{
		if ( _projection != ORTHOGONAL )
		{
			_left = -_fAspect * _near * tan( PI * _dViewingAngle / 360.0 );
			_right = _fAspect * _near * tan( PI * _dViewingAngle / 360.0 );
			_bottom = -_near * tan( PI * _dViewingAngle / 360.0 );
			_top = _near * tan( PI * _dViewingAngle / 360.0 );
		}
		else
		{
			_left = -_fAspect * 200.0 * tan( PI * _dViewingAngle / 360.0 );
			_right = _fAspect * 200.0 * tan( PI * _dViewingAngle / 360.0 );
			_bottom = -200.0 * tan( PI * _dViewingAngle / 360.0 );
			_top = 200.0 * tan( PI * _dViewingAngle / 360.0 );
		}
	};

	void setVAngle( GLfloat angle )
	{
		_dViewingAngle = angle;

		recalcView();
	};

signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);
	void translationChanged(double x, double y, double z);

protected:
    // required, used to initialized the OpenGL environment
    void initializeGL();
    // used to draw in the window
    void paintGL();
    // used to reset the drawing frustum when the window is resized
    void resizeGL( int w, int h );

	void mouseDoubleClickEvent(QMouseEvent *e);
	void keyPressEvent( QKeyEvent *e );

	void PickObject(int x, int y);
	void PickPoint(int x, int y);
	int readSelectBuffer(int x, int y);

	void cameraTransformations();

	void Render();
	void RenderScene();
	void RenderSceneMono();
	void RenderSceneAnaglyph();

	void RenderRightView();
	void RenderLeftView();

	void RenderSceneAboveBelow();
	void RenderSceneInterlaced();
	void RenderSceneColumnInterlaced();
	void RenderSceneSideBySide();
	void RenderSceneQuadBuff();
	
	void StereoProjection (	double dfLeftBorder, double dfRightBorder,double dfBottomBorder, double dFVertical,double dfNearBorder, double dfFarBorder,double dfTargetPlane, double dfCameraToTargetDistance,double dfStereoMagnitudeAdj, double dfParallaxBalanceAdj,int WhichEyeProjection);
	const double SlideStereoCameras( double step ) { return FCameraSeparation += step; };

	bool convertWinToWorld(Trio<double>, Trio<double> &);
	bool convertWorldToWin(Trio<double>, Trio<double> &);
	//template<typename _W> bool convertWinToWorld( Trio<_W>, Trio<_W> & );
	//template<typename _W> bool convertWorldToWin( Trio<_W>, Trio<_W> & );

	bool convertCoords(GLdouble winX, GLdouble winY, CPunkt3D &pkt0, CPunkt3D &pkt1);

private:
	bool m_floatingCamera;

	CCamera cam;

	CTransform m_transform;

	QPoint lastPos;
	//To indicate whether the mouse is moving while a mouse button is pressed
	int moving;
	float px0, py0, px1, py1;

	LONG sL;
	LONG sB;
	LONG sW;
	LONG sH;

	float _near;
	float _far;
	float _fAspect;
	float _fBgColor;
	GLdouble _dViewingAngle;

	GLdouble _left;
	GLdouble _right;
	GLdouble _bottom;
	GLdouble _top;

	GLViewer::Projection _projection;

	bool bStereo;
	GLViewer::StereoType iStereoMode;

	//--- STEREO_PROJECTION_SETTINGS ------------//
	double	FCameraSeparation;
	double	FStereoMagnitudeAdjustment;
	double	FParallaxBalanceAdjustment;
	double  FDistance;
	double	StereoLeftBorder;
	double	StereoRightBorder;
	double	FVertical;
	double	StereoBottomBorder;
	double	FNear;
	double	FFar;
	double	FPlan;
	//--- /STEREO_PROJECTION_SETTINGS ------------//

public:
	void useShaders( bool );
	bool shaderOn;
	CShader shaderProgram;
	GLenum prog;



	//***********************************************************************
	//Transformacje punktu w uk³adzie wspó³rzêdnych obiektu
	//***********************************************************************
//	Trio<double> multByRot(const Trio<double> rot1, const Trio<double> pkt);
//	Trio<double> moveByTra(const Trio<double> tra, const Trio<double> pkt);
//	Trio<double> scaleBySca(const double sca, const Trio<double> pkt);

//	Trio<double> localToWorldTransformation(const Trio<double> pkt1);
//	Trio<double> worldToLocalTransformation(const Trio<double> pkt1);




};

#endif // MAINWIDGET_H
