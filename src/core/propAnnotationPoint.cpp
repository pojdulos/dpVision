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

		CVector3d v = a->getDirection();
		double len = v.length();
		v.normalize();

		ui.dirX->blockSignals(true);
		ui.dirX->setValue(v.x);
		ui.dirX->blockSignals(false);

		ui.dirY->blockSignals(true);
		ui.dirY->setValue(v.y);
		ui.dirY->blockSignals(false);

		ui.dirZ->blockSignals(true);
		ui.dirZ->setValue(v.z);
		ui.dirZ->blockSignals(false);

		ui.dirLen->blockSignals(true);
		ui.dirLen->setValue(len);
		ui.dirLen->blockSignals(false);

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
	CVector3d v(ui.dirX->value(), ui.dirY->value(), ui.dirZ->value());
	v.normalize();
	double len = ui.dirLen->value();

	obj->setDirection(v * len);
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormY(double y)
{
	CVector3d v(ui.dirX->value(), ui.dirY->value(), ui.dirZ->value());
	v.normalize();
	double len = ui.dirLen->value();

	obj->setDirection(v * len);
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormZ(double z)
{
	double len = ui.dirLen->value();
	double x = ui.dirX->value();
	double y = ui.dirY->value();

	if ((x != 0.0) && (y != 0.0)) {
		double S = x * x + y * y;
		double scale = sqrt((1.0 - z * z) / S);

		x = x * scale;
		y = y * scale;
	}
	else {
		//y = x = sqrt((1.0 - z * z) / 2.0);
		y = sqrt(1.0 - z * z);
		x = 0.0;
	}

	ui.dirX->blockSignals(true);
	ui.dirX->setValue(x);
	ui.dirX->blockSignals(false);

	ui.dirY->blockSignals(true);
	ui.dirY->setValue(y);
	ui.dirY->blockSignals(false);

	obj->setDirection(CVector3d(x, y, z) * len);
	UI::updateAllViews();
}

void PropAnnotationPoint::changedNormLen(double l)
{
	CVector3d v(ui.dirX->value(), ui.dirY->value(), ui.dirZ->value());
	double len = ui.dirLen->value();

	obj->setDirection(v*len);
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
