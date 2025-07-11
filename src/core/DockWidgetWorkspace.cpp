#include "DockWidgetWorkspace.h"

#include "UI.h"
#include "AP.h"

#include "DockWidgetModel.h"
#include "Annotation.h"

#include "Workspace.h"

#include "WorkspaceTreeModel.h"
#include "WorkspaceTreeItem.h"

#include "ContextMenu.h"

#include "MainWindow.h"

DockWidgetWorkspace::DockWidgetWorkspace(QWidget *parent) : QDockWidget(parent)
{
	setupUi();

	setObjectName("DockWidgetWorkspace");

	WorkspaceTreeModel* model = new WorkspaceTreeModel(ui.treeView);
	ui.treeView->setModel(model);

	//------------------------------------------------------------------------------------------------

	QObject::connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onTreeViewItemClicked(QModelIndex)));

	//connect(model, SIGNAL(itemChanged(QStandardItem*)), SLOT(onItemChanged(QStandardItem*)));
	//ui.treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	ui.treeView->setSelectionBehavior( QAbstractItemView::SelectionBehavior::SelectRows );

	ui.treeView->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
	connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

	ui.treeView->header()->resizeSection(WorkspaceTreeItem::Column::colSelfVisibility, 16);
	ui.treeView->header()->resizeSection(WorkspaceTreeItem::Column::colKidsVisibility, 16);
	ui.treeView->header()->resizeSection(WorkspaceTreeItem::Column::colLock, 24);

	ui.treeView->header()->setSectionResizeMode(WorkspaceTreeItem::Column::colName, QHeaderView::ResizeMode::Stretch );
	ui.treeView->header()->setSectionResizeMode(WorkspaceTreeItem::Column::colSelfVisibility, QHeaderView::ResizeMode::ResizeToContents );
	ui.treeView->header()->setSectionResizeMode(WorkspaceTreeItem::Column::colKidsVisibility, QHeaderView::ResizeMode::ResizeToContents );
	ui.treeView->header()->setSectionResizeMode(WorkspaceTreeItem::Column::colLock, QHeaderView::ResizeMode::ResizeToContents );

	ui.treeView->header()->setStretchLastSection(false);

	ui.treeView->setHeaderHidden(true);
}

//#include <QtWidgets/QMessageBox>
DockWidgetWorkspace::~DockWidgetWorkspace()
{
	//QMessageBox::information(0, "DockWidgetWorkspace", "destructor");
}

void DockWidgetWorkspace::onCustomContextMenu(const QPoint &point)
{
	QModelIndex index = ui.treeView->indexAt(point);
	
	if (index.isValid())
	{
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		std::shared_ptr<CBaseObject> clickedObject = ((WorkspaceTreeItem*) model->itemFromIndex(index))->getObject();

		if (clickedObject == nullptr) return;

		CContextMenu(clickedObject, ui.treeView).exec(ui.treeView->mapToGlobal(point));
    }
	else
	{
		ui.treeView->clearSelection();

		emit currentObjectChanged(NO_CURRENT_MODEL);

		CContextMenu(nullptr, ui.treeView).exec(ui.treeView->mapToGlobal(point));
	}
}


void DockWidgetWorkspace::updateVisibilityAll(QStandardItem* parent)
{
	WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();
	if (parent == nullptr) {
		parent = model->invisibleRootItem();
	}

	for (int i = 0; i < parent->rowCount(); ++i) {
		WorkspaceTreeItem* childItem = (WorkspaceTreeItem*) parent->child(i);

		std::shared_ptr<CBaseObject> currentObject = childItem->getObject();

		childItem->changeIcon(WorkspaceTreeItem::Column::colSelfVisibility, currentObject->getSelfVisibility());
		
		qDebug() << childItem->text();

		// Rekurencyjne wywo�anie dla dzieci, je�li istniej�
		if (childItem->hasChildren()) {
			updateVisibilityAll(childItem);
		}
	}
}

// U�ycie:
// QTreeView* treeView = ...; // Twoje QTreeView
// QStandardItemModel* model = ...; // Model powi�zany z QTreeView
// iterujPoWierszach(model);


void DockWidgetWorkspace::rebuildTree()
{
	ui.treeView->blockSignals(true);
	ui.treeView->reset();

	WorkspaceTreeModel *model = (WorkspaceTreeModel *) ui.treeView->model();

	//model->clear();
	//model->initialize();
	if ( model->hasChildren() )
	{
		model->removeRows(0, model->rowCount());
	}

	for (CWorkspace::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
	{
		model->addModelWithChildren((*it).second);
	}

	ui.treeView->blockSignals(false);
}



void DockWidgetWorkspace::selectItem( int id )
{
	ui.treeView->blockSignals(true);

	if (id == -1) {
		ui.treeView->selectionModel()->clear();
	}
	else {
		WorkspaceTreeModel *model = (WorkspaceTreeModel *)ui.treeView->model();

		QModelIndexList Items = model->match(model->index(0, 0), Qt::UserRole+1, QVariant::fromValue(id), 1, Qt::MatchRecursive);

		if (Items.count() > 0)
		{
			QModelIndex current = Items.first();

			if (current.isValid())
			{
				ui.treeView->selectionModel()->select( current, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows );
			}
		}
	}

	ui.treeView->blockSignals(false);
}

std::shared_ptr<CBaseObject> DockWidgetWorkspace::getCurrentItemObj()
{
	QModelIndex current = ui.treeView->currentIndex();
	if (current.isValid()) {
		WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();
		QStandardItem* qItem = model->itemFromIndex(current);
		if (!qItem) return nullptr;

		WorkspaceTreeItem* item = static_cast<WorkspaceTreeItem*>(qItem);
		std::shared_ptr<CBaseObject> currentObject = item->getObject();

		return currentObject;
	}
	return nullptr;
}

int DockWidgetWorkspace::getCurrentItemId()
{
	QModelIndex current = ui.treeView->currentIndex();
	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		std::shared_ptr<CBaseObject> currentObject = ((WorkspaceTreeItem*)model->item(current.row(), 0))->getObject();

		if (currentObject == nullptr) return NO_CURRENT_MODEL;
		
		return currentObject->id();
	}
	return NO_CURRENT_MODEL;
}

QVector<std::shared_ptr<CBaseObject>> DockWidgetWorkspace::getSelectedObjects()
{
	QSet<std::shared_ptr<CBaseObject>> result;

	QModelIndexList indexes = ui.treeView->selectionModel()->selectedIndexes();
	for (const auto& idx : indexes)
	{
		WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem* item = (WorkspaceTreeItem *) model->itemFromIndex(idx);

		std::shared_ptr<CBaseObject> obj = item->getObject();

		if (obj != nullptr)
		{
			std::shared_ptr<CBaseObject> parent = obj->getParentPtr();

			if (parent == nullptr)
			{
				result.insert(obj);
			}
			else
			{
				while (parent != nullptr)
				{
					if (result.contains(parent))
					{
						break;
					}

					parent = parent->getParentPtr();

					if (parent == nullptr)
					{
						result.insert(obj);
						break;
					}
				}
			}
		}
	}

	return result.values().toVector();
}


void DockWidgetWorkspace::processSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (selected.empty())
		return;

	QItemSelectionModel* selectionModel = ui.treeView->selectionModel();

	QItemSelection selection = selectionModel->selection();
	//const QModelIndex parent = ui.treeView->currentIndex().parent();
	const QModelIndex parent = ui.treeView->rootIndex();

	QItemSelection invalid;

	Q_FOREACH(QModelIndex index, selection.indexes())
	{
		if (index.parent() == parent)
			continue;

		invalid.select(index, index);
	}

	selectionModel->select(invalid, QItemSelectionModel::Deselect);
}

QModelIndex DockWidgetWorkspace::findWorkspaceTreeModelIndex(int id)
{
	WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();
	QModelIndexList items = model->match(
		model->index(0, 0),
		Qt::UserRole+1,
		QVariant::fromValue(id),
		1, // look *
		Qt::MatchRecursive);

	if ( ! items.empty() )
		return items.first();

	return QModelIndex();
}

QModelIndex DockWidgetWorkspace::findWorkspaceTreeModelByHandle(CBaseObject* obj)
{
	WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();
	QModelIndexList items = model->match(
		model->index(0, 0),
		Qt::UserRole,
		QVariant::fromValue(obj),
		1, // look *
		Qt::MatchRecursive);

	if (!items.empty())
		return items.first();

	return QModelIndex();
}


void DockWidgetWorkspace::modifyItemById(int id)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);

	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		QStandardItem *item = model->itemFromIndex(current);

		Qt::CheckState state = item->checkState();
		if (state == Qt::Checked)
		{
			item->setCheckState(Qt::Unchecked);
		}
		else
		{
			item->setCheckState(Qt::Checked);
		}
	}
}

void DockWidgetWorkspace::setItemCheckedById(int id, bool b)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);

	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		QStandardItem *item = model->itemFromIndex(current);

		item->setCheckState( b ? Qt::Checked : Qt::Unchecked );
	}
}

void DockWidgetWorkspace::setItemVisibleById(int id, bool b)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);
	std::shared_ptr<CBaseObject> obj = AP::WORKSPACE::findId(id);

	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem *item = (WorkspaceTreeItem*) ( model->itemFromIndex( current ) );

		item->changeIcon(WorkspaceTreeItem::Column::colSelfVisibility, b);
	}
}

void DockWidgetWorkspace::setItemKidsVisibleById(int id, bool b)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);

	if (current.isValid()) {
		WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem* item = (WorkspaceTreeItem*)(model->itemFromIndex(current));

		item->changeIcon(WorkspaceTreeItem::Column::colKidsVisibility, b);
	}
}

void DockWidgetWorkspace::setItemLockedById(int id, bool b)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);

	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem *item = (WorkspaceTreeItem*)(model->itemFromIndex(current));
		item->changeIcon(WorkspaceTreeItem::Column::colLock, b);
	}
}

void DockWidgetWorkspace::setItemLabelById(int id, QString s)
{
	QModelIndex current = findWorkspaceTreeModelIndex(id);

	if (current.isValid()) {
		WorkspaceTreeModel *model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem *item = (WorkspaceTreeItem*)(model->itemFromIndex(current));

		item->setText(s);
	}
}

void DockWidgetWorkspace::expandAll()
{
	ui.treeView->expandAll();
}

void DockWidgetWorkspace::collapseAll()
{
	ui.treeView->collapseAll();
}

void DockWidgetWorkspace::addItem(std::shared_ptr<CBaseObject> obj)
{
	if (obj == nullptr) return;

	WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();
	CBaseObject* parent = obj->getParent();

	if (parent == nullptr)
	{
		if (obj->hasType(CBaseObject::MODEL))
		{
			model->addModelWithChildren(std::static_pointer_cast<CModel3D>(obj));
		}
	}
	else
	{
		QModelIndex parentIndex = findWorkspaceTreeModelIndex(parent->id());

		if (parentIndex.isValid())
		{
			QStandardItem* parentItem = model->itemFromIndex(parentIndex);
			switch (obj->category())
			{
			case CBaseObject::Category::OBJECT:
				model->append(parentItem, obj);
				break;
			case CBaseObject::Category::ANNOTATION:
				model->append(parentItem, obj);
				break;
			}
		}
	}
}


void DockWidgetWorkspace::addItem(int id, int parentId)
{
	ui.treeView->blockSignals(true);

	WorkspaceTreeModel *model = (WorkspaceTreeModel *)ui.treeView->model();

	if (parentId == -1)
	{
		// najwy�szy poziom
		std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getModel(id);
		if (nullptr != obj)
		{
			model->addModelWithChildren(obj);
		}
	}
	else
	{
		QModelIndex parentIndex = findWorkspaceTreeModelIndex( parentId );

		if (parentIndex.isValid())
		{
			QStandardItem *i1 = model->itemFromIndex(parentIndex);

			std::shared_ptr<CObject> parent = std::dynamic_pointer_cast<CObject>(AP::WORKSPACE::findId(parentId));

			int grandparentId = parent->parentId();

			if ( -1 != grandparentId ) {
				// dodaj� childa do childa - to na razie nie jest u�ywane
		
			}
			else {
				// dodaj� childa do modelu

				std::shared_ptr<CBaseObject> child = parent->getSomethingWithId(id);

				switch (child->category()) {
					case CBaseObject::Category::OBJECT:
						model->append(i1, child);
						break;
					case CBaseObject::Category::ANNOTATION:
						std::shared_ptr<CAnnotation> an = std::static_pointer_cast<CAnnotation>(child);
						//if (child->type() == CAnnotation::HISTOGRAM)
						//{
						//	model->appendPassiveChild( i1, an->getLabel(), an->id(), an->parentId(), an->infoRow());
						//}
						//else
						//{
							model->append(i1, an);
						//}
						break;
				}


			

			}

			ui.treeView->setExpanded(parentIndex, true);
		}
	}
	ui.treeView->blockSignals(false);
}


void DockWidgetWorkspace::removeItem(int id)
{
	ui.treeView->blockSignals(true);

	WorkspaceTreeModel *model = (WorkspaceTreeModel *)ui.treeView->model();

	QModelIndex index = findWorkspaceTreeModelIndex(id);

	if (index.isValid())
	{
		model->removeRow(index.row(), index.parent());
	}

	ui.treeView->blockSignals(false);
}

void DockWidgetWorkspace::colLockClicked(CModel3D* obj, WorkspaceTreeItem* clickedItem)
{
	obj->setLocked(! obj->isLocked());
	clickedItem->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), WorkspaceTreeItem::Column::colLock, obj->isLocked()));
}

void DockWidgetWorkspace::colSelfVisibilityClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem)
{
	bool vis = obj->switchSelfVisibility();
	clickedItem->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), WorkspaceTreeItem::Column::colSelfVisibility, vis));
}

void DockWidgetWorkspace::colKidsVisibilityClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem)
{
	bool vis = obj->switchKidsVisibility();
	clickedItem->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), WorkspaceTreeItem::Column::colKidsVisibility, vis));
}

void DockWidgetWorkspace::colNameClicked(std::shared_ptr<CBaseObject> obj, WorkspaceTreeItem* clickedItem)
{
	if (obj->hasType(CBaseObject::MODEL) || obj->hasType(CBaseObject::IMAGE))
	{
		if (obj->hasType(CBaseObject::IMAGE))
		{
			//AP::mainWin().activatePicViewerInstance(obj->id());
			QMdiSubWindow* window = AP::mainWin().getPicViewerInstance(obj->id());

			if (window != nullptr)
			{
				AP::mainWin().ui.mdiArea->setActiveSubWindow(window);
			}
		}
		else
		{
			AP::mainWin().activateGLViewerInstance();
		}

		bool b = clickedItem->checkState() == Qt::Checked;

		obj->setSelected(b);
		if (!AP::getWorkspace()->changeSelection(obj->id(), b))
		{
			emit currentObjectChanged(obj->id());
		}
	}
	else
	{
		bool b = clickedItem->checkState() == Qt::Checked;
		obj->setSelected(b);
		AP::getWorkspace()->changeSelection(obj->id(), b);

		//Qt::CheckState state = clickedItem->checkState();
		//if (state == Qt::Checked || state == Qt::Unchecked)
		//{
		//	obj->setSelected(state > 0);
		//}
	}
}

void DockWidgetWorkspace::onCurrentObjectChanged(int i)
{
	selectItem(i);
}

void DockWidgetWorkspace::onCurrentObjectChanged(std::shared_ptr<CBaseObject> obj)
{
	if (obj != nullptr)
		selectItem(obj->id());
	else
		selectItem(NO_CURRENT_MODEL);
}


void DockWidgetWorkspace::onTreeViewItemClicked(QModelIndex current)
{
	if (current.isValid())
	{
		WorkspaceTreeModel* model = (WorkspaceTreeModel*)ui.treeView->model();

		WorkspaceTreeItem* clickedItem = (WorkspaceTreeItem*) model->itemFromIndex(current);

		std::shared_ptr<CBaseObject> clickedObject = clickedItem->getObject();

		if (clickedObject == nullptr) return;

		switch (current.column())
		{
			case WorkspaceTreeItem::Column::colName:
				colNameClicked(clickedObject, clickedItem);
				break;
			case WorkspaceTreeItem::Column::colSelfVisibility:
				colSelfVisibilityClicked(clickedObject, clickedItem);
				break;
			case WorkspaceTreeItem::Column::colKidsVisibility:
				colKidsVisibilityClicked(clickedObject, clickedItem);
				break;
			case WorkspaceTreeItem::Column::colLock:
				colLockClicked((CModel3D*)clickedObject.get(), clickedItem);
				break;
			default:
				break;
		}

		emit(currentObjectChanged(clickedObject->id()));
		AP::EVENTS::workspaceTreeClicked(clickedObject->id());
	}
	else
	{
		emit(currentObjectChanged(NO_CURRENT_MODEL));
		AP::EVENTS::workspaceTreeClicked(NO_CURRENT_MODEL);
	}
}

inline void DockWidgetWorkspace::setupUi()
{
	if (this->objectName().isEmpty())
		this->setObjectName(QString::fromUtf8("DockWidgetWorkspace"));
	this->resize(885, 177);
	this->setMinimumSize(QSize(200, 168));
	this->setMaximumSize(QSize(524287, 524287));
	this->setFloating(false);
	this->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	ui.dockWidgetContents = new QWidget();
	ui.dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	
	ui.gridLayout = new QGridLayout(ui.dockWidgetContents);
	ui.gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	ui.gridLayout->setContentsMargins(0, 0, 0, 0);
	
	ui.treeView = new DeselectableTreeView(ui.dockWidgetContents);
	ui.treeView->setObjectName(QString::fromUtf8("treeView"));
	ui.treeView->setMouseTracking(false);
	ui.treeView->setEditTriggers(QAbstractItemView::AllEditTriggers);

	ui.gridLayout->addWidget(ui.treeView, 0, 0, 1, 1);

	this->setWidget(ui.dockWidgetContents);

	this->retranslateUi();

	QMetaObject::connectSlotsByName(this);
}

inline void DockWidgetWorkspace::retranslateUi()
{
	this->setWindowTitle(QCoreApplication::translate("DockWidgetWorkspace", "Workspace", nullptr));
}
