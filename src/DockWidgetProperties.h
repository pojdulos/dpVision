#ifndef DOCKWIDGETPROPERTIES_H
#define DOCKWIDGETPROPERTIES_H

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetProperties.h"

#include "Wektor3D.h"

class PropWidget;
class QVBoxLayout;
class QFormLayout;
class QScrollArea;
class CBaseObject;

class DockWidgetProperties : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetProperties(QWidget *parent = 0);
	~DockWidgetProperties();

	virtual void paintEvent(QPaintEvent * event) override;

	void selectionChanged( int id );
	void updateProperties();

	void addExpandableItems(QVector<PropWidget*> submodels);

	void addExpandableItem(QString title, PropWidget* contentWidget);

public slots:
	void onCurrentObjectChanged(int);
	void onCurrentObjectChanged(CBaseObject*);

private:
	Ui::DockWidgetProperties ui;
};

#endif // DOCKWIDGETPROPERTIES_H
