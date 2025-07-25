#pragma once
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "dll_global.h"

#include <QMainWindow>
#include <QAction>

class QOpenGLWindow;
class ProgressIndicator;

class DockWidgetWorkspace;
class DockWidgetProperties;
class DockWidgetLights;
class DockWidgetPluginList;
class DockWidgetPluginPanel;
class DockWidgetHistogram;
class DockWidgetImageViewer;

#include <QtCore/QList>

#include "ui_mainWindow.h"
#include <QMdiSubWindow>
#include <QPointer>

class SettingsDialog;

class CBaseObject;
class GLViewer;
class MdiChild;
class CImage;
class CModel3D;
class QLocalServer;
class QLocalSocket;
class QTcpServer;
class QTcpSocket;


//#include <QWidget>
//#include <QMouseEvent>
//#include <QPushButton>
//#include <QHBoxLayout>
//#include <QVBoxLayout>
//
//class TitleBar : public QWidget {
//	Q_OBJECT
//
//public:
//	explicit TitleBar(QWidget* parent = nullptr) : QWidget(parent) {
//		setStyleSheet("background-color: #2E2E2E; height: 30px;");
//
//		// Przyk�adowy przycisk zamykania
//		QPushButton* closeButton = new QPushButton("X", this);
//		closeButton->setStyleSheet("background-color: #FF0000; color: white; border: none;");
//		connect(closeButton, &QPushButton::clicked, parent, &QWidget::close);
//
//		// Uk�ad dla przycisk�w
//		QHBoxLayout* layout = new QHBoxLayout(this);
//		layout->addStretch(); // Przestrze� mi�dzy przyciskami a kraw�dzi�
//		layout->addWidget(closeButton);
//		layout->setContentsMargins(0, 0, 0, 0);
//		setLayout(layout);
//	}
//
//protected:
//	void mousePressEvent(QMouseEvent* event) {
//		if (event->button() == Qt::LeftButton) {
//			// Zapisz pozycj� kursora wzgl�dem okna g��wnego
//			dragPosition = event->globalPos() - parentWidget()->parentWidget()->frameGeometry().topLeft();
//			event->accept();
//		}
//	}
//
//	void mouseMoveEvent(QMouseEvent* event) {
//		if (event->buttons() & Qt::LeftButton) {
//			// Przesuwaj ca�e okno g��wne (QMainWindow)
//			parentWidget()->parentWidget()->move(event->globalPos() - dragPosition);
//			event->accept();
//		}
//	}
//	
//private:
//	QPoint dragPosition;
//};

class DPVISION_EXPORT CMainWindow : public QMainWindow
{
	Q_OBJECT

		CMainWindow(QWidget* parent = 0);

public:
	~CMainWindow();

	Ui::MainWindow ui;
	QPointer<ProgressIndicator> progressIndicator;
	
	static CMainWindow* instance();

	virtual void showEvent(QShowEvent* ev) override;

    QMenu *menuModel;

	QToolBar* toolbar;

	QPointer<DockWidgetWorkspace> dockWorkspace;
	QPointer<DockWidgetLights> dockLights;
	
	QPointer<DockWidgetProperties> dockProperties;

	QPointer<DockWidgetPluginList> dockPluginList;
	QPointer<DockWidgetPluginPanel> dockPluginPanel;

	QPointer<DockWidgetHistogram> dockHisto;
	QPointer<DockWidgetImageViewer> dockImage;

	GLViewer* currentViewer();
	MdiChild* currentMdiChild();

	void updateView(bool repaintAll = true, bool buffered = true);

	void updateActiveView( bool buffered = true ) { updateView( false, buffered); };
	void updateAllViews( bool buffered = true ) { updateView( true, buffered ); };

	//void updateListViewSelection( int id );

	void addPluginToListView( int id, QString txt );

	void addUserAction( QString caption, void f() ) 
	{
		QAction *newAct = new QAction( caption, this );
		//newAct->setShortcuts(QKeySequence::New);
		//newAct->setStatusTip(tr("Create a new file"));
		connect( newAct, SIGNAL(triggered()), this, SLOT(userDefinedMenuItemSlot()) );
	};

	void changeMenuAfterSelect();

	std::vector<QMdiSubWindow*> subWindows();

	bool isCurrentSubWindow(QMdiSubWindow* window) { return (window == ui.mdiArea->currentSubWindow()); };

	void leaveSelectionMode();
	void activateGLViewerInstance();

	QMdiSubWindow* getPicViewerInstance(int id);
	void activatePicViewerInstance(int id);

	void closePicViewers(int id);
	void createPicViewer(CImage* im);

	void startServer();

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

private:
	//QMenu* selectMenu;
	QActionGroup* selectActionGroup;
	//QAction *selectVertices, *selectFaces, *selectNone;
	void createSelectMenu();

	QPointer<QMenu> recentFilesMenu;

	QPointer<QMdiSubWindow> lastActiveWindow;

	QPointer<QAction> openAction;
	QList<QAction*> recentFileActionList;

	const int maxRecentFiles = 10;

	//GLViewer * createMdiChild();
	//GLViewer * activeMdiChild() const;
	
	//QLocalServer* server;
	//QLocalSocket* clientConnection;
	QTcpServer* server;
	QTcpSocket* clientConnection;

	void createRecentActions();
	void createRecentMenus();
	void updateRecentActionList();

public:
	void adjustForCurrentFile(const QString& filePath);

private:
	QPointer<SettingsDialog> settingsDialog;

private slots:
	void openRecent();
	void processNewConnection();
	void proceessData();
	void openSettingsDialog();

public slots:
	void userDefinedMenuItemSlot();
	void viewerSelected(QMdiSubWindow*);


	void onWorkspaceObjectActivated(int);
	void onWorkspaceObjectActivated(CBaseObject*);
	void onWorkspaceObjectAdded(int);
	void onWorkspaceObjectRemoved(int);
	


	// menu File
	void fileOpen();
	void fileSave();
	void importImage();

	// menu View
	//void createChild();
	void createGLViewer();
	void createPicViewer();
	void viewChildFS();
	void stereoscopyOff();
	void stereoscopyQuadbuff();
	void stereoscopyRowInterlaced();
	void stereoscopyColInterlaced();
	void stereoscopyAboveBelow();
	void stereoscopySideBySide();
	void stereoscopyRedCyan();
	void stereoscopyRedBlue();
	void stereoscopyRedGreen();
	void stereoscopyCyanRed();
	void stereoscopyBlueRed();
	void stereoscopyGreenRed();
	void projectionOrthogonal();
	void projectionPerspective();
	void useShaders(bool);
	void cameraResetPosition();

	void actionLookDir(int,std::shared_ptr<CModel3D>);

	void actionLookDir();

	// menu Select
	void actionSelectSelectionDelete();
	void actionSelectNoSelectionDelete();
	void actionSelectVertex();
	void actionSelectFace();
	void actionSelectNone();


	void imageFit(bool);

	// menu Model
	//void createMenuModel();
	void renderAsFaces();
	void renderAsEdges();
	void renderAsVertices();
	void textureOnOff();
	void smoothingOnOff();
	void createNewCopy();
	void bbShowHide(bool);
	void modelLock(bool);
	void modelClose();
	void modelVisibility(bool);

	void modelInvertNormals();
	void meshApplyTransformations();

	// menu Workspace
	void openWorkspace();
	void saveWorkspace();
	void removeAllModels();
	void removeSelectedModels();
	void resetAllTransformations();
	void resetSelectedTransformations();
	void lockAllModels();
	void lockSelectedModels();
	void unlockAllModels();
	void unlockSelectedModels();
	void selectAll();
	void unselectAll();
	void hideAllModels();
	void hideSelectedModels();
	void showAllModels();
	void showSelectedModels();
	void modelInSelection(bool);
	void modelResetTransformations();

	// menu Tools
	void pmEcol();
	void pmVsplit();
	void grabPoints(bool);
	void pickSnap(bool);

	//menu Help
	void helpAbout();
	void switchConsole(bool);

	void actionScreenshot();
};

#endif // CMAINWINDOW
