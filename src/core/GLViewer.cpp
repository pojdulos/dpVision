
//#include "AP.h"

//#include <QMdiSubWindow>
//#include <QCloseEvent>
#include <QMouseEvent>
#include <QApplication>

#include "Model3D.h"

//#include <assert.h>
//#include "MainWindow.h"

#include "GLViewer.h"

bool mouse_key_pressed;

GLViewer::GLViewer(QWidget *parent) : QOpenGLWidget(parent)
{
	QWidget::setAttribute(Qt::WA_DeleteOnClose);

	QSurfaceFormat format;

	// Ustaw odpowiednie flagi dla formatu
	//format.setAccumBufferSize(8);    // Włącz bufor akumulacji z domyślnym rozmiarem 8 bitów na kanał
	format.setDepthBufferSize(24);   // Włącz bufor głębokości z rozmiarem 24 bity
	format.setRenderableType(QSurfaceFormat::OpenGL);  // Używa OpenGL (zamiast np. OpenGLES)
	format.setStereo(true);          // Włącz buforowanie stereo
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);  // Włącz podwójne buforowanie
	format.setRedBufferSize(8);      // Ustaw rozmiar bufora czerwieni na 8 bitów (dla formatu RGBA)
	format.setGreenBufferSize(8);    // Ustaw rozmiar bufora zieleni na 8 bitów
	format.setBlueBufferSize(8);     // Ustaw rozmiar bufora niebieskiego na 8 bitów
	format.setAlphaBufferSize(8);    // Ustaw rozmiar bufora alfa na 8 bitów
	
	//format.setStencilBufferSize(8);
	//format.setVersion(2, 1);
	//format.setProfile(QSurfaceFormat::CoreProfile);  // Użyj profilu rdzeniowego

	// Ustaw format dla tego widgetu
	this->setFormat(format);

	im_rendering_now = false;

	m_floatingCamera = false;

	m_transform.reset();


	_dViewingAngle = 50.0;
	_dOrthoViewSize = 90.0;

	_near = 0.1f;
	_far = 100000.1f;

	_projection = GLViewer::Projection::PERSPECTIVE;

	_fBgColor.SetFloat(0.3f, 0.3f, 0.3f, 1.0);

	bStereo = false;
	iStereoMode = STEREO_REDCYAN;

	//--- STEREO_PROJECTION_SETTINGS ------------//
	FCameraSeparation = 0.07; // - zmiany powodują rozsuwanie widoków
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

	shaderOn = false;

	m_drawAxes = AxesStyle_ARROWS;

	m_selectionMode = 0;
	m_redraw3d = true;
	m_selSize = 20;
}

GLViewer::~GLViewer()
{
	glDeleteLists(iNfontList, 255);
}


void GLViewer::resetGeometry()
{
	m_transform.translation() = CVector3d(0.0, 0.0, 0.0);
	m_transform.scale() = CPoint3d(1.0, 1.0, 1.0);
	m_transform.rotation().setIdentity();

	cam.reset();

	update();
}

void GLViewer::initializeGL()
{
	initializeOpenGLFunctions();

	QOpenGLWidget::initializeGL();

	glClearColor(_fBgColor.fR(), _fBgColor.fG(), _fBgColor.fB(), 0.0f);

	glSetFont();

	setMouseTracking(true);
}


void GLViewer::resizeGL(int w, int h)
{
//	qDebug() << "QGLWidget::resizeGL()\n\r";
	QOpenGLWidget::resizeGL(w, h);
	
//	qDebug() << "GLViewer::resizeGL()\n\r";


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

	if (m_selectionMode != 0)
	{
		//double scale = ((double)this->height()) / m_mask.height();
		//m_mask = m_mask.scaled(scale*m_mask.width(),this->height());
		m_mask = m_mask.scaled(this->size());
	}
}


//void GLViewer::drawOverlays(QPainter &painter)
//{
//	if (m_selectionMode != 0)
//	{
//		QImage newImg(m_mask.size(), QImage::Format_ARGB32);
//		newImg.fill(Qt::transparent);
//
//		QPainter ptr(&newImg);
//		ptr.setOpacity(0.25);
//		ptr.drawImage( m_mask.rect(), m_mask );
//		ptr.end();
//
//		painter.drawImage(0,0,newImg);
//		painter.setPen(Qt::blue);
//		painter.drawText(30, 30, "Tryb zaznaczania (FUNKCJA TESTOWA)");
//		painter.drawEllipse(m_mousePos, m_selSize, m_selSize);
//	}
//}

void GLViewer::drawOverlays(QPainter& painter)
{
	if (m_selectionMode == 1)
	{
		QImage newImg(m_mask.size(), QImage::Format_ARGB32);
		newImg.fill(Qt::transparent);

		QPainter ptr(&newImg);
		ptr.setOpacity(0.51);
		ptr.drawImage(m_mask.rect(), m_mask);
		
		ptr.setPen(Qt::blue);
		ptr.drawText(30, 30, "Select areas for deletion, and press <DEL>");
		ptr.drawEllipse(m_mousePos, m_selSize, m_selSize);

		ptr.end();

		painter.drawImage(0, 0, newImg);
	}
	else if (m_selectionMode == 99)
	{
		QImage newImg(m_mask.size(), QImage::Format_ARGB32);
		newImg.fill(Qt::transparent);

		QPainter ptr(&newImg);
		ptr.setOpacity(0.51);
		ptr.drawImage(m_mask.rect(), m_mask);

		ptr.setPen(Qt::red);
		ptr.drawText(30, 30, "Deleting vertices (PLEASE WAIT...)");
		//ptr.drawEllipse(m_mousePos, m_selSize, m_selSize);

		ptr.end();

		painter.drawImage(0, 0, newImg);
	}
}


void GLViewer::paintGL()
{
	//QOpenGLContext* context = QOpenGLContext::currentContext();
	//QSurfaceFormat format1 = context->format();

	//// Wersja OpenGL
	//int major = format1.version().first;
	//int minor = format1.version().second;
	//qDebug() << "OpenGL Version:" << major << "." << minor;

	//// Profil OpenGL
	//QSurfaceFormat::OpenGLContextProfile profile = format1.profile();
	//if (profile == QSurfaceFormat::CoreProfile) {
	//	qDebug() << "Core Profile";
	//}
	//else if (profile == QSurfaceFormat::CompatibilityProfile) {
	//	qDebug() << "Compatibility Profile";
	//}
	//else {
	//	qDebug() << "No Profile";
	//}

	//// Czy jest to kontekst debugowy?
	//if (format1.options() & QSurfaceFormat::DebugContext) {
	//	qDebug() << "Debug context enabled";
	//}

	//// Inne właściwości formatu
	//qDebug() << "Depth buffer size:" << format1.depthBufferSize();
	//qDebug() << "Stencil buffer size:" << format1.stencilBufferSize();
	//qDebug() << "Samples (for multisampling):" << format1.samples();
	//qDebug() << "Swap behavior:" << format1.swapBehavior();

	QPainter painter;
	painter.begin(this);

	if (m_redraw3d)
	{
		draw3Dcontent();
	}
	else
	{
		painter.drawImage(0, 0, m_im);
	}

	drawOverlays(painter);

	painter.end();

	//toolBar();
}

void GLViewer::draw3Dcontent()
{
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glClearColor( _fBgColor.fR(), _fBgColor.fG(), _fBgColor.fB(), 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);	// wybierz macierz projekcji
	glLoadIdentity();				// zresetuj ją
	
	//assert(glGetError() == GL_NO_ERROR);

	Render();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

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
	CBaseObject* obj = UI::DOCK::WORKSPACE::getCurrentItemObj();
	if (obj != nullptr) {
		if (obj->mousePressEvent(event)) return;
	}

	mouse_key_pressed = true;
  //get the information about the position of the mouse.
    lastPos = event->pos();

	if (m_selectionMode == 0)
	{
		if (event->button() == Qt::MouseButton::LeftButton)
		{
			if (Qt::AltModifier & event->modifiers())
			{
				if (Qt::ShiftModifier & event->modifiers())   //shift+control
				{
					PickObject(event->pos().x(), event->pos().y());
				}
				else // tylko control
				{
					PickPoint(event->pos().x(), event->pos().y());
				}
				UI::DOCK::PROPERTIES::updateProperties();
			}
		}
		//if the middle mouse button is pressed ...
		if (event->button() == Qt::MouseButton::MiddleButton)
		{
			float dx = event->pos().x();
			float dy = event->pos().y();
			//get the x and y (screen) coordinates of the mouse and output the value.
			//std::cout << "mouse position: " << dx << ", " << dy << std::endl;
		}
	}
	else if (m_selectionMode == 1)
	{
		if (event->button() == Qt::MouseButton::LeftButton)
		{
			//QPainter painter;
			//painter.begin(this);
			//draw3Dcontent();
			//m_im = this->grabFrameBuffer();
			//m_mask = QImage(this->size(), QImage::Format::Format_ARGB32);
			//painter.end();
			//m_redraw3d = false;
			drawMaskCircle( event->pos() );
		}
	}
}

void GLViewer::screenshot(QString path)
{
//	draw3Dcontent();
	QImage image = this->grabFramebuffer();

	QString fileName = path;
	if (fileName.isEmpty()) {
		fileName = QFileDialog::getSaveFileName(nullptr, "Save screenshot", "", QString("PNG (*.png);;TIFF (*.tif);;JPEG (*.jpg);;Bitmap (*.bmp);;Gif (*.gif)"));
	}
	
	if (!fileName.isEmpty()) {
		image.save(fileName);
	}
}

void GLViewer::setSelectionMode(int mode)
{
	m_selectionMode = mode;
	QCursor c = this->cursor();
	if (mode ==1)
	{
		QPainter painter;
		painter.begin(this);
		draw3Dcontent();
		m_im = this->grabFramebuffer();
		m_mask = QImage(this->size(), QImage::Format::Format_ARGB32);
		painter.end();
		m_redraw3d = false;
		c.setShape(Qt::CursorShape::PointingHandCursor);
	}
	else if (mode == 99)
	{
		m_redraw3d = false;
		c.setShape(Qt::CursorShape::WaitCursor);
	}
	else
	{
		m_im = QImage();
		m_mask = QImage();
		m_redraw3d = true;
		c.setShape(Qt::CursorShape::ArrowCursor);

	}
	this->setCursor(c);
	repaint();
}

#include "VolTK.h"

void GLViewer::deleteSelectedVoxelsVolTK(CVolTK& vol, CTransform& transform, bool deleteSelected)
{
	int i = 0;
	UI::PROGRESSBAR::init(0, vol.m_displayData.size(), 0);

	CVolTK::DisplayDataPart selected, unselected;

	uint16_t wB = vol.m_currentIntensityRange.lower;
	uint16_t wE;

	for ( const CVolTK::DisplayData::value_type &dataPart : vol.m_displayData )
	{
		//selected.reserve(dataPart.second->size());
		//unselected.reserve(dataPart.second->size());
		selected.reserve(dataPart->size());
		unselected.reserve(dataPart->size());

		//for (CPoint3s dp : *dataPart.second)
		for (CPoint3s dp : *dataPart)
		{
			CPoint3d point = vol.realXYZ(dp);
			CPoint3d workspace = transform.l2w(point);
			CPoint3d world = m_transform.l2w(workspace) - cam.m_pos;
			CPoint3d win = world2win(world);

			QPoint pXY(win.x, win.y);

			if (qRgba(0, 255, 255, 255) == m_mask.pixel(pXY))
			{
				selected.push_back(dp);
			}
			else
			{
				unselected.push_back(dp);
			}
		}

		selected.shrink_to_fit();
		unselected.shrink_to_fit();

		//*dataPart.second = (deleteSelected) ? unselected : selected;
		*dataPart = (deleteSelected) ? unselected : selected;

		selected.clear();
		unselected.clear();

		UI::PROGRESSBAR::setValue(++i);
	}

	UI::PROGRESSBAR::hide();
}

// the glhProjectf (works only from perspective projection.
// With the orthogonal projection it gives different results than standard gluProject. 
// https://www.khronos.org/opengl/wiki/GluProject_and_gluUnProject_code
int glhProjectf(double objx, double objy, double objz, double* modelview, double* projection, int* viewport, double* windowCoordinate)
{
	// Transformation vectors
	double fTempo[8];
	// Modelview transform
	fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12]; // w is always 1
	fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
	fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
	fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
	// Projection transform, the final row of projection matrix is always [0 0 -1 0]
	// so we optimize for that.
	fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
	fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
	fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
	fTempo[7] = -fTempo[2];
	// The result normalizes between -1 and 1
	if (fTempo[7] == 0.0) // The w value
		return 0;
	fTempo[7] = 1.0 / fTempo[7];
	// Perspective division
	fTempo[4] *= fTempo[7];
	fTempo[5] *= fTempo[7];
	fTempo[6] *= fTempo[7];
	// Window coordinates
	// Map x, y to range 0-1
	windowCoordinate[0] = (fTempo[4] * 0.5 + 0.5) * viewport[2] + viewport[0];
	windowCoordinate[1] = (fTempo[5] * 0.5 + 0.5) * viewport[3] + viewport[1];
	// This is only correct when glDepthRange(0.0, 1.0)
	windowCoordinate[2] = (1.0 + fTempo[6]) * 0.5;	// Between 0 and 1
	return 1;
}

#include <omp.h>
#include <GL/glu.h>

void GLViewer::deleteSelectedVoxels(Volumetric* vol, bool deleteSelected)
{

	Eigen::Matrix4d m1 = vol->getGlobalTransformationMatrix(); // point to workspace
	Eigen::Matrix4d m2 = m_transform.toEigenMatrix4d(); // workspace to viewer

	Eigen::Matrix4d M = m2 * m1;

	Volumetric::VoxelType zero = vol->m_minVal - 1;

	Eigen::Vector4d cam_pos = cam.m_pos.toVector4();

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	makeCurrent();
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	doneCurrent();

	//// Konwersja modelview na Eigen::Matrix4d
	//Eigen::Matrix4d modelviewMatrix;
	//for (int i = 0; i < 16; ++i)
	//{
	//	modelviewMatrix(i / 4, i % 4) = modelview[i];
	//}

	//// Konwersja projection na Eigen::Matrix4d
	//Eigen::Matrix4d projectionMatrix;
	//for (int i = 0; i < 16; ++i)
	//{
	//	projectionMatrix(i / 4, i % 4) = projection[i];
	//}

	GLdouble viewportD[4];
	for (int i = 0; i < 4; ++i)
	{
		viewportD[i] = double(viewport[i]);
	}

	UI::PROGRESSBAR::init(0, vol->layers(), 0);

	for (int z = 0; z < vol->layers(); z++)
	{
		UI::PROGRESSBAR::setValue(z);

#pragma omp parallel for
		for (int y = 0; y < vol->rows(); y++)
			for (int x = 0; x < vol->columns(); x++)
			{
				Eigen::Vector4d pt = vol->realXYZ(x, y, z).toVector4();
				Eigen::Vector4d worldPt = M * pt - cam_pos;

				double winX, winY, winZ;

				int res1 = gluProject(worldPt.x(), worldPt.y(), worldPt.z(), modelview, projection, viewport, &winX, &winY, &winZ);
				CPoint3d win(winX, viewportD[3] - winY, winZ);


				//double wc[3];
				//int res = glhProjectf(worldPt.x(), worldPt.y(), worldPt.z(), modelview, projection, viewport, wc);
				//CPoint3d win(wc[0], viewportD[3] - wc[1], wc[2]);

				QPoint pXY(win.x, win.y);

				if ((deleteSelected && (m_mask.pixel(pXY) == qRgba(0, 255, 255, 255)))
					|| (!deleteSelected && (m_mask.pixel(pXY)) == qRgba(0, 0, 0, 0)))
				{
					(*vol)[z][y * vol->columns() + x] = zero;
					vol->m_minVal = zero;
				}
			}
	}

	UI::updateAllViews();

	UI::PROGRESSBAR::hide();
}

#include <omp.h>

void get_GL_matrices(GLViewer *v, GLdouble *modelview, GLdouble *projection, GLint *viewport) {
	v->makeCurrent();
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	v->doneCurrent();
}

// Funkcja do równoległego przetwarzania
CPoint3d world2win_parallel(const CPoint3d& world, GLdouble* modelview, GLdouble* projection, GLint* viewport) {
	double winX, winY, winZ;

	int res1 = gluProject(world.X(), world.Y(), world.Z(), modelview, projection, viewport, &winX, &winY, &winZ);

	return CPoint3d(winX, (double)viewport[3] - winY, winZ);
}

void GLViewer::deleteSelectedVertices(bool deleteSelected)
{
	CModel3D *obj = AP::WORKSPACE::getCurrentModel();
	if (NULL != obj)
	{
		setSelectionMode(99);

		for (auto& child : obj->children())
		{
			if (child.second->getSelfVisibility())
			{
				if (child.second->hasType(CObject::VOLUMETRIC_NEW))
				{
					deleteSelectedVoxels((Volumetric*)child.second, deleteSelected);
				}
				else if (child.second->hasType(CObject::VOLTK))
				{
					deleteSelectedVoxelsVolTK(*(CVolTK*)child.second, obj->transform(), deleteSelected);
				}
				else if (child.second->hasType(CObject::CLOUD) || child.second->hasType(CObject::ORDEREDCLOUD) || child.second->hasType(CObject::MESH))
				{

					CPointCloud* cloud = (CPointCloud*)child.second;

					int size = cloud->vertices().size();
					int step = size / 100;
					//int i = size - 1;

					UI::PROGRESSBAR::init(0, size, 0);

					//std::string fname(obj->fileInfo().absolutePathA() + "/test.txt");
				//FILE *plik = fopen(fname.c_str(), "w");

					CPointCloud::Vertices tempV;
					CPointCloud::Colors tempC;

					std::map<size_t, size_t> idxMap;

					Eigen::Matrix4d m1 = cloud->getGlobalTransformationMatrix(); // point to workspace
					Eigen::Matrix4d m2 = m_transform.toEigenMatrix4d(); // workspace to viewer
					
					m2.block<3, 1>(0, 3) -= cam.m_pos.toVector3();
					
					Eigen::Matrix4d M = m2 * m1;

					cloud->vnormals().clear();

					GLint viewport[4];
					GLdouble modelview[16];
					GLdouble projection[16];
					get_GL_matrices(this, modelview, projection, viewport);

					//#pragma omp parallel for
					for (int i = 0; i < size; i++)
					{
						//UI::STATUSBAR::printfTimed(500, QString::number(i).toStdString().c_str());
						//if ((i%10000)==0) printf("Start iteracji %d przez wątek %d\n", i, omp_get_thread_num());

						CPoint3d point = cloud->vertices()[i];

						Eigen::Vector4d worldPt = M * point.toVector4();

						CPoint3d win = world2win_parallel(CPoint3d(worldPt[0], worldPt[1], worldPt[2]), modelview, projection, viewport);

						bool conditionMet = true;
						if ((win.x >= 0) && (win.y >= 0) && (m_mask.width() > win.x) && (m_mask.height() > win.y))
						{
							QPoint pXY(win.x, win.y);
							conditionMet = (deleteSelected && (qRgba(0, 255, 255, 255) != m_mask.pixel(pXY)))
								|| (!deleteSelected && (qRgba(0, 255, 255, 255) == m_mask.pixel(pXY)));
						}
						
						if (conditionMet)
						{
							//#pragma omp critical
							{
								tempV.push_back(point);
								idxMap[i] = tempV.size() - 1;
								if (cloud->hasVertexColors())
									tempC.push_back(cloud->vcolors()[i]);
							}
						}
						
						

						
						//#pragma omp atomic
						//std::cout << i << endl;
						//UI::STATUSBAR::setText(QString::number(i));
						if ((i % step) == 0) UI::PROGRESSBAR::setValue(i);
						//printf("Koniec iteracji %d przez wątek %d\n", i, omp_get_thread_num());
					}
					
					if (cloud->hasType(CObject::MESH))
					{
						CMesh* mesh = (CMesh*)cloud;
						if (mesh->hasFaces())
						{
							mesh->fnormals().clear();

							CMesh::Faces new_faces;

							obj->switchOption(CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff);
							obj->switchOption(CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOn);

							UI::PROGRESSBAR::init(0, mesh->faces().size(), 0);
							for (int j = 0; j < mesh->faces().size(); j++)
							{
								if ((j%1000)==0) //UI::STATUSBAR::setText(QString("Reindexing faces: %1").arg(j));
									UI::PROGRESSBAR::setValue(j);
								CFace f = mesh->faces()[j];
								
								if ((idxMap.find(f.A()) != idxMap.end()) &&
									(idxMap.find(f.B()) != idxMap.end()) &&
									(idxMap.find(f.C()) != idxMap.end()))
								{
									new_faces.push_back(f);
								}
							}
							
							mesh->faces() = new_faces;
					
							obj->switchOption(CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff);
							obj->switchOption(CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff);

							mesh->removeUnusedVertices();
						}
					}
					else
					{
						cloud->vertices().swap(tempV);
						cloud->vcolors().swap(tempC);
					}
					idxMap.clear();

					UI::PROGRESSBAR::hide();
					//fclose(plik);

					//cloud->vertices().clear();
					//cloud->vcolors().clear();

					tempV.clear();
					tempV.shrink_to_fit();
					tempC.clear();
					tempC.shrink_to_fit();

				}
				else
				{
					UI::STATUSBAR::setText("Currently, this function only supports cloud-like objects !!!");
				}
			}
		}
		AP::leaveSelectionMode();
	}
	else
	{
		AP::leaveSelectionMode();
		UI::STATUSBAR::setText("You need first select an object!!!");
	}
	this->repaint();
	UI::STATUSBAR::setText("DONE!");
}

void GLViewer::drawMaskCircle(QPoint pos)
{
	QPainter qPainter;
	qPainter.begin(&m_mask);
	qPainter.setBrush(Qt::cyan);
	qPainter.setPen(Qt::cyan);
	qPainter.drawEllipse( pos, m_selSize, m_selSize );
	qPainter.end();
}

/***********************************************************************
  when the mouse button is released ...
 */
void GLViewer::mouseReleaseEvent( QMouseEvent* event )
{
	CBaseObject* obj = UI::DOCK::WORKSPACE::getCurrentItemObj();
	if (obj != nullptr) {
		if (obj->mouseReleaseEvent(event)) return;
	}

	mouse_key_pressed = false;

	if (m_selectionMode == 0)
	{
		if (event->button() == Qt::MouseButton::MidButton)
		{
			double dx = event->pos().x();
			double dy = event->pos().y();
			//get the x and y (screen) coordinates of the mouse and output the value.
			//std::cout << "mouse position: " << dx << ", " << dy << std::endl;
		}
	}
	else
	{
		//m_redraw3d = true;
		//m_im = QImage();
	}

	update();
}


//bool GLViewer::addCircle( QPoint pos )
//{
//	// Compare radius of circle with distance  
//	// of its center from given point 
//	for (int x = pos.x() - m_selSize; x <= pos.x() + m_selSize; x++)
//		for (int y = pos.y() - m_selSize; y <= pos.y() + m_selSize; y++)
//			if (((x - pos.x()) * (x - pos.x()) + (y - pos.y()) * (y - pos.y())) <= (m_selSize * m_selSize) )
//			{
//				m_selXY.insert(std::pair<int,int>(x, y));
//				//UI::STATUSBAR::printf(L"CYAN: [%d,%d]", x, y);
//			}
//
//	return false;
//}


void GLViewer::rotate(double dx, double dy) {
	double xAngle = 0.0, yAngle = 0.0, zAngle = 0.0;

	//if (Qt::AltModifier != QApplication::keyboardModifiers())
	//{
	xAngle = deg2rad(dy * 0.3);
	yAngle = deg2rad(dx * 0.3);
	//}
	//else
	//{
	//	xAngle = DEG_TO_RAD(dy) * 0.3;
	//	zAngle = DEG_TO_RAD(dx) * 0.3;
	//}

	CQuaternion invR = m_transform.rotation().inverse();

	//osie układu współrzędnych związanego z ekranem konwertuję do współrzędnych Workspace'a
	CVector3d xAxis = invR * CVector3d::XAxis();
	CVector3d yAxis = invR * CVector3d::YAxis();
	//CVector3d zAxis = invR * CVector3d::ZAxis();

	CModel3D* obj = AP::WORKSPACE::getCurrentModel();

	if (NULL != obj)
	{
		//środek obrotu konwertuję do współrzędnych Workspace'a
		CPoint3d p1 = obj->transform() * obj->getMidpoint();

		// obroty w układzie Workspace'a wokół punktu p1 i wyznaczonych osi
		obj->transform().rotate(p1, { {xAxis, xAngle}, {yAxis, yAngle} }, false);
		//obj->transform().rotate(p1, { {xAxis, xAngle}, {yAxis, yAngle}, {zAxis, zAngle} }, false);

		UI::STATUSBAR::printf(L"rotacja obiektu"); // : [%f, %f, %f]", wX.X(), wX.Y(), wX.Z());
	}
	else
	{
		m_transform.rotate({ {xAxis, xAngle}, {yAxis, yAngle} });
		//m_transform.rotate({ {xAxis, xAngle}, {yAxis, yAngle}, {zAxis, zAngle} });

		UI::STATUSBAR::printf(L"rotacja układu współrzędnych"); // : [%f, %f, %f]", wX.X(), wX.Y(), wX.Z());
	}
	UI::DOCK::PROPERTIES::updateProperties();
}


void GLViewer::translate(double dx, double dy, double dz) {
	if (NULL != AP::WORKSPACE::getCurrentModel())
	{
		CQuaternion invR = m_transform.rotation().inverse();

		CVector3d t = invR * CVector3d(dx, dy, dz);

		AP::WORKSPACE::getCurrentModel()->transform().translate(t);

		emit translationChanged(t.X(), t.Y(), t.Z());

		UI::STATUSBAR::printf(L"translacja obiektu: [%f,%f,%f]", t.X(), t.Y(), t.Z());
	}
	else
	{
		CVector3d t = CVector3d(dx, dy, dz);

		m_transform.translate(t);

		UI::STATUSBAR::printf(L"translacja środka układu współrzędnych: [%f,%f,%f]", t.X(), t.Y(), t.Z());
	}
	UI::DOCK::PROPERTIES::updateProperties();
}

/************************************************************************
  if the mouse is moved ...
 */
void GLViewer::mouseMoveEvent( QMouseEvent* event )
{
	CBaseObject *obj = UI::DOCK::WORKSPACE::getCurrentItemObj();
	if ( obj != nullptr) {
		if (obj->mouseMoveEvent(event)) return;
	}

	if (m_selectionMode == 0) // Normal mode
	{
		double dx = double(event->pos().x()) - double(lastPos.x());
		double dy = double(event->pos().y()) - double(lastPos.y());

		if (Qt::AltModifier & event->modifiers())
		{
			dx /= 100;
			dy /= 100;
		}

		//double	matrix[16];
		//m_transform.rotation().toInvertedGLMatrixD(matrix);

		if (true) {
			if (event->buttons() & Qt::MouseButton::LeftButton)
			{
				if (Qt::ShiftModifier == QApplication::keyboardModifiers())
				{
					double factor = (NULL != AP::WORKSPACE::getCurrentModel()) ? AP::WORKSPACE::getCurrentModel()->transform().scale().x : 5.0;

					this->translate(0.0, 0.0, dy / factor);
				}
				else if (Qt::ControlModifier == QApplication::keyboardModifiers())
				{
					CPoint3d factor = (NULL != AP::WORKSPACE::getCurrentModel()) ? AP::WORKSPACE::getCurrentModel()->transform().scale() : CPoint3d(5.0, 5.0, 5.0);
					
					this->translate(dx / factor.x, -dy / factor.y, 0.0);
				}
				else
				{
					this->rotate(dx, dy);
				}
			}
		}
		else {
			if (event->buttons() & Qt::MouseButton::LeftButton)
			{
				if (Qt::ShiftModifier == QApplication::keyboardModifiers())
				{
					UI::STATUSBAR::setText("SHIFT pressed: nothing to do...");
				}
				else if (Qt::ControlModifier != QApplication::keyboardModifiers())
				{
					this->rotate(dx, dy);
				}
			}
			else if (event->buttons() & Qt::MouseButton::MidButton)
			{
				double factor = (NULL != AP::WORKSPACE::getCurrentModel()) ? AP::WORKSPACE::getCurrentModel()->transform().scale().x : 5.0;

				this->translate(0.0, 0.0, dy / factor);
			}
			else if (event->buttons() & Qt::MouseButton::RightButton)
			{
				CPoint3d factor = (NULL != AP::WORKSPACE::getCurrentModel()) ? AP::WORKSPACE::getCurrentModel()->transform().scale() : CPoint3d(5.0, 5.0, 5.0);

				this->translate(dx / factor.x, -dy / factor.y, 0.0);
			}
		}

	}
	else if ( m_selectionMode == 1 )
	{
		m_mousePos = event->pos();
		UI::STATUSBAR::printf("%d, %d", event->pos().x(), event->pos().y());

		if (event->buttons() & Qt::MouseButton::LeftButton)
		{
			//m_selectionPath.push_back( event->pos() );
			drawMaskCircle(event->pos());
		}
	}


	lastPos = event->pos();

	 //If the mouse is moving we want to keep a note of this, will be useful for other interactions and events in future.
    moving = true;
	UI::updateAllViews();
    moving = false;
}

void GLViewer::wheelEvent(QWheelEvent * event)
{
	CBaseObject* obj = UI::DOCK::WORKSPACE::getCurrentItemObj();
	if (obj != nullptr) {
		if (obj->wheelEvent(event)) return;
	}

	if (m_selectionMode == 0)
	{
		double		matrix[16];
		m_transform.rotation().toInvertedGLMatrixD(matrix);

		double d = -(double)event->angleDelta().y();

		if (NULL != AP::WORKSPACE::getCurrentModel())
		{
			double factor = 10.0 * AP::WORKSPACE::getCurrentModel()->getTransform().scale().x;

			CVector3d t = CVector3d(0.0, 0.0, d / factor).transformByMatrixD(matrix);

			AP::WORKSPACE::getCurrentModel()->getTransform().translate(t);

			UI::STATUSBAR::printf(L"translacja obiektu: [%f,%f,%f]", t.X(), t.Y(), t.Z());
		}
		else
		{
			double factor = 25.0;
			CVector3d t = CVector3d(0.0, 0.0, d / factor);

			m_transform.translate(t);

			UI::STATUSBAR::printf(L"translacja środka układu współrzędnych: [%f,%f,%f]", t.X(), t.Y(), t.Z());
		}

		UI::DOCK::PROPERTIES::updateProperties();
	}
	else if (m_selectionMode == 1)
	{
		if (Qt::ControlModifier != QApplication::keyboardModifiers())
		{
			m_selSize += (event->angleDelta().y() < 0) ? -1 : (event->angleDelta().y() > 0) ? 1 : 0;
			if (m_selSize < 1) m_selSize = 1;
		}
		else
		{
			m_selSize += (event->angleDelta().y() < 0) ? -10 : (event->angleDelta().y() > 0) ? 10 : 0;
			if (m_selSize < 10) m_selSize = 10;
		}
		UI::STATUSBAR::setText(QString("Selection size: %1").arg(m_selSize));
	}

	moving = true;
	UI::updateAllViews();
	moving = false;
}

void GLViewer::setBGcolor(GLfloat col)
{
	_fBgColor.SetFloat(col, col, col, 1.0);
	glClearColor(_fBgColor.fR(), _fBgColor.fG(), _fBgColor.fB(), 0.0f);
	
}

void GLViewer::setBGcolor(CRGBA& col)
{
	_fBgColor.SetFloat(col.fR(), col.fG(), col.fB(), 1.0);
	glClearColor(_fBgColor.fR(), _fBgColor.fG(), _fBgColor.fB(), 0.0f);
}

CRGBA& GLViewer::getBGcolor()
{
	return _fBgColor;
}

void GLViewer::recalcView()
{
	if (_fAspect >= 1.0)
	{
		_top = (_projection == ORTHOGONAL) ? (_dOrthoViewSize) : (_near * tan(PI * _dViewingAngle / 360.0));
		_right = _fAspect * _top;
	}
	else
	{
		_right = (_projection == ORTHOGONAL) ? (_dOrthoViewSize) : (_near * tan(PI * _dViewingAngle / 360.0));
		_top = _right / _fAspect;
	}

	_bottom = -_top;
	_left = -_right;
}

void GLViewer::setVAngle(GLdouble angle)
{
	_dViewingAngle = angle;

	recalcView();
}

GLdouble GLViewer::getVAngle()
{
	return _dViewingAngle;
}

void GLViewer::setOrthoSize(GLdouble fov) {
	_dOrthoViewSize = fov;
	recalcView();
}

GLdouble GLViewer::getOrthoSize() {
	return _dOrthoViewSize;
}


void GLViewer::closeEvent( QCloseEvent * closeEvent )
{
	//emit closed( this->objectName() );

	//if ( mdiArea()->subWindowList().size() < 2 )
	//{
	//	closeEvent->ignore();
	//}
	//else
	//{
	closeEvent->accept();
	//}
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

bool GLViewer::currentProjectionIs(GLViewer::Projection p) {
	return _projection == p;
}

void GLViewer::setProjection( GLViewer::Projection p )
{
	_projection = p;

	recalcView();
}
