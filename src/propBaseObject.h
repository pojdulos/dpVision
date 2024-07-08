#ifndef PROPBASEOBJECT_H
#define PROPBASEOBJECT_H

#include "propWidget.h"
#include "ui_propBaseObject.h"

#include "FlowLayout.h"

class CBaseObject;

class PropBaseObject : public PropWidget
{
	Q_OBJECT
	CBaseObject* obj;
	FlowLayout* keywords_layout;

	static bool group_visible;
public:

	//explicit PropBaseObject(CBaseObject *o, QWidget *parent = 0, bool vis3 = true);
	explicit PropBaseObject(CBaseObject* o, QWidget* parent = 0);
	~PropBaseObject();

	virtual void updateProperties() override;

	virtual QString treeItemLabel() override;

	void updateKeywords();

	void setSelectionEnabled(bool b) { ui.selected->setEnabled(b); };
	void setSelectionCheckable(bool b) { ui.selected->setCheckable(b); };
	void setVisibilityCheckable(bool b) { ui.visible->setCheckable(b); };

	void setVisibilityTwoState() { ui.visible->setTristate(false); };

	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

public slots:
	void changedLabel(QString);
	void onDescrChanged();

	void addKeywordToWidget(QString keyword);
	void onAddKeywordButtonClick();

	void changedSelection(int);
	void changedVisibility(int);
	void changedKidsVisibility(bool);
	void slotChangeVisibility(bool);

	void deleteKeyword();

signals:
	void signalChangedVisibility(bool);

private:
	Ui::propBaseObject ui;
};

#endif // PROPBASEOBJECT_H
