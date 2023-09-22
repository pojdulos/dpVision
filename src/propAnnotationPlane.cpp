#include "propAnnotationPlane.h"
#include "AnnotationPlane.h"

#include "UI.h"
#include <QVBoxLayout>

PropAnnotationPlane::PropAnnotationPlane(CAnnotationPlane *an, QWidget *parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationPlane::~PropAnnotationPlane()
{
}

PropWidget* PropAnnotationPlane::create(CAnnotationPlane* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropAnnotationPlane(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

void PropAnnotationPlane::updateProperties()
{
	//PropAnnotation::updateProperties();

	CAnnotationPlane *a = (CAnnotationPlane*)obj;

	if (NULL != a)
	{
		ui.ctrX->blockSignals(true);
		ui.ctrX->setValue(a->getCenter().X());
		ui.ctrX->blockSignals(false);

		ui.ctrY->blockSignals(true);
		ui.ctrY->setValue(a->getCenter().Y());
		ui.ctrY->blockSignals(false);

		ui.ctrZ->blockSignals(true);
		ui.ctrZ->setValue(a->getCenter().Z());
		ui.ctrZ->blockSignals(false);

		ui.normX->blockSignals(true);
		ui.normX->setValue(a->getNormal().X());
		ui.normX->blockSignals(false);

		ui.normY->blockSignals(true);
		ui.normY->setValue(a->getNormal().Y());
		ui.normY->blockSignals(false);

		ui.normZ->blockSignals(true);
		ui.normZ->setValue(a->getNormal().Z());
		ui.normZ->blockSignals(false);

		ui.size->blockSignals(true);
		ui.size->setValue(a->getSize());
		ui.size->blockSignals(false);

	}
}


void PropAnnotationPlane::changedCtrX(double x)
{
	CPoint3d c = ((CAnnotationPlane*)obj)->getCenter();
	c.X(x);
	((CAnnotationPlane*)obj)->setCenter(c);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedCtrY(double y)
{
	CPoint3d c = ((CAnnotationPlane*)obj)->getCenter();
	c.Y(y);
	((CAnnotationPlane*)obj)->setCenter(c);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedCtrZ(double z)
{
	CPoint3d c = ((CAnnotationPlane*)obj)->getCenter();
	c.Z(z);
	((CAnnotationPlane*)obj)->setCenter(c);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedNormX(double x)
{
	CVector3d n = ((CAnnotationPlane*)obj)->getNormal();
	n.X(x);
	((CAnnotationPlane*)obj)->setNormal(n);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedNormY(double y)
{
	CVector3d n = ((CAnnotationPlane*)obj)->getNormal();
	n.Y(y);
	((CAnnotationPlane*)obj)->setNormal(n);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedNormZ(double z)
{
	CVector3d n = ((CAnnotationPlane*)obj)->getNormal();
	n.Z(z);
	((CAnnotationPlane*)obj)->setNormal(n);
	UI::updateAllViews();
}

void PropAnnotationPlane::changedSize(int s)
{
	((CAnnotationPlane*)obj)->setSize(s);
	UI::updateAllViews();
}
