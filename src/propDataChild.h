#ifndef PROPDATACHILD_H
#define PROPDATACHILD_H

#include "propBaseObject.h"

class CBaseObject;

class PropDataChild : public PropWidget
{
//	Q_OBJECT
	CBaseObject* obj;

public:
	explicit PropDataChild(CBaseObject *m, QWidget *parent = 0);
	~PropDataChild();

	static PropWidget* create(CBaseObject* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:

};

#endif // PROPDATACHILD_H
