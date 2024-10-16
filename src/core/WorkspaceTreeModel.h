#pragma once
#include "WorkspaceTreeItem.h"

class CModel3D;
class CAnnotation;
class CObject;

class WorkspaceTreeModel : public QStandardItemModel
{
public:
	WorkspaceTreeModel(QObject *parent);
	~WorkspaceTreeModel();

	void initialize();

	WorkspaceTreeItem* append(QStandardItem* root, CBaseObject* obj);

	//WorkspaceTreeItem* append(QStandardItem* root, CModel3D* obj, bool hideable, bool lockable, bool checkable, bool checked);
	//WorkspaceTreeItem* appendDataChild(QStandardItem* root, CObject* obj);
	//WorkspaceTreeItem* appendAnnotationChild(QStandardItem* root, CAnnotation* an);

	void addModelWithChildren(CModel3D *obj);
};

