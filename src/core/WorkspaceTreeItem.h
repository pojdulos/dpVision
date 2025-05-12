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
	explicit WorkspaceTreeItem(CBaseObject *obj);

	void changeIcon(Column c, bool b);

	static QIcon getNewIcon(CBaseObject::Type t, Column c, bool b);

	void setObject(CBaseObject* obj);
	
	CBaseObject* getObject();

	//inline void setId(int id) { setData( id, Qt::UserRole + 1 ); };
	//inline int getId() { return data( Qt::UserRole + 1 ).value<int>(); }

	QList<QStandardItem*> getFields();
	WorkspaceTreeItem* getField(Column c);

private:
	QMap<int, WorkspaceTreeItem*> m_Fields;

protected:
	WorkspaceTreeItem* createVisibleColumn(CBaseObject* obj);
	WorkspaceTreeItem* createKidsVisibleColumn(CBaseObject* obj);
	WorkspaceTreeItem* createLockColumn(CModel3D* obj);
};

