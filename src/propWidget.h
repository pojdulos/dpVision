#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QtWidgets/QWidget>

class CBaseObject;

class PropWidget : public QWidget
{
public:
	explicit PropWidget(QWidget *parent = 0);
	~PropWidget() {};

	virtual void updateProperties();

	static PropWidget* build(QVector<PropWidget*> content, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual QString treeItemLabel();
};

#endif // PROPWIDGET_H
