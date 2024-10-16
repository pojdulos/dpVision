#pragma once
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "dll_global.h"

class QOpenGLWindow;
class QMainWindow;
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

class CBaseObject;
class GLViewer;
class MdiChild;
class CImage;
class CModel3D;
class QLocalServer;
class QLocalSocket;
class QTcpServer;
class QTcpSocket;

class DPVISION_EXPORT CMainWindow : public QMainWindow
{
	Q_OBJECT

		CMainWindow(QWidget* parent = 0);

public:
	~CMainWindow();

	Ui::MainWindow ui;
	ProgressIndicator* progressIndicator;
	
	static CMainWindow* instance();

	virtual void showEvent(QShowEvent* ev) override;

    QMenu *menuModel;

	QToolBar* toolbar;

	DockWidgetWorkspace *dockWorkspace;
	DockWidgetLights *dockLights;
	
	DockWidgetProperties *dockProperties;

	DockWidgetPluginList *dockPluginList;
	DockWidgetPluginPanel *dockPluginPanel;

	DockWidgetHistogram *dockHisto;
	DockWidgetImageViewer* dockImage;

	GLViewer * currentViewer();
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

	QMenu* recentFilesMenu;

	QAction* openAction;
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
	void adjustForCurrentFile(const QString& filePath);
	void updateRecentActionList();

private slots:
	void openRecent();
	void processNewConnection();
	void proceessData();

public slots:
	void userDefinedMenuItemSlot();
	void viewerSelected(QMdiSubWindow*);


	void onCurrentObjectChanged(int);
	void onCurrentObjectChanged(CBaseObject*);


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

	void actionLookDir(int,CModel3D*);

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


};

#endif // CMAINWINDOW