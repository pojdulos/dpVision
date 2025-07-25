#pragma once
#ifndef PROPCUSTOMWIDGET_H
#define PROPCUSTOMWIDGET_H

#include "BaseObject.h"
#include "propWidget.h"

class QColorDialog;

class PropCustomWidget : public PropWidget
{
	Q_OBJECT

	CBaseObject* obj;

public:
	explicit PropCustomWidget(CBaseObject *mesh, QWidget *parent = 0);
	~PropCustomWidget();

	static PropWidget* create(CBaseObject* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual void updateProperties() override;

public slots:

private:
};

#endif // PROPCUSTOMWIDGET_H
