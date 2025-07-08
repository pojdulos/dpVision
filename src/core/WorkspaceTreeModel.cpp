#include "WorkspaceTreeModel.h"

#include "Model3D.h"
#include "Annotation.h"

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

WorkspaceTreeItem* WorkspaceTreeModel::append(QStandardItem* root, CBaseObject* obj)
{
	WorkspaceTreeItem* i1 = new WorkspaceTreeItem(obj);

	root->appendRow(i1->getFields());

	if (obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		auto& kids = ((CObject*)obj)->children();
		if (!kids.empty())
		{
			for (const auto& iter : kids)
			{
				this->append(i1, (CObject*)iter.second.get());
			}
		}

		auto& anns = ((CObject*)obj)->annotations();

		if (!anns.empty())
		{
			for (const auto& iter : anns)
			{
				this->append(i1, iter.second.get());
			}
		}
	}
	else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
	{
		auto& anns = ((CAnnotation*)obj)->annotations();

		if (!anns.empty())
		{
			for (const auto& iter : anns)
			{
				this->append(i1, iter.second.get());
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

void WorkspaceTreeModel::addModelWithChildren(CModel3D* obj)
{
	this->append(this->invisibleRootItem(), obj);
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


