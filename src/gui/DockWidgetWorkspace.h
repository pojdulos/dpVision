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
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDebug>

class DeselectableTreeView : public QTreeView
{
	Q_OBJECT

public:
	DeselectableTreeView(QWidget* parent) : QTreeView(parent) {}
	virtual ~DeselectableTreeView() {}

signals:
	void itemOrderMoveRequested(int movedId, int anchorId, bool after);

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

	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dragMoveEvent(QDragMoveEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
	virtual void startDrag(Qt::DropActions supportedActions) override;

private:
	int draggedItemId_ = -1;
	int draggedParentId_ = -1;
};



class DockWidgetWorkspace : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetWorkspace(QWidget *parent = 0);
	~DockWidgetWorkspace();

	void updateVisibilityAll(QStandardItem* parent = nullptr);

	void rebuildTree();

	void addItem(std::shared_ptr<CBaseObject> obj);
	void addItem(int id, int parentId);
	void removeItem(int id);

	inline QTreeView* getTreeView() { return ui.treeView; };

	// obsolete, use: rebuildTree()
	void updateTreeView() { rebuildTree(); };
	
	void selectItem( int id );

	std::shared_ptr<CBaseObject> getCurrentItemObj();

	int getCurrentItemId();

	QVector<std::shared_ptr<CBaseObject>> getSelectedObjects();

	void processSelection(const QItemSelection& selected, const QItemSelection& deselected);

	QModelIndex findWorkspaceTreeModelIndex(int id);
	QModelIndex findWorkspaceTreeModelByHandle(CBaseObject* obj);
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
	void colSelfVisibilityClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem);
	void colKidsVisibilityClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem);
	void colNameClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem);
	bool refreshItemById(int id);

signals:
	//void currentObjectChanged(std::shared_ptr<CBaseObject>);
	void currentObjectChanged(int);

public slots:
	void onWorkspaceObjectActivated(int);
	void onWorkspaceObjectStateChanged(int);
	void onWorkspaceObjectAdded(int);
	void onWorkspaceObjectRemoved(int);
	void onWorkspaceStructureChanged();

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
	void onItemOrderMoveRequested(int movedId, int anchorId, bool after);
};

#endif // DOCKWIDGETWORKSPACE_H
