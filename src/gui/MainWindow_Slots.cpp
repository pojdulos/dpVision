#include <QtWidgets/QFileDialog>
#include <QtWidgets/QToolButton>
#include <QtGui/QKeyEvent>
#include <QMessageLogger>

//#include "../api/AP.h"

//

#include "AppSettings.h"
#include "MainApplication.h"
#include "MainWindow.h"

#include "GLViewer.h"
#include "DockWidgetProperties.h"
#include "MdiChild.h"
#include "PicViewer.h"
#include "ImageViewerHost.h"
#include "ImageViewerState.h"

#include "Image.h"
#include <chrono>

#include "FileConnector.h"
#include "adapters/QtProgressAdapter.h"
#include "StatusBarManager.h"
#include "WorkspaceImageHost.h"
#include "WorkspaceImportHost.h"
#include "../core/AppStateManager.h"
#include "../core/Workspace.h"
#include "../core/PluginRuntimeManager.h"

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
	if (e->key() == Qt::Key_Alt) //:Key_Control)
	{
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
	}
}

void CMainWindow::keyReleaseEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Alt) //:Key_Control)
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
			CWorkspace::instance()->_objectActivate(((PicViewer*)child->m_widget)->id());
		}
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

void CMainWindow::onWorkspaceObjectStateChanged(int i)
{
	Q_UNUSED(i);
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

void CMainWindow::onWorkspaceStructureChanged()
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
		if ( nullptr != WorkspaceImportHost::loadModel(action->data().toString(), false, false) )
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
		if ( nullptr != WorkspaceImportHost::loadModel(fileName, false, false, std::make_shared<QtProgressAdapter>(this->progressIndicator)) )
		{
			adjustForCurrentFile(fileName);
			AppSettings::mainSettings()->setValue("recentFile", fileName);
		}
	}
}

void CMainWindow::fileSave()
{
	CWorkspace* wksp = CWorkspace::instance();
	std::shared_ptr<CModel3D> obj = wksp->_getModel(wksp->_getCurrentModelId());

	if ( NULL != obj )
	{
		if (obj->hasType(CObject::IMAGE))
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), obj->path(), QString("PNG (*.png);;TIFF (*.tif);;JPEG (*.jpg);;Bitmap (*.bmp);;Gif (*.gif)"));

			if (!fileName.isEmpty())
			{
				((CImage*)obj.get())->save( fileName );
			}
		}
		else
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), obj->path(), CFileConnector::getSaveExts() );

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
		
		StatusBarManager::setText("Loading image...");
		std::shared_ptr<CImage> im = CImage::load( fileName );
		if (nullptr != im)
		{
			WorkspaceImageHost::addImage(im, true);
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
	dockProperties->updateProperties();
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
	dockProperties->updateProperties();
}


void CMainWindow::modelVisibility( bool vis )
{
	CWorkspace* wksp = CWorkspace::instance();
	std::shared_ptr<CModel3D> obj = wksp->_getModel(wksp->_getCurrentModelId());
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
		
		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

void CMainWindow::modelInvertNormals()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		if (auto m = std::dynamic_pointer_cast<CMesh>(obj->getChild()))
		{
			m->invertNormals();
			CWorkspace::instance()->notifyObjectStateChanged(obj->id());
			StatusBarManager::setText("Normalne scian zostaly odwrocone");
		}
		else
		{
			StatusBarManager::setText("Obiekt nie jest siatka…...");
		}
	}
}

void CMainWindow::meshApplyTransformations()
{
	CWorkspace* wksp = CWorkspace::instance();
	std::shared_ptr<CModel3D> obj = wksp->_getModel(wksp->_getCurrentModelId());
	if (NULL != obj)
	{
		if ( auto o = std::dynamic_pointer_cast<CObject>(obj->getChild()) )
		{
			if ((o->type() == CObject::MESH) || (o->type() == CObject::CLOUD ) || (o->type() == CObject::ORDEREDCLOUD) )
			{
				obj->applyTransform(CTransform());



				StatusBarManager::setText("WspĂłĹ‚rzÄ™dne obiektu zostaĹ‚y przeksztaĹ‚cone");

				CWorkspace::instance()->notifyObjectStateChanged(obj->id());
			}
			else
			{
				StatusBarManager::setText("Obiekt nie jest chmurÄ… punktĂłw lub siatkÄ…...");
			}
		}
		else
		{
			StatusBarManager::setText("Obiekt nie jest chmurÄ… punktĂłw lub siatkÄ…...");
		}

	}
}

void CMainWindow::renderAsFaces()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		obj->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff );
		obj->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff );
		StatusBarManager::setText(  "Rendering: Faces" );
		
		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

void CMainWindow::renderAsEdges()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		obj->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOff );
		obj->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOn );
		StatusBarManager::setText(  "Rendering: Edges" );

		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

void CMainWindow::renderAsVertices()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		obj->switchOption( CModel3D::Opt::optRenderAsEdges, CModel3D::Switch::switchOff );
		obj->switchOption( CModel3D::Opt::optRenderAsPoints, CModel3D::Switch::switchOn );
		StatusBarManager::setText( "Rendering: Vertices" );
		
		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

void CMainWindow::textureOnOff()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		if ( obj->switchOption( CModel3D::Opt::optRenderWithTexture, CModel3D::Switch::switchToggle ) )
		{
			//ModifyMenu(GetMenu( hWnd ), IDM_MODEL_TEXTUREONOFF, MF_BYCOMMAND|MF_STRING, IDM_MODEL_TEXTUREONOFF, GetResourceString( IDS_MODEL_TEXTUREOFF ).c_str() ); 
			StatusBarManager::setText(  "Texture: on" );
		}
		else
		{
			//ModifyMenu(GetMenu( hWnd ), IDM_MODEL_TEXTUREONOFF, MF_BYCOMMAND|MF_STRING, IDM_MODEL_TEXTUREONOFF, GetResourceString( IDS_MODEL_TEXTUREON ).c_str() ); 
			StatusBarManager::setText(  "Texture: off" );
		}

		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

void CMainWindow::smoothingOnOff()
{
	if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()))
	{
		obj->calcVN();
		StatusBarManager::setText( obj->switchOption( CModel3D::Opt::optSmoothVertices, CModel3D::Switch::switchToggle ) ? "Wygladzanie wierzcholkow: wlaczone" : "Wygladzanie wierzcholkow: wylaczone" );
		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
	else
		StatusBarManager::setText( "None objects selected" );
}

void CMainWindow::createNewCopy()
{
	CWorkspace::instance()->duplicateCurrentModel();
}

void CMainWindow::cameraResetPosition()
{
		GLViewer *view = this->currentViewer();
		if (nullptr != view)
		{
			view->resetGeometry();
			dockProperties->updateProperties();
			updateAllViews();
		}
}

double computeCameraDistance(CBoundingBox& bb, double fovY_deg,	double aspect)
{
	if (bb.isInvalid()) return 1.0;

	double fovY = fovY_deg * M_PI / 180.0;
	double fovX = 2.0 * atan(tan(fovY / 2.0) * aspect);

	CPoint3d min = bb.getMin();
	CPoint3d max = bb.getMax();

	double halfWidth = (max.x - min.x) * 0.5;
	double halfHeight = (max.y - min.y) * 0.5;
	double halfDepth = (max.z - min.z) * 0.5;

	// odlegĹ‚oĹ›Ä‡ wymagana w pionie i w poziomie
	double dY = halfHeight / tan(fovY / 2.0);
	double dX = halfWidth / tan(fovX / 2.0);

	double dist = std::max(dX, dY);

	// dodajemy gĹ‚Ä™bokoĹ›Ä‡, ĹĽeby box siÄ™ mieĹ›ciĹ‚ w Z
	dist += halfDepth;

	return dist;
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

	bool b_center = true;
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
		bb = CWorkspace::instance()->topBB();

		if (bb.isInvalid()) return;
	}



	QRect r = view->rect();
	double aspect = double(r.width()) / double(r.height());
	double fovY = view->getVAngle(); //getFovY(); // np. 45 stopni


	Eigen::Matrix4d R = view->transform().toEigenMatrix4d(); // zawiera juĹĽ obrĂłt

	CPoint3d center = bb.getMidpoint();
	CPoint3d rotatedCenter = R * center;

	// teraz centrowanie
	view->transform().translate(CVector3d(-rotatedCenter.x,
		-rotatedCenter.y,
		0.0));

	// obliczenie zoomu
	double dist = computeCameraDistance(bb, fovY, aspect);
	double targetZ = 200.0 - dist;

	// przesuniÄ™cie w Z
	view->transform().translate(CVector3d(0.0, 0.0, targetZ - rotatedCenter.z));

	dockProperties->updateProperties();
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

	actionLookDir(dir, CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId()));
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

void CMainWindow::imageFit(bool fit)
{
	std::shared_ptr<CModel3D> im = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());
	if ((im != nullptr) && im->hasType(CObject::IMAGE))
	{
		ImageViewerHost::setFitToWindow(im->id(), fit);
	}
}



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
		CWorkspace::instance()->removeAll();
		//unsigned long t1, t2;

		//t1 = GetTickCount();
		auto t1 = std::chrono::steady_clock::now();

		//CModel3D* obj = parser->load(path, true);
		WorkspaceImportHost::loadModel(path, true);

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

	for (const auto& o : CWorkspace::instance()->children())
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
	if ( 0 == reply ) CWorkspace::instance()->removeAll();
}

void CMainWindow::removeSelectedModels()
{
	int reply = QMessageBox::question(0, "You are about to removing all selected (checked) models.\nRealy you want to do it?", "Caution!");
	if (0 == reply) CWorkspace::instance()->removeChecked();
}

void CMainWindow::resetAllTransformations()
{
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = CWorkspace::instance()->begin(); it != CWorkspace::instance()->end(); it++)
	{
		it->second->transform().reset();
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::resetSelectedTransformations()
{
	std::list<int> sel = CWorkspace::instance()->checkedIds();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(*it))
		{
			obj->transform().reset();
		}
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::lockAllModels()
{
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = CWorkspace::instance()->begin(); it != CWorkspace::instance()->end(); it++)
	{
		it->second->setLocked(true);
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::lockSelectedModels()
{
	std::list<int> sel = CWorkspace::instance()->checkedIds();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(*it))
		{
			obj->setLocked(true);
		}
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::unlockAllModels()
{
	for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = CWorkspace::instance()->begin(); it != CWorkspace::instance()->end(); it++)
	{
		it->second->setLocked(false);
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::unlockSelectedModels()
{
	std::list<int> sel = CWorkspace::instance()->checkedIds();
	for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
	{
		if (std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(*it))
		{
			obj->setLocked(false);
		}
	}
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::selectAll()
{
	CWorkspace::instance()->checkAll();
}

void CMainWindow::unselectAll()
{
	CWorkspace::instance()->clearChecked();
	CWorkspace::instance()->notifyStructureChanged();
}

void CMainWindow::hideAllModels()
{
	CWorkspace::instance()->setAllVisible(false);
}

void CMainWindow::hideSelectedModels()
{
	CWorkspace::instance()->setCheckedVisible(false);
}

void CMainWindow::showAllModels()
{
	CWorkspace::instance()->setAllVisible(true);
}

void CMainWindow::showSelectedModels()
{
	CWorkspace::instance()->setCheckedVisible(true);
}

void CMainWindow::modelInSelection(bool b)
{
	std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());
	if (nullptr != obj)
	{
		CWorkspace::instance()->setChecked(obj->id(), b);
	}
}

void CMainWindow::modelResetTransformations()
{
	std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());
	if (nullptr != obj)
	{
		obj->transform().reset();
		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
	}
}

#include <QtWidgets/QMenu>

void CMainWindow::modelLock( bool b )
{
	std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());
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

		CWorkspace::instance()->notifyObjectStateChanged(obj->id());
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
	auto obj = wksp->_getModel(wksp->_getCurrentModelId());

	if (obj != nullptr)
	{
		if (auto im = std::dynamic_pointer_cast<CImage>(obj))
		{
			if (wksp->_objectRemove(im))
				StatusBarManager::setText("Current image has been removed...");
		}
		else
		{
			if (wksp->removeCurrent())
				StatusBarManager::setText("Current model has been removed...");
		}
	}
	else
		StatusBarManager::setText("Nothing to remove...");
}

#include "ui_pmDialog.h"

void CMainWindow::pmEcol()
{
	std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue(std::dynamic_pointer_cast<CMesh>(obj->getChild())->vertices().size());

		if ( pmD->exec() )
		{	
			try {
				std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());
				if ( NULL != obj )
				{
					QFileInfo f( obj->path() );

					if ( pmUi.checkBox->isChecked() )
					{
						obj = std::dynamic_pointer_cast<CModel3D>(obj->getCopy());
						
						QString fname( f.baseName() + "_" + QString::number( pmUi.spinBox->value() ) );
		
						obj->PMeshEcoll( pmUi.spinBox->value() );

						obj->setPath( f.absolutePath() + "/" + fname + "." + f.completeSuffix() );
						
						CWorkspace::instance()->_objectAdd(obj);
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
	std::shared_ptr<CModel3D> obj = CWorkspace::instance()->_getModel(CWorkspace::instance()->_getCurrentModelId());

	if ( NULL != obj )
	{
		QDialog* pmD = new QDialog(); // (0, 0);

		Ui_pmDialog pmUi;
		pmUi.setupUi(pmD);

		pmUi.spinBox->setValue(std::dynamic_pointer_cast<CMesh>(obj->getChild())->vertices().size());

		if ( pmD->exec() )
		{	
			if ( pmUi.checkBox->isChecked() )
			{
				obj = CWorkspace::instance()->duplicateCurrentModel();
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
	PluginRuntimeManager::setGlobalPickingEnabled(b);
}

void CMainWindow::pickSnap(bool b)
{
	PluginRuntimeManager::setPickSnapEnabled(b);
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
