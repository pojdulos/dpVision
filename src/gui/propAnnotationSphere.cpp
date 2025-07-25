#include "propAnnotationSphere.h"
#include "AnnotationSphere.h"

#include "../api/UI.h"
#include <QVBoxLayout>

PropAnnotationSphere::PropAnnotationSphere(CAnnotationSphere *an, QWidget *parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationSphere::~PropAnnotationSphere()
{
}

PropWidget* PropAnnotationSphere::create(CAnnotationSphere* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropAnnotationSphere(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotationSphere::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotationSphere* m = (CAnnotationSphere*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m), new PropAnnotationSphere(m) });
}

void PropAnnotationSphere::updateProperties()
{
	//PropAnnotation::updateProperties();

	CAnnotationSphere *a = (CAnnotationSphere*)obj;

	if (NULL != a)
	{
		ui.ctrX->blockSignals(true);
		ui.ctrX->setValue(a->center().X());
		ui.ctrX->blockSignals(false);

		ui.ctrY->blockSignals(true);
		ui.ctrY->setValue(a->center().Y());
		ui.ctrY->blockSignals(false);

		ui.ctrZ->blockSignals(true);
		ui.ctrZ->setValue(a->center().Z());
		ui.ctrZ->blockSignals(false);

		ui.radius->blockSignals(true);
		ui.radius->setValue(a->radius());
		ui.radius->blockSignals(false);
	}
}


void PropAnnotationSphere::changedCtrX(double x)
{
	((CAnnotationSphere*)obj)->center().x = x;
	UI::updateAllViews();
}

void PropAnnotationSphere::changedCtrY(double y)
{
	((CAnnotationSphere*)obj)->center().y = y;
	UI::updateAllViews();
}

void PropAnnotationSphere::changedCtrZ(double z)
{
	((CAnnotationSphere*)obj)->center().z = z;
	UI::updateAllViews();
}

void PropAnnotationSphere::changedRadius(double x)
{
	((CAnnotationSphere*)obj)->radius() = x;
	UI::updateAllViews();
}
