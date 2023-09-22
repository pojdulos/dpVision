#include <QtWidgets/QFileDialog>
#include <QtWidgets/QToolButton>
#include <QtGui/QKeyEvent>
#include <QMessageLogger>

#include "AP.h"

#include "MainApplication.h"
#include "MainWindow.h"

#include "GLViewer.h"
#include "PicViewer.h"
#include "MdiChild.h"

#include "Image.h"


void CMainWindow::closeEvent(QCloseEvent * event)
{
	//QRect r = this->frameGeometry();
	QRect r = this->geometry();

	AP::mainApp().settings->setValue( "mainwindow/maximized", this->isMaximized() );

	if ( !this->isMaximized() ) {
		AP::mainApp().settings->setValue("mainwindow/geometry", r);
	}

	deleteLater();
	QMainWindow::closeEvent(event);
}

void CMainWindow::keyPressEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Control)
	{
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
	}
}

void CMainWindow::keyReleaseEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Control)
	{
		QApplication::restoreOverrideCursor();
	}
}

void CMainWindow::viewerSelected(QMdiSubWindow* window)
{
	qDebug() << "viewerSelected\n\r";

	if (window != nullptr)
	{
		MdiChild* child = (MdiChild*)window->widget();
		if (child->hasType(MdiChild::Type::Pic))
		{
			AP::WORKSPACE::setCurrentModel(((PicViewer*)child->m_widget)->id());
		}

		UI::DOCK::PROPERTIES::updateProperties();
	}
}

void CMainWindow::onCurrentObjectChanged(int i)
{
	changeMenuAfterSelect();
	updateView(true, true);
}

void CMainWindow::onCurrentObjectChanged(CBaseObject* obj)
{
	changeMenuAfterSelect();
	updateView(true, true);
}

//void CMainWindow::createChild()
//{
//	MdiChild::create(MdiChild::Type::GL, ui.mdiArea)->show();
//	//createMdiChild()->show();
//}

void CMainWindow::createGLViewer()
{
//	qDebug() << "createGLViewer()\n\r";

	MdiChild::create(MdiChild::Type::GL, ui.mdiArea, MdiChild::Show::Normal);
}

void CMainWindow::createPicViewer()
{
	MdiChild::create(MdiChild::Type::Pic, ui.mdiArea)->show();
}


void CMainWindow::viewChildFS()
{
	MdiChild* child = currentMdiChild();
	if (NULL != child)
	{
		child->fullScreen();
	}
}

#include "FileConnector.h"

void CMainWindow::openRecent() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		if ( nullptr != AP::WORKSPACE::loadModel(action->data().toString().toStdWString(), false, false ))
		{
			adjustForCurrentFile(action->data().toString());
			AP::mainApp().settings->setValue("recentFile", action->data().toString());
		}
}


void CMainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), AP::mainApp().settings->value("recentFile").toString(), CFileConnector::getLoadExts() );
	
	if (!fileName.isEmpty())
	{
		if ( nullptr != AP::WORKSPACE::loadModel(fileName, false, false ))
		{
			adjustForCurrentFile(fileName);
			AP::mainApp().settings->setValue("recentFile", fileName);
		}
	}
}

void CMainWindow::fileSave()
{
	CModel3D* obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		if (obj->hasType(CObject::IMAGE))
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), AP::WORKSPACE::getCurrentModel()->path(), QString("PNG (*.png);;TIFF (*.tif);;JPEG (*.jpg);;Bitmap (*.bmp);;Gif (*.gif)"));

			if (!fileName.isEmpty())
			{
				((CImage*)obj)->save( fileName );
			}
		}
		else
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), AP::WORKSPACE::getCurrentModel()->path(), CFileConnector::getSaveExts() );

			if (!fileName.isEmpty())
			{
				if ( obj->save(fileName) )
				{
					UI::STATUSBAR::setText(L"Saved: " + fileName.toStdWString());
					adjustForCurrentFile(fileName);
					AP::mainApp().settings->setValue("recentFile", fileName);
				}
			}
		}
	}
	else
	{
		UI::STATUSBAR::setText( "You need to select any object to save..." );
	}
}

#include "Image.h"

void CMainWindow::importImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import image"), AP::mainApp().settings->value("recentFile").toString(), QString("2D Images (*.bmp;*.gif;*.jpg;*.png;*.tif;*.tiff)"));

	if (!fileName.isEmpty())
	{
		CImage* im = CImage::load( fileName );
		if (nullptr != im)
		{
			AP::WORKSPACE::addImage(im, false);
			
			//adjustForCurrentFile(fileName);
			//AP::mainApp().settings->setValue("recentFile", fileName);
		}
	}
}


void CMainWindow::stereoscopyOff()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->switchStereo(GLViewer::Switch::switchOff);
		UI::STATUSBAR::printf("Stereoscopy: Off");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyQuadbuff()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_QUADBUFF);
		UI::STATUSBAR::printf("Stereoscopy: Quad buffering");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRowInterlaced()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_INTERLACED);
		UI::STATUSBAR::printf("Stereoscopy: Row interlaced");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyColInterlaced()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_COLUMNINTERLACED);
		UI::STATUSBAR::printf("Stereoscopy: Column interlaced");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyAboveBelow()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_ABOVEBELOW);
		UI::STATUSBAR::printf("Stereoscopy: Above/Below");
		updateActiveView();
	}
}

void CMainWindow::stereoscopySideBySide()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_SIDEBYSIDE);
		UI::STATUSBAR::printf("Stereoscopy: Side by side");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedCyan()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDCYAN);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Cyan");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedBlue()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDBLUE);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Blue");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedGreen()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDGREEN);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Green");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyCyanRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_CYANRED);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Cyan");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyBlueRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_BLUERED);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Blue");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyGreenRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_GREENRED);
		UI::STATUSBAR::printf("Stereoscopy: Anaglyph Red-Green");
		updateActiveView();
	}
}

void CMainWindow::useShaders( bool on )
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		if (view->shaderOn = on)
		{
			view->useShaders(true);

			UI::STATUSBAR::printf("Shaders On");
		}
		else
		{
			view->useShaders(false);

			UI::STATUSBAR::printf("Shaders Off");
		}
		updateActiveView();
	}
}

void CMainWindow::projectionOrthogonal()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setProjection(GLViewer::Projection::ORTHOGONAL);
		UI::STATUSBAR::printf("Orthogonal projection");
		updateActiveView();
	}
	UI::DOCK::PROPERTIES::updateProperties();
}

void CMainWindow::projectionPerspective()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setProjection(GLViewer::Projection::PERSPECTIVE);
		UI::STATUSBAR::printf("Perspective projection");
		updateActiveView();
	}
	UI::DOCK::PROPERTIES::updateProperties();
}


void CMainWindow::modelVisibility( bool vis )
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if ( nullptr != obj )
	{
		if ( vis )
		{
			obj->setSelfVisibility( true );
			UI::STATUSBAR::printf(  "Model visibility: Show" );
		}
		else
		{
			obj->setSelfVisibility( false );
			UI::STATUSBAR::printf(  "Model visibility: Hide" );
		}
		
		UI::DOCK::WORKSPACE::setItemVisibleById(obj->id(), vis);
		UI::DOCK::PROPERTIES::updateProperties();
		updateAllViews();
	}
}

void CMainWindow::modelInvertNormals()
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if ( NULL != obj )
	{
		if ( ( CBaseObject::Category::OBJECT == obj->getChild()->category() ) && ( CObject::MESH == ((CObject*)obj->getChild())->type() ) )
		{
			((CMesh*)obj->getChild())->invertNormals();

			UI::STATUSBAR::printf(L"Normalne ścian zostały odwrócone");

			updateAllViews();
		}
		else
		{
			UI::STATUSBAR::printf(L"Obiekt nie jest siatką...");
		}
	}
}

void CMainWindow::meshApplyTransformations()
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if (NULL != obj)
	{
		if ( obj->getChild()->category() == CBaseObject::Category::OBJECT )
		{
			if ((((CObject*)obj->getChild())->type() == CObject::MESH) || (((CObject*)obj->getChild())->type() == CObject::CLOUD ) || (((CObject*)obj->getChild())->type() == CObject::ORDEREDCLOUD) )
			{
				AP::WORKSPACE::getCurrentModel()->applyTransform(CTransform());

				//CTransform trans = AP::WORKSPACE::getCurrentModel()->getTransform();
				//((CMesh*)AP::WORKSPACE::getCurrentModel()->getChild())->applyTransformation( trans, CTransform() );
				//AP::WORKSPACE::getCurrentModel()->setTransform(CTransform());

				UI::STATUSBAR::printf(L"Współrzędne obiektu zostały przekształcone");

				updateAllViews();
			}
			else
			{
				UI::STATUSBAR::printf(L"Obiekt nie jest chmurą punktów lub siatką...");
			}
		}
		else
		{
			UI::STATUSBAR::printf(L"Obiekt nie jest chmurą punktów lub siatką...");
		}

		UI::DOCK::PROPERTIES::updateProperties();
	}
}

void CMainWindow::renderAsFaces()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff );
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff );
		UI::STATUSBAR::printf(  "Rendering: Faces" );
		
		updateAllViews();
	}
}

void CMainWindow::renderAsEdges()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff );
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOn );
		UI::STATUSBAR::printf(  "Rendering: Edges" );

		updateAllViews();
	}
}

void CMainWindow::renderAsVertices()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff );
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOn );
		UI::STATUSBAR::printf( "Rendering: Vertices" );
		
		updateAllViews();
	}
}

void CMainWindow::textureOnOff()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		if ( AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderWithTexture, CModel3D::Switch::switchToggle ) )
		{
			//ModifyMenu(GetMenu( hWnd ), IDM_MODEL_TEXTUREONOFF, MF_BYCOMMAND|MF_STRING, IDM_MODEL_TEXTUREONOFF, GetResourceString( IDS_MODEL_TEXTUREOFF ).c_str() ); 
			UI::STATUSBAR::printf(  "Texture: on" );
		}
		else
		{
			//ModifyMenu(GetMenu( hWnd ), IDM_MODEL_TEXTUREONOFF, MF_BYCOMMAND|MF_STRING, IDM_MODEL_TEXTUREONOFF, GetResourceString( IDS_MODEL_TEXTUREON ).c_str() ); 
			UI::STATUSBAR::printf(  "Texture: off" );
		}

		updateAllViews();
	}
}

void CMainWindow::smoothingOnOff()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->calcVN();
		//UI::STATUSBAR::printf( AP::WORKSPACE::getCurrentModel()->GetMeshInfoText().c_str() );

		UI::STATUSBAR::printf( AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optSmoothVertices, CModel3D::Switch::switchToggle ) ? "Wygladzanie wierzcholkow: wlaczone" : "Wygladzanie wierzcholkow: wylaczone" );
	}
	else
		UI::STATUSBAR::printf( "None objects selected" );

	updateAllViews();
}

void CMainWindow::createNewCopy()
{
	AP::WORKSPACE::duplicateCurrentModel();
}

void CMainWindow::cameraResetPosition()
{
		GLViewer *view = this->currentViewer();
		if (nullptr != view)
		{
			view->resetGeometry();
			UI::DOCK::PROPERTIES::updateProperties();
			UI::updateAllViews();
		}
}


void CMainWindow::actionLookDir(int direction, CModel3D* obj)
{
	if ((direction < 1) || (direction > 6)) return;

	GLViewer* view = this->currentViewer();
	
	if (view == nullptr) return;

	view->resetGeometry(); // == "actionLookF"
	
	//1 - front, 2 - back, 3 - left, 4 - right, 5 - top, 6 - bottom
	if (direction == 5)
	{
		view->transform().rotateAroundAxisDeg(CVector3d::XAxis(), 90);
	}
	else if (direction == 6)
	{
		view->transform().rotateAroundAxisDeg(CVector3d::XAxis(), -90);
	}
	else if (direction == 3)
	{
		view->transform().rotateAroundAxisDeg(CVector3d::YAxis(), 90);
	}
	else if (direction == 4)
	{
		view->transform().rotateAroundAxisDeg(CVector3d::YAxis(), -90);
	}
	else if (direction == 2)
	{
		view->transform().rotateAroundAxisDeg(CVector3d::YAxis(), 180);
	}

	bool center = true;
	CBoundingBox bb;

	
	if (obj != nullptr)
	{
		CModel3D* current = obj;

		if (current->getBoundingBox().isInvalid()) return;

		CPoint3d min = current->getMin();
		CPoint3d max = current->getMax();

		Eigen::Matrix4d T = current->transform().toEigenMatrix4d();

		CPoint3d m1 = T * min;
		CPoint3d m2 = T * CPoint3d(min.x, min.y, max.z);
		CPoint3d m3 = T * CPoint3d(min.x, max.y, max.z);
		CPoint3d m4 = T * CPoint3d(min.x, max.y, min.z);
		CPoint3d m5 = T * CPoint3d(max.x, max.y, min.z);
		CPoint3d m6 = T * CPoint3d(max.x, min.y, min.z);
		CPoint3d m7 = T * CPoint3d(max.x, min.y, max.z);
		CPoint3d m8 = T * max;

		bb.expand(m1);
		bb.expand(m2);
		bb.expand(m3);
		bb.expand(m4);
		bb.expand(m5);
		bb.expand(m6);
		bb.expand(m7);
		bb.expand(m8);
	}
	else
	{
		bb = AP::WORKSPACE::instance()->topBB();

		if (bb.isInvalid()) return;

	}

	//	CModel3D* mdl = new CModel3D;
	//	mdl->getBoundingBox() = bb;
	//	AP::WORKSPACE::addModel(mdl);
	//	mdl->setLocked(true);

	if (center)
	{
		CPoint3d mdl = bb.getMidpoint();

		std::cout << "\nmidpoint:\n" << mdl.toVector3() << "\n";

		CVector3d tra;// = CPoint3d() - mdl;

		//1 - front, 2 - back, 3 - left, 4 - right, 5 - top, 6 - bottom
		if (direction == 1)
		{
			tra.Set(-mdl.x, -mdl.y, 0.0);
		}
		else if (direction == 5)
		{
			tra.Set(-mdl.x, mdl.z, 0.0);
		}
		else if (direction == 6)
		{
			tra.Set(-mdl.x, -mdl.z, 0.0);
		}
		else if (direction == 3)
		{
			tra.Set(-mdl.z, -mdl.y, 0.0);
		}
		else if (direction == 4)
		{
			tra.Set(mdl.z, -mdl.y, 0.0);
		}
		else if (direction == 2)
		{
			tra.Set(mdl.x, -mdl.y, 0.0);
		}

		view->transform().translate(tra);
	}

	CVector3d correction(0.0, 0.0, -10.0);

	CPoint3d min = bb.getMin();
	CPoint3d max = bb.getMax();

	Eigen::Matrix4d T = view->transform().toEigenMatrix4d();

	CPoint3d m1, m2, m3, m4, m5, m6, m7, m8;

	m1 = T * min;
	m2 = T * CPoint3d(min.x, min.y, max.z);
	m3 = T * CPoint3d(min.x, max.y, max.z);
	m4 = T * CPoint3d(min.x, max.y, min.z);
	m5 = T * CPoint3d(max.x, max.y, min.z);
	m6 = T * CPoint3d(max.x, min.y, min.z);
	m7 = T * CPoint3d(max.x, min.y, max.z);
	m8 = T * max;

	m1 = view->world2win(m1 - view->camPos());
	m2 = view->world2win(m2 - view->camPos());
	m3 = view->world2win(m3 - view->camPos());
	m4 = view->world2win(m4 - view->camPos());
	m5 = view->world2win(m5 - view->camPos());
	m6 = view->world2win(m6 - view->camPos());
	m7 = view->world2win(m7 - view->camPos());
	m8 = view->world2win(m8 - view->camPos());

	QRect r = view->rect();

	double minX = std::min(std::min(std::min(m1.x, m2.x), std::min(m3.x, m4.x)), std::min(std::min(m5.x, m6.x), std::min(m7.x, m8.x)));
	double minY = std::min(std::min(std::min(m1.y, m2.y), std::min(m3.y, m4.y)), std::min(std::min(m5.y, m6.y), std::min(m7.y, m8.y)));

	double maxX = std::max(std::max(std::max(m1.x, m2.x), std::max(m3.x, m4.x)), std::max(std::max(m5.x, m6.x), std::max(m7.x, m8.x)));
	double maxY = std::max(std::max(std::max(m1.y, m2.y), std::max(m3.y, m4.y)), std::max(std::max(m5.y, m6.y), std::max(m7.y, m8.y)));

	while ((minX < 0) || (minY < 0) || (maxX >= r.width()) || (maxY >= r.height()))
	{
		view->transform().translate(correction);

		T = view->transform().toEigenMatrix4d();

		m1 = T * min;
		m2 = T * CPoint3d(min.x, min.y, max.z);
		m3 = T * CPoint3d(min.x, max.y, max.z);
		m4 = T * CPoint3d(min.x, max.y, min.z);
		m5 = T * CPoint3d(max.x, max.y, min.z);
		m6 = T * CPoint3d(max.x, min.y, min.z);
		m7 = T * CPoint3d(max.x, min.y, max.z);
		m8 = T * max;

		m1 = view->world2win(m1 - view->camPos());
		m2 = view->world2win(m2 - view->camPos());
		m3 = view->world2win(m3 - view->camPos());
		m4 = view->world2win(m4 - view->camPos());
		m5 = view->world2win(m5 - view->camPos());
		m6 = view->world2win(m6 - view->camPos());
		m7 = view->world2win(m7 - view->camPos());
		m8 = view->world2win(m8 - view->camPos());

		minX = std::min(std::min(std::min(m1.x, m2.x), std::min(m3.x, m4.x)), std::min(std::min(m5.x, m6.x), std::min(m7.x, m8.x)));
		minY = std::min(std::min(std::min(m1.y, m2.y), std::min(m3.y, m4.y)), std::min(std::min(m5.y, m6.y), std::min(m7.y, m8.y)));

		maxX = std::max(std::max(std::max(m1.x, m2.x), std::max(m3.x, m4.x)), std::max(std::max(m5.x, m6.x), std::max(m7.x, m8.x)));
		maxY = std::max(std::max(std::max(m1.y, m2.y), std::max(m3.y, m4.y)), std::max(std::max(m5.y, m6.y), std::max(m7.y, m8.y)));
	}

	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}


void CMainWindow::actionLookDir()
{
	QObject* s = sender();
	QString name = s->objectName();

	int dir = 1;

	if (name.startsWith("actionLookB"))
	{
		dir = 2;
	}
	else if (name.startsWith("actionLookL"))
	{
		dir = 3;
	}
	else if (name.startsWith("actionLookR"))
	{
		dir = 4;
	}
	if (name.startsWith("actionLookT"))
	{
		dir = 5;
	}
	else if (name.startsWith("actionLookD"))
	{
		dir = 6;
	}

	actionLookDir(dir, AP::WORKSPACE::getCurrentModel());
}

void CMainWindow::actionSelectSelectionDelete()
{
	currentViewer()->deleteSelectedVertices(true);
}

void CMainWindow::actionSelectNoSelectionDelete()
{
	currentViewer()->deleteSelectedVertices(false);
}

void CMainWindow::actionSelectVertex()
{
	UI::STATUSBAR::setText("Select Vertex");
	currentViewer()->setSelectionMode( 1 );
	ui.action_select_selection_delete->setEnabled(true);
	ui.action_select_noselection_delete->setEnabled(true);
	UI::updateCurrentView();
}

void CMainWindow::actionSelectFace()
{
	UI::STATUSBAR::setText("Select Face");
	currentViewer()->setSelectionMode( 2 );
	ui.action_select_selection_delete->setEnabled(true);
	ui.action_select_noselection_delete->setEnabled(true);
	UI::updateCurrentView();
}

void CMainWindow::actionSelectNone()
{
	UI::STATUSBAR::setText("Select None");
	currentViewer()->setSelectionMode( 0 );
	ui.action_select_selection_delete->setEnabled(false);
	ui.action_select_noselection_delete->setEnabled(false);
	UI::updateCurrentView();
}

#include "PicViewer.h"

void CMainWindow::imageFit(bool fit)
{
	CModel3D * im = AP::WORKSPACE::getCurrentModel();
	if ((im != nullptr) && im->hasType(CObject::IMAGE))
	{
		((CImage*)im)->fitToWindow = fit;
	}

	QMdiSubWindow* window = getPicViewerInstance(im->id());
	if (window != nullptr)
	{
		((PicViewer*)((MdiChild*)window->widget())->m_widget)->reloadImage();
	}
}

void CMainWindow::bbShowHide(bool show)
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		while ( show != AP::WORKSPACE::getCurrentModel()->toggleDrawBB() ) {};

		if ( show )
		{
			UI::STATUSBAR::printf(  "Bounding box: on" );
		}
		else
		{
			UI::STATUSBAR::printf(  "Bounding box: off" );
		}

		updateAllViews();
	}
	else
	{
		GLViewer *view = this->currentViewer();
		if (NULL != view)
		{
			view->toggleAxesVisibility();

			updateActiveView();
		}
	}
}

void CMainWindow::openWorkspace()
{
	//QString path = QFileDialog::getOpenFileName(this, tr("Open Workspace"), "", tr("dpVision multiarchive file (*.dpvision);;Faro scene (*.Faro)"));
	QString path = QFileDialog::getOpenFileName(this, tr("Open Workspace"), "", tr("dpVision multiarchive file (*.dpvision)"));

	if ("" != path)
	{
		int reply = UI::MESSAGEBOX::question(L"You are about to removing all models in your workspace.\nRealy you want to do it?", L"Caution!");
		if (0 == reply)
		{
			AP::WORKSPACE::instance()->load(path);
		}
	}
}


void CMainWindow::saveWorkspace()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save Workspace"),"",tr("dpVision multiarchive file (*.dpvision)"));
	
	if ( "" != path ) 
	{
		if (AP::WORKSPACE::instance()->save(path))
		{
			adjustForCurrentFile(path);
			AP::mainApp().settings->setValue("recentFile", path);
		}
	}
}

void CMainWindow::removeAllModels()
{
	int reply = UI::MESSAGEBOX::question(L"You are about to removing all models in your workspace.\nRealy you want to do it?", L"Caution!");
	if ( 0 == reply ) AP::WORKSPACE::removeAllModels();
}

void CMainWindow::removeSelectedModels()
{
	int reply = UI::MESSAGEBOX::question(L"You are about to removing all selected (checked) models.\nRealy you want to do it?", L"Caution!");
	if (0 == reply) AP::WORKSPACE::removeSelectedModels();
}

void CMainWindow::resetAllTransformations()
{
	for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->transform().reset();
	}
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::resetSelectedTransformations()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		AP::WORKSPACE::getModel(*it)->transform().reset();
	}
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::lockAllModels()
{
	//UI::MESSAGEBOX::information(L"Lock ALL");
	for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->setLocked(true);
		UI::DOCK::WORKSPACE::setItemLockedById(it->first, true);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::lockSelectedModels()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		CModel3D *obj = AP::WORKSPACE::getModel(*it);
		obj->setLocked(true);
		UI::DOCK::WORKSPACE::setItemLockedById(*it, true);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::unlockAllModels()
{
	//UI::MESSAGEBOX::information(L"Lock ALL");
	for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->setLocked(false);
		UI::DOCK::WORKSPACE::setItemLockedById(it->first, false);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::unlockSelectedModels()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		CModel3D *obj = AP::WORKSPACE::getModel(*it);
		obj->setLocked(false);
		UI::DOCK::WORKSPACE::setItemLockedById(*it, false);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void CMainWindow::selectAll()
{
	AP::getWorkspace()->getSelection().clear();
	for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		AP::WORKSPACE::SELECTION::selectModel(it->first);
	}
}

void CMainWindow::unselectAll()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		AP::WORKSPACE::SELECTION::unselectModel(*it);
	}
}

void CMainWindow::hideAllModels()
{
	AP::WORKSPACE::setAllModelsVisible(false);
	//for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	//{
	//	it->second->setVisible(false);
	//	UI::DOCK::WORKSPACE::setItemVisibleById(it->first, false);
	//}
	//UI::changeMenuAfterSelect();
	//UI::DOCK::PROPERTIES::updateProperties();
	//UI::updateAllViews();
}

void CMainWindow::hideSelectedModels()
{
	AP::WORKSPACE::SELECTION::setModelsVisible(false);
	//std::list<int> sel = AP::getWorkspace()->getSelection();
	//for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	//{
	//	CModel3D *obj = AP::WORKSPACE::getModel(*it);
	//	obj->setVisible(false);
	//	UI::DOCK::WORKSPACE::setItemVisibleById(*it, false);
	//}
	//UI::changeMenuAfterSelect();
	//UI::DOCK::PROPERTIES::updateProperties();
	//UI::updateAllViews();
}

void CMainWindow::showAllModels()
{
	AP::WORKSPACE::setAllModelsVisible(true);
	//for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	//{
	//	it->second->setVisible(true);
	//	UI::DOCK::WORKSPACE::setItemVisibleById(it->first, true);
	//}
	//UI::changeMenuAfterSelect();
	//UI::DOCK::PROPERTIES::updateProperties();
	//UI::updateAllViews();
}

void CMainWindow::showSelectedModels()
{
	AP::WORKSPACE::SELECTION::setModelsVisible(true);
	//std::list<int> sel = AP::getWorkspace()->getSelection();
	//for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	//{
	//	CModel3D *obj = AP::WORKSPACE::getModel(*it);
	//	obj->setVisible(true);
	//	UI::DOCK::WORKSPACE::setItemVisibleById(*it, true);
	//}
	//UI::changeMenuAfterSelect();
	//UI::DOCK::PROPERTIES::updateProperties();
	//UI::updateAllViews();
}

void CMainWindow::modelInSelection(bool b)
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if (nullptr != obj)
	{
		if (b)
		{
			AP::WORKSPACE::SELECTION::selectModel(obj->id());
		}
		else
		{
			AP::WORKSPACE::SELECTION::unselectModel(obj->id());
		}
		UI::DOCK::PROPERTIES::updateProperties();
	}
}

void CMainWindow::modelResetTransformations()
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if (nullptr != obj)
	{
		obj->transform().reset();
		UI::DOCK::PROPERTIES::updateProperties();
		updateAllViews();
	}
}

#include <QtWidgets/QMenu>

void CMainWindow::modelLock( bool b )
{
	CModel3D * obj = AP::WORKSPACE::getCurrentModel();
	if ( nullptr != obj )
	{
		if ( obj->setLocked( b ) )
		{
			UI::STATUSBAR::printf(  "Model locked" );
		}
		else
		{
			UI::STATUSBAR::printf(  "Model unlocked" );
		}

		UI::DOCK::WORKSPACE::setItemLockedById(obj->id(), b);
		UI::DOCK::PROPERTIES::updateProperties();
		updateAllViews();
	}
}


void CMainWindow::modelClose()
{
	MdiChild* child = currentMdiChild();

	if ((child != nullptr) && child->hasType(MdiChild::Type::GL))
	{
		GLViewer* view = (GLViewer*) child->m_widget;
		
		if ((view != nullptr) && (view->m_selectionMode == 1))
		{
			view->setSelectionMode(99);
			view->deleteSelectedVertices();

			return;
		}
	}

	CModel3D* obj = AP::WORKSPACE::getCurrentModel();

	if (obj != nullptr)
	{
		if (obj->hasType(CObject::Type::IMAGE))
		{
			if (AP::WORKSPACE::removeImage((CImage*)obj))
				UI::STATUSBAR::printf("Current image has been removed...");
		}
		else
		{
			if (AP::WORKSPACE::removeModel(obj))
				UI::STATUSBAR::printf("Current model has been removed...");
		}
	}
	else
		UI::STATUSBAR::printf("Nothing to remove...");
}

#include "ui_pmDialog.h"

void CMainWindow::pmEcol()
{
	CModel3D *obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue( ((CMesh*) AP::WORKSPACE::getCurrentModel()->getChild())->vertices().size() );

		if ( pmD->exec() )
		{	
			try {
				CModel3D *obj = AP::WORKSPACE::getCurrentModel();
				if ( NULL != obj )
				{
					QFileInfo f( obj->path() );

					if ( pmUi.checkBox->isChecked() )
					{
						obj = obj->getCopy();
						
						QString fname( f.baseName() + "_" + QString::number( pmUi.spinBox->value() ) );
		
						obj->PMeshEcoll( pmUi.spinBox->value() );

						obj->setPath( f.absolutePath() + "/" + fname + "." + f.completeSuffix() );
						
						AP::WORKSPACE::addModel( obj );
					}
					else
					{
						obj->PMeshEcoll( pmUi.spinBox->value() );
					}
				}
			}
			catch ( std::bad_alloc &e ) {
				UI::STATUSBAR::printf( "PMeshEcollSelf(): %s", e.what() );
				UI::MESSAGEBOX::error( "CModel3D::PMeshEcoll() failed with Bad Alloc error." );
			}
			catch ( ... ) {
				UI::STATUSBAR::printf( "Something wrong in CModel3D::PMeshEcoll()" );
			}
								
			updateAllViews();
		}
	}
}

void CMainWindow::pmVsplit()
{
	CModel3D *obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue( ((CMesh*)AP::WORKSPACE::getCurrentModel()->getChild())->vertices().size() );

		if ( pmD->exec() )
		{	
			if ( pmUi.checkBox->isChecked() )
			{
				obj = AP::WORKSPACE::duplicateCurrentModel();
				if (nullptr == obj) return;
			}

			try {
				obj->PMeshVsplit( pmUi.spinBox->value() );
			}
			catch ( std::bad_alloc &e ) {
				UI::STATUSBAR::printf( "PMeshVsplitSelf(): %s", e.what() );
				UI::MESSAGEBOX::error( "CModel3D::PMeshVsplit() failed with Bad Alloc error." );
			}
			catch ( ... ) {
				UI::STATUSBAR::printf( "Something wrong in CModel3D::PMeshVSplit()" );
			}
								
			updateAllViews();
		}
	}				
}

void CMainWindow::grabPoints(bool b)
{
	AP::mainApp().bGlobalPicking = b;
}

void CMainWindow::pickSnap(bool b)
{
	AP::mainApp().bPickSnap = b;
}


#include "ui_aboutDialog.h"

void CMainWindow::helpAbout()
{
	QDialog* about = new QDialog(); // (0, 0);

    Ui_aboutDialog aboutUi;
    aboutUi.setupUi(about);

    about->show();
/*
	QMenu* menu = menuBar()->findChild<QMenu*>( "menuHelp" );
	QList<QAction*>actions = menu->findChildren<QAction*>();

	QList<QMenu*> menus = actions[0]->findChildren<QMenu*>();

	foreach ( QAction* a, actions )
	{
		if ( 0 == a->objectName().compare( "actionAbout" ) )
		{
			a->setObjectName( "ble ble ble" );
		}
	}

	//menu->actions()[0]->setEnabled(false);
*/
}

void CMainWindow::switchConsole(bool b)
{
	static FILE* oldstdout;
	static FILE* oldstderr;
	if (b)
	{
		if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
			oldstdout = freopen("CONOUT$", "w", stdout);
			oldstderr = freopen("CONOUT$", "w", stderr);
		}
	}
	else
	{
		fclose(stdout);
		fclose(stderr);
		FreeConsole();
	}
}


void CMainWindow::userDefinedMenuItemSlot()
{

}