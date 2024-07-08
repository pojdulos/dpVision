#include "WorkspaceTreeItem.h"
#include "AP.h"
#include "Model3D.h"

WorkspaceTreeItem::WorkspaceTreeItem(CBaseObject* obj) : QStandardItem()
{
	if (obj != nullptr)
	{
		QStandardItem::setText(obj->getLabel());

		setObject(obj);

		//	setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
		setCheckable(true);
		setCheckState(obj->isSelected() ? Qt::Checked : Qt::Unchecked);

		setToolTip(QString::fromStdWString(obj->infoRow()));
	
		if (obj->hasCategory(CBaseObject::Category::OBJECT))
		{
			if (obj->hasType(CBaseObject::Type::MODEL))
			{
				m_Fields[Column::colSelfVisibility] = createVisibleColumn(obj);
				m_Fields[Column::colSelfVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colSelfVisibility, obj->getSelfVisibility()));

				m_Fields[Column::colKidsVisibility] = createKidsVisibleColumn(obj);
				m_Fields[Column::colKidsVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colKidsVisibility, obj->getKidsVisibility()));

				m_Fields[Column::colLock] = createLockColumn((CModel3D*)obj);
				m_Fields[Column::colLock]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colLock, ((CModel3D*)obj)->isLocked()));
			}
			else
			{
				m_Fields[Column::colSelfVisibility] = createVisibleColumn(obj);
				m_Fields[Column::colSelfVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colSelfVisibility, obj->getSelfVisibility()));

				m_Fields[Column::colKidsVisibility] = createKidsVisibleColumn(obj);
				m_Fields[Column::colKidsVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colKidsVisibility, obj->getKidsVisibility()));
			}
		}
		else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
		{
			m_Fields[Column::colSelfVisibility] = createVisibleColumn(obj);
			m_Fields[Column::colSelfVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colSelfVisibility, obj->getSelfVisibility()));

			m_Fields[Column::colKidsVisibility] = createKidsVisibleColumn(obj);
			m_Fields[Column::colKidsVisibility]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colKidsVisibility, obj->getKidsVisibility()));
		}
	}
}

void WorkspaceTreeItem::changeIcon(Column c, bool b)
{
	WorkspaceTreeItem* field = getField(c);
	CBaseObject* obj = getObject();

	if (field != nullptr)
	{
		field->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), c, b));
	}
}


QIcon WorkspaceTreeItem::getNewIcon(CBaseObject::Type t, Column c, bool b)
{
	switch (c)
	{
	case Column::colSelfVisibility:
		if (t == CBaseObject::Type::MODEL)
			return QIcon(b ? ":/icons/VisibleMatrix.ico" : ":/icons/HiddenMatrix.ico");
		else
			return QIcon(b ? ":/icons/Visible.ico" : ":/icons/Hidden.ico");
		break;
	case Column::colKidsVisibility:
		return QIcon(b ? ":/icons/VisibleKids.ico" : ":/icons/HiddenKids.ico");
		break;
	case Column::colLock:
		return QIcon(b ? ":/icons/Lock.ico" : ":/icons/Unlock.ico");
		break;
	}
	return QIcon();
}


void WorkspaceTreeItem::setObject(CBaseObject* obj)
{
	setData(QVariant::fromValue(obj->id()), Qt::UserRole);
	setData(QVariant::fromValue(obj), Qt::UserRole + 1);
}

CBaseObject* WorkspaceTreeItem::getObject()
{
	return data(Qt::UserRole + 1).value<CBaseObject*>();
}

QList<QStandardItem*> WorkspaceTreeItem::getFields()
{
	QList<QStandardItem*> cols;

	cols << this;
	for (const auto& f : m_Fields) cols << f;

	return cols;
}

WorkspaceTreeItem* WorkspaceTreeItem::getField(Column c)
{
	if (m_Fields.contains(c))
		return (WorkspaceTreeItem*)m_Fields[c];
	else
		return nullptr;
}

WorkspaceTreeItem* WorkspaceTreeItem::createVisibleColumn(CBaseObject* obj)
{
	WorkspaceTreeItem* m_V = new WorkspaceTreeItem();
	m_V->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_V->setObject(obj);
	m_V->setToolTip("change own visibility");
	return m_V;
}

WorkspaceTreeItem* WorkspaceTreeItem::createKidsVisibleColumn(CBaseObject* obj)
{
	WorkspaceTreeItem* m_V = new WorkspaceTreeItem();
	m_V->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_V->setObject(obj);
	m_V->setToolTip("change kids visibility");
	return m_V;
}

WorkspaceTreeItem* WorkspaceTreeItem::createLockColumn(CModel3D* obj)
{
	WorkspaceTreeItem* m_L = new WorkspaceTreeItem();
	m_L->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_L->setObject(obj);
	m_L->setToolTip("lock / unlock");
	return m_L;
}

