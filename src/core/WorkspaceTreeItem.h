#pragma once

#include "QStandardItem"
#include "Global.h"
#include "BaseObject.h"

class CAnnotation;
class CModel3D;

class DPVISION_EXPORT  WorkspaceTreeItem : public QStandardItem
{
public:
	enum Column {
		colName = 0,
		colSelfVisibility = 1,
		colKidsVisibility = 2,
		colLock = 3
	};

	explicit WorkspaceTreeItem() : QStandardItem() {};
	explicit WorkspaceTreeItem(QString name) : QStandardItem( name ) {};
	explicit WorkspaceTreeItem(std::shared_ptr<CBaseObject> obj);

	void changeIcon(Column c, bool b);

	static QIcon getNewIcon(CBaseObject::Type t, Column c, bool b);

	void setObject(std::shared_ptr<CBaseObject> obj);
	
	std::shared_ptr<CBaseObject> getObject();

	int getObjectId();

	//inline void setId(int id) { setData( id, Qt::UserRole + 1 ); };
	//inline int getId() { return data( Qt::UserRole + 1 ).value<int>(); }

	QList<QStandardItem*> getFields();
	WorkspaceTreeItem* getField(Column c);

private:
	QMap<int, WorkspaceTreeItem*> m_Fields;

protected:
	WorkspaceTreeItem* createVisibleColumn(std::shared_ptr<CBaseObject> obj);
	WorkspaceTreeItem* createKidsVisibleColumn(std::shared_ptr<CBaseObject> obj);
	WorkspaceTreeItem* createLockColumn(std::shared_ptr<CBaseObject> obj);
};

