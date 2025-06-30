#include "MainWindow.h"

#include "AP.h"
#include "MainApplication.h"
#include "Model3D.h"

//#include <QtWidgets/QMainWindow>
//#include <QtWidgets/QProgressBar>

#include <QtCore/QFileInfo>

#include <QtWidgets>
#include <QMdiSubWindow>
#include <QRect>

#include "DockWidgetWorkspace.h"
#include "DockWidgetProperties.h"
#include "DockWidgetLights.h"
#include "DockWidgetPluginList.h"
#include "DockWidgetPluginPanel.h"
#include "DockWidgetHistogram.h"
#include "DockWidgetImageViewer.h"

#include "ProgressIndicator.h"

#include "GLViewer.h"
#include "PicViewer.h"
#include "MdiChild.h"
#include "Image.h"
#include "AppSettings.h"


void restoreDockGeometry(QDockWidget* dock)
{
	int width = AppSettings::mainSettings()->value("gui/mainwindow/" + dock->objectName() + "/width", 200).toInt();
	int height = AppSettings::mainSettings()->value("gui/mainwindow/" + dock->objectName() + "/height", 200).toInt();
	dock->resize(width, height);
}

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	//// Usuniêcie domyœlnego paska tytu³owego
	//setWindowFlags(Qt::FramelessWindowHint);

	//// Tworzenie niestandardowego TitleBar
	//TitleBar* titleBar = new TitleBar(this);
	//titleBar->setGeometry(0, 0, width(), 30);
	//titleBar->raise();
	//titleBar->show();

	//// Widget kontenerowy dla ca³ego uk³adu
	//QWidget* container = new QWidget();

	//// Nowy uk³ad g³ówny okna
	//QVBoxLayout* mainLayout = new QVBoxLayout();
	//mainLayout->setMargin(0);
	//mainLayout->setSpacing(0);

	//// Dodanie TitleBar na samej górze
	//mainLayout->addWidget(titleBar);

	//// Dodanie centralnego widgetu z Qt Designer
	//mainLayout->addWidget(ui.centralWidget);

	//// Ustawienie nowego uk³adu jako uk³ad g³ówny
	//container->setLayout(mainLayout);
	//setCentralWidget(container);


	ui.menuModel->menuAction()->setVisible(false);
	ui.menuImage->menuAction()->setVisible(false);
	ui.action_File_SaveAs->setEnabled( false );


	settingsDialog = nullptr;

	//this->toolbar = new QToolBar("toolbar",this);
	//this->addToolBar(toolbar);
	//this->toolbar->setVisible(true);

	//-------------------------------------------------------
	// Dolny dock
	this->dockHisto = new DockWidgetHistogram(this);
	this->addDockWidget(Qt::BottomDockWidgetArea, dockHisto);
	this->dockHisto->setVisible(false);

	//-------------------------------------------------------
	this->dockWorkspace = new DockWidgetWorkspace(this);
	this->dockProperties = new DockWidgetProperties(this);
	this->dockPluginList = new DockWidgetPluginList(this);
	this->dockPluginPanel = new DockWidgetPluginPanel(this);
	this->dockLights = new DockWidgetLights(this);
	this->dockImage = new DockWidgetImageViewer(this);

	this->dockLights->setVisible(false);
	this->dockImage->setVisible(false);

	//-------------------------------------------------------
	// Lewy dock
	this->addDockWidget(Qt::BottomDockWidgetArea, dockWorkspace);
	this->addDockWidget(Qt::LeftDockWidgetArea, dockProperties);
	this->addDockWidget(Qt::LeftDockWidgetArea, dockLights);
	this->tabifyDockWidget(dockProperties, dockLights);
	this->addDockWidget(Qt::LeftDockWidgetArea, dockImage);

	//-------------------------------------------------------
	// Prawy dock
	this->addDockWidget(Qt::RightDockWidgetArea, dockPluginList);
	this->addDockWidget(Qt::RightDockWidgetArea, dockPluginPanel);
	//-------------------------------------------------------

	//this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	//this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	this->progressIndicator = new ProgressIndicator(this->statusBar());
	//this->progressIndicator->setMaximumHeight(16);
	//this->progressIndicator->setMaximumWidth(200);
	//this->progressIndicator->setTextVisible(true);
	this->progressIndicator->hide();
	this->statusBar()->addPermanentWidget(progressIndicator, 0);

	this->dockWorkspace->rebuildTree();
	this->dockWorkspace->selectItem(NO_CURRENT_MODEL); // -1 = ¿aden nie zaznaczony

	createRecentActions();
	createRecentMenus();

	createSelectMenu();

	MdiChild::create(MdiChild::Type::GL, ui.mdiArea, MdiChild::Show::Maximized);

	connect(CWorkspace::instance(), SIGNAL(currentObjectChanged(int)), this, SLOT(onCurrentObjectChanged(int)));
	connect(CWorkspace::instance(), SIGNAL(currentObjectChanged(int)), dockWorkspace, SLOT(onCurrentObjectChanged(int)));
	connect(CWorkspace::instance(), SIGNAL(currentObjectChanged(int)), dockProperties, SLOT(onCurrentObjectChanged(int)));

	connect(dockWorkspace, SIGNAL(currentObjectChanged(int)), CWorkspace::instance(), SLOT(onCurrentObjectChanged(int)));
	connect(dockWorkspace, SIGNAL(currentObjectChanged(int)), this, SLOT(onCurrentObjectChanged(int)));

	connect(dockWorkspace, SIGNAL(currentObjectChanged(int)), dockProperties, SLOT(onCurrentObjectChanged(int)));

	connect(ui.mdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow* window) {
		if (window != nullptr) {
			lastActiveWindow = window;  // Przechowuj wskaŸnik jako QPointer
		}
		});
}

CMainWindow::~CMainWindow() {}


CMainWindow* CMainWindow::instance()
{
	static CMainWindow* m_instance;
	if (m_instance == NULL) {
		m_instance = new CMainWindow(); // don't delete it at end !!!
		// deleteLater() is called in closeEvent()

		//m_instance->setWindowFlags(Qt::FramelessWindowHint);
	}
	return m_instance;
}

void CMainWindow::closeEvent(QCloseEvent* event)
{
	AppSettings::mainSettings()->setValue("gui/mainwindow/dockState", this->saveState());
	AppSettings::mainSettings()->setValue("gui/mainwindow/geometry", this->saveGeometry());

	deleteLater();
	QMainWindow::closeEvent(event);
}

void CMainWindow::showEvent(QShowEvent* ev)
{
	static bool isInitialized = false;

	static bool firstShow = true;
	if (firstShow) {
		this->restoreGeometry(AppSettings::mainSettings()->value("gui/mainwindow/geometry").toByteArray());

		QByteArray dockState = AppSettings::mainSettings()->value("gui/mainwindow/dockState").toByteArray();
		if (dockState.isEmpty()) {
			dockWorkspace->setFixedHeight(500);
			AppSettings::mainSettings()->setValue("gui/mainwindow/dockState", this->saveState());
			dockWorkspace->setMinimumHeight(0);
			dockWorkspace->setMaximumHeight(QWIDGETSIZE_MAX);
			dockState = AppSettings::mainSettings()->value("gui/mainwindow/dockState").toByteArray();
		}
		this->restoreState(dockState);

		firstShow = false;
	}

	QMainWindow::showEvent(ev);

	if (ev->spontaneous())
		return;

	if (isInitialized)
		return;

	dockProperties->selectionChanged(NO_CURRENT_MODEL);

	isInitialized = true;
}

void CMainWindow::updateView(bool repaintAll, bool buffered)
{
	//qInfo() << "checkpoint" << Qt::endl;

	if (repaintAll)
	{
		if (ui.mdiArea != nullptr)
		{
			foreach(QMdiSubWindow * window, ui.mdiArea->subWindowList())
			{
				MdiChild* child = (MdiChild*)window->widget();

				if (child != nullptr)
				{
					child->updateView(buffered);
				}
				else
				{
					qInfo() << "No active child.";
				}

			}
		}
	}
	else
	{
		if (lastActiveWindow != nullptr)
		{
			MdiChild* child = (MdiChild*)lastActiveWindow->widget();

			if (child != nullptr)
			{
				child->updateView(buffered);
			}
			else
			{
				qInfo() << "No active child.";
			}
		}
		else
		{
			qInfo() << "No active subwindow.";
		}
	}

	//QCoreApplication::processEvents();
	AP::processEvents();
}

void CMainWindow::addPluginToListView( int id, QString txt )
{
	dockPluginList->addPluginToList( id, txt );
}


void CMainWindow::changeMenuAfterSelect()
{
	CModel3D *obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL == obj )
	{
		ui.menuImage->menuAction()->setVisible(false);
		ui.menuModel->menuAction()->setVisible(false);
		ui.action_File_SaveAs->setEnabled( false );
	}
	else if (obj->hasType(CObject::IMAGE))
	{
		ui.menuModel->menuAction()->setVisible(false);

		ui.menuImage->menuAction()->setVisible(true);
		ui.action_ImageFit->setChecked(((CImage*)obj)->fitToWindow);

		ui.action_File_SaveAs->setEnabled(true);
	}
	else if (obj->hasType(CObject::MODEL))
	{
		ui.menuImage->menuAction()->setVisible(false);

		ui.action_Model_Show->setChecked( obj->getSelfVisibility() );
		ui.action_Model_ShowTexture->setChecked( obj->testOption( CModel3D::optRenderWithTexture ) );
		ui.action_Model_Lock->setChecked( obj->isLocked() );
		
		ui.actionModelInSelection->setChecked( AP::WORKSPACE::SELECTION::isModelSelected(obj->id() ) );

		ui.action_Model_ShowBB->setChecked( obj->DrawBB() );
		
		ui.action_Model_Smoothing->setChecked( obj->testOption( CModel3D::optSmoothVertices ) );
		
		ui.action_Model_RenderAsFaces->setChecked( ! ( obj->testOption( CModel3D::optRenderAsEdges ) || obj->testOption( CModel3D::optRenderAsPoints ) ) );
		ui.action_Model_RenderAsEdges->setChecked( obj->testOption( CModel3D::optRenderAsEdges ) );
		ui.action_Model_RenderAsVertices->setChecked( obj->testOption( CModel3D::optRenderAsPoints ) );

		ui.menuModel->menuAction()->setVisible(true);

		ui.action_File_SaveAs->setEnabled( true );
	}
}

std::vector<QMdiSubWindow*> CMainWindow::subWindows() {
	//return ui.mdiArea->subWindowList().toVector().toStdVector();
	QVector<QMdiSubWindow*> vec = ui.mdiArea->subWindowList().toVector();
	return std::vector<QMdiSubWindow*>(vec.begin(), vec.end());
}

GLViewer * CMainWindow::currentViewer()
{
	if (lastActiveWindow != nullptr)
	{
		MdiChild* child = (MdiChild*)lastActiveWindow->widget();
		if (child != nullptr)
		{
			if (child->hasType(MdiChild::Type::GL))
			{
				return qobject_cast<GLViewer*>(child->m_widget);
			}

		}
	}
	return nullptr;
}

MdiChild* CMainWindow::currentMdiChild()
{
	if (lastActiveWindow != nullptr)
	{
		return (MdiChild*)lastActiveWindow->widget();
	}
	return nullptr;
}



void CMainWindow::leaveSelectionMode()
{
	ui.action_select_None->setChecked(true);
	actionSelectNone();
}

void CMainWindow::activateGLViewerInstance()
{
	auto lista = ui.mdiArea->subWindowList();

	foreach(QMdiSubWindow * window, lista)
	{
		MdiChild* child = (MdiChild*)window->widget();

		if (child != nullptr)
		{
			if (child->hasType(MdiChild::Type::GL))
			{
				ui.mdiArea->setActiveSubWindow(window);
				return;
			}
		}

	}

	ui.mdiArea->setActiveSubWindow(MdiChild::create(MdiChild::Type::GL, ui.mdiArea));
}

QMdiSubWindow* CMainWindow::getPicViewerInstance(int id)
{
	auto lista = ui.mdiArea->subWindowList();

	foreach(QMdiSubWindow * window, lista)
	{
		MdiChild* child = (MdiChild*)window->widget();

		if (child != nullptr)
		{
			if (child->hasType(MdiChild::Type::Pic))
			{
				PicViewer* pic = qobject_cast<PicViewer*>(child->m_widget);
				if (pic->id() == id)
				{
					return window;
				}
			}
		}

	}

	return nullptr;
}

void CMainWindow::activatePicViewerInstance(int id)
{
	QMdiSubWindow* window = getPicViewerInstance(id);

	if (window != nullptr)
	{
		ui.mdiArea->setActiveSubWindow(window);
	}
	else
	{
		ui.mdiArea->setActiveSubWindow(MdiChild::create((CImage*)AP::WORKSPACE::getModel(id), ui.mdiArea));
	}
}

void CMainWindow::closePicViewers(int id)
{
		auto lista = ui.mdiArea->subWindowList();

		foreach(QMdiSubWindow * window, lista)
		{
			MdiChild* child = (MdiChild*)window->widget();

			if (child != nullptr)
			{
				if (child->hasType(MdiChild::Type::Pic))
				{
					PicViewer* pic = qobject_cast<PicViewer*>(child->m_widget);
					if (pic->id() == id)
					{
						window->close();
					}
				}
			}

		}
}

void CMainWindow::createPicViewer(CImage* im)
{
	MdiChild::create(im, ui.mdiArea)->show();
}


void CMainWindow::createSelectMenu()
{
	//selectVertices = new QAction("Select Vertices", this);
	//selectVertices->setCheckable(true);
	//selectVertices->setChecked(false);
	//connect(selectVertices, SIGNAL(triggered()), this, SLOT(slotSelectSomething()));

	//selectFaces = new QAction("Select Faces", this);
	//selectFaces->setCheckable(true);
	//selectFaces->setChecked(false);
	//connect(selectFaces, SIGNAL(triggered()), this, SLOT(slotSelectSomething()));

	//selectNone = new QAction("Select Faces", this);
	//selectNone->setCheckable(true);
	//selectNone->setChecked(true);
	//connect(selectNone, SIGNAL(triggered()), this, SLOT(slotSelectSomething()));

	selectActionGroup = new QActionGroup(this);
	//selectActionGroup->addAction(selectVertices);
	//selectActionGroup->addAction(selectFaces);
	//selectActionGroup->addAction(selectNone);
	//selectNone->setChecked(true);
	selectActionGroup->addAction(ui.action_select_Vertex);
	selectActionGroup->addAction(ui.action_select_Face);
	selectActionGroup->addAction(ui.action_select_None);
	ui.action_select_None->setChecked(true);

	ui.action_select_Face->setVisible(false);

	//ui.menuSelect->addAction(selectVertices);
	///i.menuSelect->addAction(selectFaces);
	//ui.menuSelect->addAction(selectNone);
}

void CMainWindow::createRecentActions()
{
	QAction* action = 0;
	for (auto i = 0; i < maxRecentFiles; ++i) {
		action = new QAction(this);
		action->setVisible(false);
		QObject::connect(action, &QAction::triggered, this, &CMainWindow::openRecent);
		recentFileActionList.append( action );
	}
}

void CMainWindow::createRecentMenus()
{
	for (auto i = 0; i < maxRecentFiles; ++i)
		ui.menuRecentFiles->addAction(recentFileActionList.at(i));

	updateRecentActionList();
}


void CMainWindow::adjustForCurrentFile(const QString &filePath) {
	QString currentFilePath = filePath;
	setWindowFilePath(currentFilePath);

	QStringList recentFilePaths = AppSettings::mainSettings()->value("recentFiles").toStringList();
	recentFilePaths.removeAll(filePath);
	recentFilePaths.prepend(filePath);
	while (recentFilePaths.size() > maxRecentFiles)
		recentFilePaths.removeLast();
	AppSettings::mainSettings()->setValue("recentFiles", recentFilePaths);

	updateRecentActionList();
}

void CMainWindow::updateRecentActionList() {
	QStringList recentFilePaths = AppSettings::mainSettings()->value("recentFiles").toStringList();

	auto itEnd = 0;
	if (recentFilePaths.size() <= maxRecentFiles)
		itEnd = recentFilePaths.size();
	else
		itEnd = maxRecentFiles;

	for (auto i = 0; i < itEnd; ++i) {
		QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
		recentFileActionList.at(i)->setText(strippedName);
		recentFileActionList.at(i)->setData(recentFilePaths.at(i));
		recentFileActionList.at(i)->setVisible(true);
	}

	for (auto i = itEnd; i < maxRecentFiles; ++i)
		recentFileActionList.at(i)->setVisible(false);
}

