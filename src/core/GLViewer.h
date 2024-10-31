#pragma once
#ifndef GL_VIEWERx_H
#define GL_VIEWERx_H

//#include "Global.h"
#include "UI.h"
#include "AP.h"

#include <QTGui/QMatrix4x4>

//#include "Shader.h"
#include "Wektor3D.h"
#include "Camera.h"

#include "Quaternion.h"
#include "Transform.h"

#define LEFT_EYE_PROJECTION -1
#define RIGHT_EYE_PROJECTION 1

#include <QTWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
//#include <QtGui/QOpenGLFunctions_3_2_Core>
#include <QtOpenGL>

#include "RGBA.h"

#include <set>

class CBoundVector;
class CAnnotationPyramid;
class CVolTK;
#include "Volumetric.h"

class DPVISION_EXPORT GLViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	typedef int AxesStyle;
	static const int AxesStyle_NONE = 0;
	static const int AxesStyle_ARROWS = 1;
	static const int AxesStyle_LINES = 2;
	static const int AxesStyle_PLANES = 3;
	static const int AxesStyle_END = 4;

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

	explicit GLViewer(QWidget *parent = 0);
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

	void resetGeometry();

	inline CTransform &transform() { return m_transform; }
	
	void setProjection( GLViewer::Projection );
	void applyProjection();
	bool currentProjectionIs(GLViewer::Projection);

	void setCameraFloating( bool f ) { m_floatingCamera = f; }

	void setAxesVisible(AxesStyle s) { m_drawAxes = s; };
	void toggleAxesVisibility() { m_drawAxes+=1; if (m_drawAxes==AxesStyle_END) m_drawAxes=AxesStyle_NONE; };

	void screenshot(QString path = QString());

	void setSelectionMode(int mode);

	void deleteSelectedVoxelsVolTK(CVolTK& cloud, CTransform& transform, bool deleteSelected);

	void deleteSelectedVoxels(Volumetric* vol, bool deleteSelected);

	void deleteSelectedVertices(bool deleteSelected=true);

	void drawMaskCircle(QPoint pos);

public slots:

	//the required function definitions for the mouse events.
    void mousePressEvent( QMouseEvent* event ) override;
	void mouseReleaseEvent( QMouseEvent* event ) override;
	void rotate(double dx, double dy);
	void translate(double dx, double dy, double dz=0.0);
	void mouseMoveEvent( QMouseEvent* event ) override;
	void wheelEvent(QWheelEvent* event) override;

	void setBGcolor(GLfloat col);
	void setBGcolor(CRGBA& col);

	CRGBA& getBGcolor();

	void recalcView();

	void setVAngle(GLdouble angle);
	GLdouble getVAngle();

	void setOrthoSize(GLdouble fov);
	GLdouble getOrthoSize();


signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);
	void translationChanged(double x, double y, double z);

protected:
    // required, used to initialized the OpenGL environment
    void initializeGL() override;
	
	// used to draw in the window
	void paintGL() override;

	// used to reset the drawing frustum when the window is resized
    void resizeGL( int w, int h ) override;

	void draw3Dcontent();
	void drawOverlays(QPainter & painter);

	void PickObject(int x, int y);
	
	void PickMeshPoint(double xx, double yy, CModel3D* obj);
	void PickCloudPoint(double xx, double yy, CModel3D* obj);
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

	void axis3D(double W1, double L1, double W2, double W3);

	void triad3D(double W1 = 0.02, double L1 = 0.8, double W2 = 0.04, double W3 = 0.08, bool active = true);

	void glSetFont();

	void glDrawString(GLfloat x, GLfloat y, QString string);

	void toolBar(void);

	void objectTriad(void);

	void cameraTriad(void);

	void rysujOsie();

	void StereoProjection (	double dfLeftBorder, double dfRightBorder,double dfBottomBorder, double dFVertical,double dfNearBorder, double dfFarBorder,double dfTargetPlane, double dfCameraToTargetDistance,double dfStereoMagnitudeAdj, double dfParallaxBalanceAdj,int WhichEyeProjection);
	const double SlideStereoCameras( double step ) { return FCameraSeparation += step; };

public:
	bool convertWinToWorld(CPoint3d, CPoint3d &);
	bool screen2obj(double xx, double yy, CModel3D* obj, CPoint3d& in, CPoint3d& out, CVector3d& ray);
	bool inTriangle(CPoint3d pt, CPoint3d pA, CPoint3d pB, CPoint3d pC);
	bool inRectangle(CPoint3d pt, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D);
	void findIntersection(CPoint3d origin, CVector3d direction, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D, std::vector<std::pair<double, CPoint3d>>& intersections);
	bool getEntryAndExit(CPoint3d pMin, CPoint3d pMax, CPoint3d origin, CVector3d direction, CPoint3d& entry, CPoint3d& exit);
	CAnnotationPyramid* getPyramid(double xx, double yy, CPoint3d entry, CPoint3d exit, CModel3D* obj, double& r);
	bool isInPixelShadow(double pxlX, double pxlY, double ptX, double ptY);
	bool convertWorldToWin(CPoint3d, CPoint3d &);
	
	CPoint3d world2win(const CPoint3d& world);
	
	//template<typename _W> bool convertWinToWorld( CTriple<_W>, CTriple<_W> & );
	//template<typename _W> bool convertWorldToWin( CTriple<_W>, CTriple<_W> & );

	bool convertCoords(GLdouble winX, GLdouble winY, CPoint3d &pkt0, CPoint3d &pkt1);

	int m_selectionMode;
	bool m_redraw3d;

	QImage m_im;
	QImage m_mask;

	QPoint m_mousePos;
	int m_selSize;
	inline CPoint3d camPos() { return cam.m_pos; };

private:
	bool m_floatingCamera;

	CCamera cam;

	int iNfontList;

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
	GLdouble _dViewingAngle;
	GLdouble _dOrthoViewSize;

	//float _fBgColor;
	CRGBA _fBgColor;

	GLdouble _left;
	GLdouble _right;
	GLdouble _bottom;
	GLdouble _top;

	GLViewer::Projection _projection;

	bool bStereo;
	GLViewer::StereoType iStereoMode;

	AxesStyle m_drawAxes;

	bool m_itWasMaximized;

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
	//CShader shaderProgram;
	GLenum prog;



	//***********************************************************************
	//Transformacje punktu w układzie współrzędnych obiektu
	//***********************************************************************
//	CTriple<double> multByRot(const CTriple<double> rot1, const CTriple<double> pkt);
//	CTriple<double> moveByTra(const CTriple<double> tra, const CTriple<double> pkt);
//	CTriple<double> scaleBySca(const double sca, const CTriple<double> pkt);

//	CTriple<double> localToWorldTransformation(const CTriple<double> pkt1);
//	CTriple<double> worldToLocalTransformation(const CTriple<double> pkt1);



	void closeEvent(QCloseEvent * closeEvent) override;
};

#endif // GLVIEWERx_H
