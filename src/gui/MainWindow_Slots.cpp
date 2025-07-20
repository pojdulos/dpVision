#include <QtWidgets/QFileDialog>
#include <QtWidgets/QToolButton>
#include <QtGui/QKeyEvent>
#include <QMessageLogger>

#include "../api/AP.h"

#include "../api/UI.h"

#include "AppSettings.h"
#include "MainApplication.h"
#include "MainWindow.h"

#include "GLViewer.h"
#include "PicViewer.h"
#include "MdiChild.h"

#include "Image.h"
#include <chrono>

#include "FileConnector.h"
#include "adapters/QtProgressAdapter.h"
#include "StatusBarManager.h"

#include <QMessageBox>

void CMainWindow::actionScreenshot() {
	GLViewer* view = this->currentViewer();

	if (view != nullptr)
	{
		view->screenshot();
	}
};

#include "DockWidgetWorkspace.h"

void saveDockGeometry(QDockWidget* dock)
{
	QRect geom = dock->geometry();
	AppSettings::mainSettings()->setValue("gui/mainwindow/" + dock->objectName() + "/width", geom.width());
	AppSettings::mainSettings()->setValue("gui/mainwindow/" + dock->objectName() + "/height", geom.height());
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

void CMainWindow::onWorkspaceObjectActivated(int i)
{
	changeMenuAfterSelect();
	updateView(true, true);
}

void CMainWindow::onWorkspaceObjectActivated(CBaseObject* obj)
{
	changeMenuAfterSelect();
	updateView(true, true);
}

void CMainWindow::onWorkspaceObjectAdded(int i)
{
	changeMenuAfterSelect();
	updateView(true, true);
}

void CMainWindow::onWorkspaceObjectRemoved(int i)
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


void CMainWindow::openRecent() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		if ( nullptr != AP::WORKSPACE::loadModel(action->data().toString().toStdWString(), false, false ))
		{
			adjustForCurrentFile(action->data().toString());
			AppSettings::mainSettings()->setValue("recentFile", action->data().toString());
		}
}


void CMainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), AppSettings::mainSettings()->value("recentFile").toString(), CFileConnector::getLoadExts() );
	
	if (!fileName.isEmpty())
	{
		if ( nullptr != AP::WORKSPACE::loadModel(fileName, false, false, std::make_shared<QtProgressAdapter>(this->progressIndicator) ))
		{
			adjustForCurrentFile(fileName);
			AppSettings::mainSettings()->setValue("recentFile", fileName);
		}
	}
}

void CMainWindow::fileSave()
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		if (obj->hasType(CObject::IMAGE))
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), AP::WORKSPACE::getCurrentModel()->path(), QString("PNG (*.png);;TIFF (*.tif);;JPEG (*.jpg);;Bitmap (*.bmp);;Gif (*.gif)"));

			if (!fileName.isEmpty())
			{
				((CImage*)obj.get())->save( fileName );
			}
		}
		else
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), AP::WORKSPACE::getCurrentModel()->path(), CFileConnector::getSaveExts() );

			if (!fileName.isEmpty())
			{
				if ( obj->save(fileName) )
				{
					if (auto sb = this->statusBar())
					{
						sb->showMessage( "Saved: " + fileName );
					}
					adjustForCurrentFile(fileName);
					AppSettings::mainSettings()->setValue("recentFile", fileName);
				}
			}
		}
	}
	else
	{
		StatusBarManager::setText( "You need to select any object to save..." );
	}
}

#include "Image.h"

void CMainWindow::importImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import image"), AppSettings::mainSettings()->value("recentFile").toString(), QString("2D Images (*.bmp;*.gif;*.jpg;*.png;*.tif;*.tiff)"));

	if (!fileName.isEmpty())
	{
		std::shared_ptr<CImage> im = CImage::load( fileName );
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
		StatusBarManager::setText("Stereoscopy: Off");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyQuadbuff()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_QUADBUFF);
		StatusBarManager::setText("Stereoscopy: Quad buffering");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRowInterlaced()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_INTERLACED);
		StatusBarManager::setText("Stereoscopy: Row interlaced");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyColInterlaced()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_COLUMNINTERLACED);
		StatusBarManager::setText("Stereoscopy: Column interlaced");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyAboveBelow()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_ABOVEBELOW);
		StatusBarManager::setText("Stereoscopy: Above/Below");
		updateActiveView();
	}
}

void CMainWindow::stereoscopySideBySide()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_SIDEBYSIDE);
		StatusBarManager::setText("Stereoscopy: Side by side");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedCyan()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDCYAN);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Cyan");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedBlue()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDBLUE);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Blue");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyRedGreen()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_REDGREEN);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Green");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyCyanRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_CYANRED);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Cyan");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyBlueRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_BLUERED);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Blue");
		updateActiveView();
	}
}

void CMainWindow::stereoscopyGreenRed()
{
	GLViewer *view = this->currentViewer();
	if (NULL != view)
	{
		view->setStereoMode(GLViewer::StereoType::STEREO_GREENRED);
		StatusBarManager::setText("Stereoscopy: Anaglyph Red-Green");
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

			StatusBarManager::setText("Shaders On");
		}
		else
		{
			view->useShaders(false);

			StatusBarManager::setText("Shaders Off");
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
		StatusBarManager::setText("Orthogonal projection");
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
		StatusBarManager::setText("Perspective projection");
		updateActiveView();
	}
	UI::DOCK::PROPERTIES::updateProperties();
}


void CMainWindow::modelVisibility( bool vis )
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
	if ( nullptr != obj )
	{
		if ( vis )
		{
			obj->setSelfVisibility( true );
			StatusBarManager::setText(  "Model visibility: Show" );
		}
		else
		{
			obj->setSelfVisibility( false );
			StatusBarManager::setText(  "Model visibility: Hide" );
		}
		
		UI::DOCK::WORKSPACE::setItemVisibleById(obj->id(), vis);
		UI::DOCK::PROPERTIES::updateProperties();
		updateAllViews();
	}
}

void CMainWindow::modelInvertNormals()
{
	if (std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel())
	{
		if (auto m = std::dynamic_pointer_cast<CMesh>(obj->getChild()))
		{
			m->invertNormals();
			updateAllViews();
			StatusBarManager::setText("Normalne ścian zostały odwrócone");
		}
		else
		{
			StatusBarManager::setText("Obiekt nie jest siatką...");
		}
	}
}

void CMainWindow::meshApplyTransformations()
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
	if (NULL != obj)
	{
		if ( auto o = std::dynamic_pointer_cast<CObject>(obj->getChild()) )
		{
			if ((o->type() == CObject::MESH) || (o->type() == CObject::CLOUD ) || (o->type() == CObject::ORDEREDCLOUD) )
			{
				AP::WORKSPACE::getCurrentModel()->applyTransform(CTransform());

				//CTransform trans = AP::WORKSPACE::getCurrentModel()->getTransform();
				//((CMesh*)AP::WORKSPACE::getCurrentModel()->getChild())->applyTransformation( trans, CTransform() );
				//AP::WORKSPACE::getCurrentModel()->setTransform(CTransform());

				StatusBarManager::setText("Współrzędne obiektu zostały przekształcone");

				updateAllViews();
			}
			else
			{
				StatusBarManager::setText("Obiekt nie jest chmurą punktów lub siatką...");
			}
		}
		else
		{
			StatusBarManager::setText("Obiekt nie jest chmurą punktów lub siatką...");
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
		StatusBarManager::setText(  "Rendering: Faces" );
		
		updateAllViews();
	}
}

void CMainWindow::renderAsEdges()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff );
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOn );
		StatusBarManager::setText(  "Rendering: Edges" );

		updateAllViews();
	}
}

void CMainWindow::renderAsVertices()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff );
		AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOn );
		StatusBarManager::setText( "Rendering: Vertices" );
		
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
			StatusBarManager::setText(  "Texture: on" );
		}
		else
		{
			//ModifyMenu(GetMenu( hWnd ), IDM_MODEL_TEXTUREONOFF, MF_BYCOMMAND|MF_STRING, IDM_MODEL_TEXTUREONOFF, GetResourceString( IDS_MODEL_TEXTUREON ).c_str() ); 
			StatusBarManager::setText(  "Texture: off" );
		}

		updateAllViews();
	}
}

void CMainWindow::smoothingOnOff()
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->calcVN();
		//StatusBarManager::setText( AP::WORKSPACE::getCurrentModel()->GetMeshInfoText().c_str() );

		StatusBarManager::setText( AP::WORKSPACE::getCurrentModel()->switchOption( CModel3D::Opt::optSmoothVertices, CModel3D::Switch::switchToggle ) ? "Wygladzanie wierzcholkow: wlaczone" : "Wygladzanie wierzcholkow: wylaczone" );
	}
	else
		StatusBarManager::setText( "None objects selected" );

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
			updateAllViews();
		}
}


void CMainWindow::actionLookDir(int direction, std::shared_ptr<CModel3D> obj)
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
		auto current = obj;

		if (current->getBoundingBox().isInvalid()) return;

		Eigen::Matrix4d MT = CBaseObject::getGlobalTransformationMatrix(current);

		for (CPoint3d& corner : current->getCorners())
		{
			corner = MT * corner;
			bb.expand(corner);
		}
	}
	else
	{
		bb = AP::WORKSPACE::instance()->topBB();

		if (bb.isInvalid()) return;
	}


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


	double minX = DBL_MAX;
	double maxX = -DBL_MAX;
	double minY = DBL_MAX;
	double maxY = -DBL_MAX;

	Eigen::Matrix4d MT = view->transform().toEigenMatrix4d();

	for (CPoint3d& corner : bb.getCorners())
	{
		corner = MT * corner;
		corner = corner - view->camPos();
		corner = view->world2win(corner);

		minX = corner.x < minX ? corner.x : minX;
		minY = corner.y < minY ? corner.y : minY;
		maxX = corner.x > maxX ? corner.x : maxX;
		maxY = corner.y > maxY ? corner.y : maxY;
	}

	QRect r = view->rect();
	qInfo() << r << QVector<int>({r.left(),r.top(),r.right(),r.bottom()});
	qInfo() << QVector<double>({ minX, minY, maxX, maxY });


	CTransform T;

	while ((minX > r.left()) && (minY > r.top()) && (maxX < r.right()) && (maxY < r.bottom()))
	{
		view->transform().translate(CVector3d(0.0, 0.0, 1.0) );

		T = view->transform();

		minX = DBL_MAX;
		maxX = -DBL_MAX;
		minY = DBL_MAX;
		maxY = -DBL_MAX;

		for (CPoint3d& corner : bb.getCorners())
		{
			corner = T * corner;
			corner = corner - view->camPos();
			corner = view->world2win(corner);

			minX = corner.x < minX ? corner.x : minX;
			minY = corner.y < minY ? corner.y : minY;
			maxX = corner.x > maxX ? corner.x : maxX;
			maxY = corner.y > maxY ? corner.y : maxY;
		}

		qInfo() << view->rect() << QVector<double>({ minX, minY, maxX, maxY });
	}

	while ((minX < r.left()) || (minY < r.top()) || (maxX > r.right()) || (maxY > r.bottom()))
	{
		view->transform().translate(CVector3d(0.0, 0.0, -1.0));

		T = view->transform();

		minX = DBL_MAX;
		maxX = -DBL_MAX;
		minY = DBL_MAX;
		maxY = -DBL_MAX;

		for (CPoint3d& corner : bb.getCorners())
		{
			corner = T * corner;
			corner = corner - view->camPos();
			corner = view->world2win(corner);

			minX = corner.x < minX ? corner.x : minX;
			minY = corner.y < minY ? corner.y : minY;
			maxX = corner.x > maxX ? corner.x : maxX;
			maxY = corner.y > maxY ? corner.y : maxY;
		}

		qInfo() << view->rect() << QVector<double>({ minX, minY, maxX, maxY });
	}

	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
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
	StatusBarManager::setText("Select Vertex");
	currentViewer()->setSelectionMode( 1 );
	ui.action_select_selection_delete->setEnabled(true);
	ui.action_select_noselection_delete->setEnabled(true);
	updateActiveView();
}

void CMainWindow::actionSelectFace()
{
	StatusBarManager::setText("Select Face");
	currentViewer()->setSelectionMode( 2 );
	ui.action_select_selection_delete->setEnabled(true);
	ui.action_select_noselection_delete->setEnabled(true);
	updateActiveView();
}

void CMainWindow::actionSelectNone()
{
	StatusBarManager::setText("Select None");
	currentViewer()->setSelectionMode( 0 );
	ui.action_select_selection_delete->setEnabled(false);
	ui.action_select_noselection_delete->setEnabled(false);
	updateActiveView();
}

#include "PicViewer.h"

void CMainWindow::imageFit(bool fit)
{
	std::shared_ptr<CModel3D> im = AP::WORKSPACE::getCurrentModel();
	if ((im != nullptr) && im->hasType(CObject::IMAGE))
	{
		((CImage*)im.get())->fitToWindow = fit;
	}

	QMdiSubWindow* window = getPicViewerInstance(im->id());
	if (window != nullptr)
	{
		((PicViewer*)((MdiChild*)window->widget())->m_widget)->reloadImage();
	}
}

//void CMainWindow::bbShowHide(bool show)
//{
//	if ( NULL != AP::WORKSPACE::getCurrentModel() )
//	{
//		while ( show != AP::WORKSPACE::getCurrentModel()->toggleDrawBB() ) {};
//
//		if ( show )
//		{
//			StatusBarManager::setText(  "Bounding box: on" );
//		}
//		else
//		{
//			StatusBarManager::setText(  "Bounding box: off" );
//		}
//
//		updateAllViews();
//	}
//	else
//	{
//		GLViewer *view = this->currentViewer();
//		if (NULL != view)
//		{
//			view->toggleAxesVisibility();
//
//			updateActiveView();
//		}
//	}
//}

#include "DockWidgetWorkspace.h"

void CMainWindow::bbShowHide(bool show)
{
	QVector<std::shared_ptr<CBaseObject>> objts = this->dockWorkspace->getSelectedObjects();

	if (objts.isEmpty()) {
		GLViewer* view = this->currentViewer();
		if (NULL != view)
		{
			view->toggleAxesVisibility();

			updateActiveView();
		}
	}
	else {
		for (auto obj : objts) {
			if (obj->hasCategory(CBaseObject::OBJECT)) {
				if (!obj->hasType(CObject::MODEL) && !obj->hasType(CObject::MOVEMENT)) {
					std::shared_ptr<CObject> m = std::static_pointer_cast<CObject>(obj); 
					
					m->toggleDrawBB();
										
					//qInfo() << obj->getLabel() << QString(", BBox: %1 %2 %3 - %4 %5 %6")
					//	.arg(m->getMin().x).arg(m->getMin().y).arg(m->getMin().z)
					//	.arg(m->getMax().x).arg(m->getMax().y).arg(m->getMax().z);
				}
			}
		}
		updateAllViews();
	}
}


void CMainWindow::openWorkspace()
{
	//QString path = QFileDialog::getOpenFileName(this, tr("Open Workspace"), "", tr("dpVision multiarchive file (*.dpvision);;Faro scene (*.Faro)"));
	QString path = QFileDialog::getOpenFileName(this, tr("Open Workspace"), "", tr("All workspace files (*.dpw;*.atmdl;*.dpvision);;Digital patient workspace (*.dpw;*.atmdl);;dpVision multiarchive file (*.dpvision)"));

	if (path.isEmpty()) return;

	CParser* parser = CFileConnector::getLoadParser(path);

	if (parser == nullptr) return;

	int reply = QMessageBox::question(0,"You are about to removing all models in your workspace.\nRealy you want to do it?", "Caution!");
	
	if (0 == reply)
	{
		AP::WORKSPACE::removeAllModels();
		//unsigned long t1, t2;

		//t1 = GetTickCount();
		auto t1 = std::chrono::steady_clock::now();

		//CModel3D* obj = parser->load(path, true);
		AP::WORKSPACE::loadModel(path, true);

		//t2 = GetTickCount();
		auto t2 = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();

		qInfo() << "Load time: " << duration;

	}
	
	if (!parser->inPlugin()) delete parser;

	return;
}

void CMainWindow::saveWorkspace()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save Workspace"),"",tr("Digital patient workspace (*.dpw);;dpVision multiarchive file (*.dpvision)"));
	
	if (path.isEmpty()) return;

	CParser* parser = CFileConnector::getSaveParser(path);

	if (parser == nullptr) return;

	QVector<std::shared_ptr<CBaseObject>> objects;

	for (const auto& o : AP::WORKSPACE::instance()->children())
	{
		objects << o.second;
	}

	bool result = parser->save(objects, path);

	if (!parser->inPlugin()) delete parser;

	if (result)
	{
		adjustForCurrentFile(path);
		AppSettings::mainSettings()->setValue("recentFile", path);
	}

	return;
}

void CMainWindow::removeAllModels()
{
	int reply = QMessageBox::question(0, "You are about to removing all models in your workspace.\nRealy you want to do it?", "Caution!");
	if ( 0 == reply ) AP::WORKSPACE::removeAllModels();
}

void CMainWindow::removeSelectedModels()
{
	int reply = QMessageBox::question(0, "You are about to removing all selected (checked) models.\nRealy you want to do it?", "Caution!");
	if (0 == reply) AP::WORKSPACE::removeSelectedModels();
}

void CMainWindow::resetAllTransformations()
{
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->transform().reset();
	}
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::resetSelectedTransformations()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		AP::WORKSPACE::getModel(*it)->transform().reset();
	}
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::lockAllModels()
{
	//UI::MESSAGEBOX::information(L"Lock ALL");
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->setLocked(true);
		UI::DOCK::WORKSPACE::setItemLockedById(it->first, true);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::lockSelectedModels()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getModel(*it);
		obj->setLocked(true);
		UI::DOCK::WORKSPACE::setItemLockedById(*it, true);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::unlockAllModels()
{
	//UI::MESSAGEBOX::information(L"Lock ALL");
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		it->second->setLocked(false);
		UI::DOCK::WORKSPACE::setItemLockedById(it->first, false);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::unlockSelectedModels()
{
	std::list<int> sel = AP::getWorkspace()->getSelection();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getModel(*it);
		obj->setLocked(false);
		UI::DOCK::WORKSPACE::setItemLockedById(*it, false);
	}
	UI::changeMenuAfterSelect();
	UI::DOCK::PROPERTIES::updateProperties();
	updateAllViews();
}

void CMainWindow::selectAll()
{
	AP::getWorkspace()->clearSelection();
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
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
	//updateAllViews();
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
	//updateAllViews();
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
	//updateAllViews();
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
	//updateAllViews();
}

void CMainWindow::modelInSelection(bool b)
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
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
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
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
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
	if ( nullptr != obj )
	{
		if ( obj->setLocked( b ) )
		{
			StatusBarManager::setText(  "Model locked" );
		}
		else
		{
			StatusBarManager::setText(  "Model unlocked" );
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

	auto wksp = CWorkspace::instance();

	auto obj = AP::WORKSPACE::getCurrentModel();

	if (obj != nullptr)
	{
		if (auto im = std::dynamic_pointer_cast<CImage>(obj))
		{
			if (wksp->_objectRemove(im))
				StatusBarManager::setText("Current image has been removed...");
		}
		else
		{
			if (wksp->_objectRemove(obj))
				StatusBarManager::setText("Current model has been removed...");
		}
	}
	else
		StatusBarManager::setText("Nothing to remove...");
}

#include "ui_pmDialog.h"

void CMainWindow::pmEcol()
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue(std::dynamic_pointer_cast<CMesh>(AP::WORKSPACE::getCurrentModel()->getChild())->vertices().size());

		if ( pmD->exec() )
		{	
			try {
				std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();
				if ( NULL != obj )
				{
					QFileInfo f( obj->path() );

					if ( pmUi.checkBox->isChecked() )
					{
						obj = std::dynamic_pointer_cast<CModel3D>(obj->getCopy());
						
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
				StatusBarManager::setText( QString("PMeshEcollSelf(): %1").arg(e.what()));
				QMessageBox::critical( 0, "CModel3D::PMeshEcoll() failed with Bad Alloc error." , "");
			}
			catch ( ... ) {
				StatusBarManager::setText( "Something wrong in CModel3D::PMeshEcoll()" );
			}
								
			updateAllViews();
		}
	}
}

void CMainWindow::pmVsplit()
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue(std::dynamic_pointer_cast<CMesh>(AP::WORKSPACE::getCurrentModel()->getChild())->vertices().size());

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
				StatusBarManager::setText( QString("PMeshVsplitSelf(): %1").arg(e.what()));
				QMessageBox::critical( 0, "CModel3D::PMeshVsplit() failed with Bad Alloc error.", "" );
			}
			catch ( ... ) {
				StatusBarManager::setText( "Something wrong in CModel3D::PMeshVSplit()" );
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
#ifdef _WIN32
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
#endif
}


void CMainWindow::userDefinedMenuItemSlot()
{

}

#include "SettingsDialog.h"

void CMainWindow::openSettingsDialog()
{
	if (!settingsDialog)
		settingsDialog = new SettingsDialog(this);
	settingsDialog->exec();
	//settingsDialog->show();
	//settingsDialog->raise();
	//settingsDialog->activateWindow();
}