#pragma once

#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "dll_global.h"

#include <QtWidgets/QMenu>

class CBaseObject;
class CObject;
class CModel3D;
class QGraphicsView;

class DPVISION_EXPORT CContextMenu : public QMenu
{
    Q_OBJECT

public:
    CContextMenu(CBaseObject *obj, QWidget *parent = nullptr);
    ~CContextMenu();

	QMenu* createAddObjectMenu();

	QMenu* createRepositionMenu();

	QMenu* createDeleteMenu();

	QMenu* createVolumetricMenu();


	QMenu* createCopySubMenu(QString label, CObject* obj);

	QMenu* createCopyMenu();

	QMenu* createMoveSubMenu(QString label, CObject* obj);

	QMenu* createMoveMenu();

	QMenu* createAddAnnoMenu();

private:
	CBaseObject *m_obj;

	//QGraphicsView* m_graphView = nullptr; // lub std::unique_ptr, jeœli C++11+

public slots:
	void slot_delete_object_with_subtree();
	void slotAddAnnotation();
	void slotPlaneToMesh();
	void slotTriangleToPlane();
	void slotCreateEmptyModel();

	void slot_mesh_create();

	void slot_volum_create();
	void slot_volumetric_export();
	void slot_copy_frames_as_models();
	void slot_volumetric_set_metadata();
	void slot_vol_show_images();
	void slot_volumetric_sift_cloud();
	void slot_volumetric_marching_cube();
	void slot_volumetric_marching_tetra();

	//void pointEditSlot();
	void pointHide();

	void newPicWindow();

	void slotCollapseAll();
	void slotExpandAll();

	void slotHideAll();

	void slotShowAll();


	void histogramSave();
	void setOfFacesToMesh();

	//void volTKtoCloud();
	//void volTKtoMesh();

	void meshAction();
	
	void moveTo();
	void copyTo();

	void saveObjAs();
	void slot_make_me_root();
	void slot_test_graph();
	void slot_make_me_root2();
	void slot_repositioning();
	void slot_apply_last_transform();
	void slot_delete_and_keep_children();
	void slot_create_inversed_transform();
};

#endif // CONTEXTMENU_H
