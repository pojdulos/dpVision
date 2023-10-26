#pragma once

#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QtWidgets/QMenu>

class CBaseObject;
class CObject;
class CModel3D;

class __declspec(dllexport) CContextMenu : public QMenu
{
    Q_OBJECT

public:
    CContextMenu(CBaseObject *obj, QWidget *parent = nullptr);
    ~CContextMenu();

	QMenu* createCopySubMenu(QString label, CObject* obj);

	QMenu* createCopyMenu();

	QMenu* createMoveSubMenu(QString label, CObject* obj);

	QMenu* createMoveMenu();

	QMenu* createAddAnnoMenu();

private:
	CBaseObject *m_obj;

private slots:
	void slotDeleteObject();
	void slotAddAnnotation();
	void slotCreateEmptyModel();

	//void pointEditSlot();
	void pointHide();

	void newPicWindow();

	void histogramSave();
	void setOfFacesToMesh();

	void volTKshowImagesDlg();
	void volTKtoCloud();
	void volTKtoMesh();

	void meshAction();
	
	void copyTo();
	void copyToNew();

	void moveTo(); 
	void moveToNew();

	void saveObjAs();
};

#endif // CONTEXTMENU_H
