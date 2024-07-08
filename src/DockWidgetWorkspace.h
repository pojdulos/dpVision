#ifndef DOCKWIDGETWORKSPACE_H
#define DOCKWIDGETWORKSPACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

class CAnnotation;
class CFace;
class CModel3D;
class CBaseObject;
class CObject;
class QStandardItem;
class WorkspaceTreeItem;

#include "dll_global.h"

#include <QMouseEvent>
#include <QDebug>

class DeselectableTreeView : public QTreeView
{
	Q_OBJECT

public:
	DeselectableTreeView(QWidget* parent) : QTreeView(parent) {}
	virtual ~DeselectableTreeView() {}

private:
	virtual void mousePressEvent(QMouseEvent* event) override
	{
		QModelIndex item = indexAt(event->pos());

		QTreeView::mousePressEvent(event);
		
		if (!item.isValid())
		{
			selectionModel()->clear();
			emit(clicked(item));
		}
	}
};



class DockWidgetWorkspace : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetWorkspace(QWidget *parent = 0);
	~DockWidgetWorkspace();

	void updateVisibilityAll(QStandardItem* parent = nullptr);

	void rebuildTree();

	void addItem(CBaseObject* obj);
	void addItem(int id, int parentId);
	void removeItem(int id);

	// obsolete, use: rebuildTree()
	void updateTreeView() { rebuildTree(); };
	
	void selectItem( int id );

	int getCurrentItemId();

	QVector<CBaseObject*> getSelectedObjects();

	void processSelection(const QItemSelection& selected, const QItemSelection& deselected);

	QModelIndex findWorkspaceTreeModelIndex(int id);
	void setItemCheckedById(int id, bool b);
	void setItemVisibleById(int id, bool b);
	void setItemKidsVisibleById(int id, bool b);
	void setItemLockedById(int id, bool b);
	void setItemLabelById(int id, QString s);
	void modifyItemById(int id);

	void expandAll();
	void collapseAll();

private:
	void colLockClicked(CModel3D* obj, WorkspaceTreeItem* clickedItem);
	void colSelfVisibilityClicked(CBaseObject* obj, WorkspaceTreeItem* clickedItem);
	void colKidsVisibilityClicked(CBaseObject* obj, WorkspaceTreeItem* clickedItem);
	void colNameClicked(CBaseObject* obj, WorkspaceTreeItem* clickedItem);

signals:
	void currentObjectChanged(CBaseObject*);
	void currentObjectChanged(int);

public slots:
	void onCurrentObjectChanged(int);
	void onCurrentObjectChanged(CBaseObject*);

	void onTreeViewItemClicked(QModelIndex);

private:
	void setupUi();
	void retranslateUi();

	struct Ui {
		QWidget* dockWidgetContents;
		QGridLayout* gridLayout;
		DeselectableTreeView* treeView;
	} ui;

private slots:
 	void onCustomContextMenu(const QPoint &point);
};

#endif // DOCKWIDGETWORKSPACE_H
