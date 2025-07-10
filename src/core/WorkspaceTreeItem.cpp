#include "WorkspaceTreeItem.h"
#include "AP.h"
#include "Model3D.h"

WorkspaceTreeItem::WorkspaceTreeItem(std::shared_ptr<CBaseObject> obj) : QStandardItem()
{
	if (obj != nullptr)
	{
		QStandardItem::setText(obj->getLabel());

		this->setObject(obj);

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

				m_Fields[Column::colLock] = createLockColumn(obj);
				m_Fields[Column::colLock]->setIcon(WorkspaceTreeItem::getNewIcon((CBaseObject::Type)obj->type(), Column::colLock, std::static_pointer_cast<CModel3D>(obj)->isLocked()));
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
	WorkspaceTreeItem* field = this->getField(c);
	std::shared_ptr<CBaseObject> obj = this->getObject();

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


void WorkspaceTreeItem::setObject(std::shared_ptr<CBaseObject> obj)
{
	this->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(obj), Qt::UserRole);
	this->setData(QVariant::fromValue(obj->id()), Qt::UserRole+1);
}

std::shared_ptr<CBaseObject> WorkspaceTreeItem::getObject()
{
	return this->data(Qt::UserRole).value<std::shared_ptr<CBaseObject>>();
}

int WorkspaceTreeItem::getObjectId()
{
	return this->data(Qt::UserRole+1).value<int>();
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

WorkspaceTreeItem* WorkspaceTreeItem::createVisibleColumn(std::shared_ptr<CBaseObject> obj)
{
	WorkspaceTreeItem* m_V = new WorkspaceTreeItem();
	m_V->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_V->setObject(obj);
	m_V->setToolTip("change own visibility");
	return m_V;
}

WorkspaceTreeItem* WorkspaceTreeItem::createKidsVisibleColumn(std::shared_ptr<CBaseObject> obj)
{
	WorkspaceTreeItem* m_V = new WorkspaceTreeItem();
	m_V->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_V->setObject(obj);
	m_V->setToolTip("change kids visibility");
	return m_V;
}

WorkspaceTreeItem* WorkspaceTreeItem::createLockColumn(std::shared_ptr<CBaseObject> obj)
{
	WorkspaceTreeItem* m_L = new WorkspaceTreeItem();
	m_L->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_L->setObject(obj);
	m_L->setToolTip("lock / unlock");
	return m_L;
}

