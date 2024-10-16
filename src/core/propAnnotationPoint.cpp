#include "propAnnotationPoint.h"
#include "AnnotationPoint.h"

#include "UI.h"
#include <QVBoxLayout>

PropAnnotationPoint::PropAnnotationPoint(CAnnotationPoint *an, QWidget *parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationPoint::~PropAnnotationPoint()
{
}

PropWidget* PropAnnotationPoint::create(CAnnotationPoint* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropAnnotationPoint(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotationPoint::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotationPoint* m = (CAnnotationPoint*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m), new PropAnnotationPoint(m) });
}

void PropAnnotationPoint::updateProperties()
{
	//PropAnnotation::updateProperties();

	CAnnotationPoint *a = (CAnnotationPoint*)obj;

	if (NULL != a)
	{
		ui.ctrX->blockSignals(true);
		ui.ctrX->setValue(a->x);
		ui.ctrX->blockSignals(false);

		ui.ctrY->blockSignals(true);
		ui.ctrY->setValue(a->y);
		ui.ctrY->blockSignals(false);

		ui.ctrZ->blockSignals(true);
		ui.ctrZ->setValue(a->z);
		ui.ctrZ->blockSignals(false);

		ui.dirX->blockSignals(true);
		ui.dirX->setValue(a->getDirection().X());
		ui.dirX->blockSignals(false);

		ui.dirY->blockSignals(true);
		ui.dirY->setValue(a->getDirection().Y());
		ui.dirY->blockSignals(false);

		ui.dirZ->blockSignals(true);
		ui.dirZ->setValue(a->getDirection().Z());
		ui.dirZ->blockSignals(false);

		ui.showNorm->blockSignals(true);
		ui.showNorm->setChecked(a->m_showNorm);
		ui.showNorm->blockSignals(false);

	}
}


void PropAnnotationPoint::changedCtrX(double x)
{
	obj->x = x;
	UI::updateAllViews();
}

void PropAnnotationPoint::changedCtrY(double y)
{
	obj->y = y;
	UI::updateAllViews();
}

void PropAnnotationPoint::changedCtrZ(double z)
{
	obj->z = z;
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormX(double x)
{
	CVector3d n = obj->getDirection();
	n.X(x);
	obj->setDirection(n);
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormY(double y)
{
	CVector3d n = obj->getDirection();
	n.Y(y);
	obj->setDirection(n);
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormZ(double z)
{
	CVector3d n = obj->getDirection();
	n.Z(z);
	obj->setDirection(n);
	UI::updateAllViews();
}

void PropAnnotationPoint::toggledShowNorm(bool b)
{
	obj->m_showNorm = b;
	UI::updateAllViews();
}

//
//void PropAnnotationPlane::changedSize(int s)
//{
//	((CAnnotationPlane*)obj)->setSize(s);
//	UI::updateAllViews();
//}
