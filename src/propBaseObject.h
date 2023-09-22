#ifndef PROPBASEOBJECT_H
#define PROPBASEOBJECT_H

#include "propWidget.h"
#include "ui_propBaseObject.h"

class CBaseObject;

class PropBaseObject : public PropWidget
{
	Q_OBJECT
		CBaseObject* obj;


public:

	//explicit PropBaseObject(CBaseObject *o, QWidget *parent = 0, bool vis3 = true);
	explicit PropBaseObject(CBaseObject* o, QWidget* parent = 0);
	~PropBaseObject();

	virtual void updateProperties() override;

	void setSelectionEnabled(bool b) { ui.selected->setEnabled(b); };
	void setSelectionCheckable(bool b) { ui.selected->setCheckable(b); };
	void setVisibilityCheckable(bool b) { ui.visible->setCheckable(b); };

	void setVisibilityTwoState() { ui.visible->setTristate(false); };

public slots:
	void changedLabel(QString);
	void onDescrChanged();

	void changedSelection(int);
	void changedVisibility(int);
	void changedKidsVisibility(bool);
	void slotChangeVisibility(bool);

signals:
	void signalChangedVisibility(bool);

private:
	Ui::propBaseObject ui;
};

#endif // PROPBASEOBJECT_H
