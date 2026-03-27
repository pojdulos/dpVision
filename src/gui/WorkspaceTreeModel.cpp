#include "WorkspaceTreeModel.h"

#include "Model3D.h"
#include "Annotation.h"
#include <QMimeData>

WorkspaceTreeModel::WorkspaceTreeModel(QObject *parent) : QStandardItemModel(parent)
{
	initialize();
}

WorkspaceTreeModel::~WorkspaceTreeModel()
{
}

void WorkspaceTreeModel::initialize()
{
	setHorizontalHeaderLabels(QStringList() << tr("name") << tr("") << tr("") << tr(""));
	//for (int column = 1; column < columnCount(); ++column)
	//	horizontalHeaderItem(column)->setTextAlignment(	Qt::AlignVCenter | Qt::AlignLeft );
}

WorkspaceTreeItem* WorkspaceTreeModel::append(QStandardItem* root, std::shared_ptr<CBaseObject> obj)
{
	WorkspaceTreeItem* i1 = new WorkspaceTreeItem(obj);

	root->appendRow(i1->getFields());

	if (obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		std::shared_ptr<CObject> object = std::static_pointer_cast<CObject>(obj);
		if (!object->children().empty())
		{
			for (int id : object->orderedChildIds())
			{
				if (std::shared_ptr<CBaseObject> child = object->getChild(id))
				{
					this->append(i1, child);
				}
			}
		}

		if (!object->annotations().empty())
		{
			for (int id : object->orderedAnnotationIds())
			{
				if (CAnnotation* annotation = object->annotation(id))
				{
					this->append(i1, annotation->shared_from_this());
				}
			}
		}
	}
	else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
	{
		std::shared_ptr<CAnnotation> annotationObject = std::static_pointer_cast<CAnnotation>(obj);
		if (!annotationObject->annotations().empty())
		{
			for (int id : annotationObject->orderedAnnotationIds())
			{
				if (CAnnotation* annotation = annotationObject->annotation(id))
				{
					this->append(i1, annotation->shared_from_this());
				}
			}
		}
	}

	return i1;
}


//WorkspaceTreeItem* WorkspaceTreeModel::append(QStandardItem* root, CModel3D *obj, bool hideable, bool lockable, bool checkable, bool checked)
//{
//	WorkspaceTreeItem* i1 = new WorkspaceTreeItem(obj);
//
//	root->appendRow(i1->getFields());
//
//
//
//	return i1;
//}

//WorkspaceTreeItem* WorkspaceTreeModel::appendAnnotationChild(QStandardItem* root, CAnnotation *an)
//{
//	WorkspaceTreeItem* i1 = new WorkspaceTreeItem( an );
//
//	root->appendRow(i1->getFields());
//
//	if (!an->annotations().empty())
//	{
//		for (const auto& iter : an->annotations())
//		{
//			this->appendAnnotationChild(i1, iter.second);
//		}
//	}
//
//	return i1;
//}

//WorkspaceTreeItem* WorkspaceTreeModel::appendDataChild(QStandardItem* root, CObject* obj)
//{
//	WorkspaceTreeItem* i1 = new WorkspaceTreeItem(obj);
//
//	root->appendRow(i1->getFields());
//
//	if (!obj->children().empty())
//	{
//		for (const auto &iter : obj->children())
//		{
//			if (iter.second->hasCategory(CBaseObject::Category::OBJECT))
//			{
//				this->appendDataChild(i1, (CObject*)iter.second);
//			}
//		}
//	}
//
//	if (!obj->annotations().empty())
//	{
//		for (const auto& iter : obj->annotations())
//		{
//			this->appendAnnotationChild(i1, iter.second);
//		}
//	}
//
//	return i1;
//}

void WorkspaceTreeModel::addModelWithChildren(std::shared_ptr<CModel3D> obj)
{
	this->append(this->invisibleRootItem(), obj);
}

QStringList WorkspaceTreeModel::mimeTypes() const
{
	return QStringList() << "application/x-dpvision-workspace-item";
}

QMimeData* WorkspaceTreeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();

	for (const QModelIndex& index : indexes)
	{
		if (index.column() != 0)
		{
			continue;
		}

		const int id = index.data(Qt::UserRole + 1).toInt();
		if (id != 0)
		{
			mimeData->setData("application/x-dpvision-workspace-item", QByteArray::number(id));
			break;
		}
	}

	return mimeData;
}

//void WorkspaceTreeModel::addModelWithChildren(CModel3D * obj)
//{
//	WorkspaceTreeItem* i1 = this->append(this->invisibleRootItem(), obj, true, true, true, obj->isSelected());
//
//	// transformacja
//	// this->appendPassiveChild(i1, L"transformation", 0, obj->getTransform().infoRow());
//
//	std::vector<unsigned int> ids = obj->getChildrenIds();
//	for (std::vector<unsigned int>::iterator iid = ids.begin(); iid != ids.end(); iid++)
//	{
//		CBaseObject* child = obj->getChild(*iid);
//		if (child->hasCategory(CBaseObject::Category::OBJECT))
//		{
//			this->appendDataChild(i1, (CObject*)child);
//		}
//	}
//
//	CModel3D::Annotations::iterator ip;
//	for (ip = obj->annotations().begin(); ip != obj->annotations().end(); ip++)
//	{
//		this->appendAnnotationChild( i1, (*ip).second );
//	}
//}


