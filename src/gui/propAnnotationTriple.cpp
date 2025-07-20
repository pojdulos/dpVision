#include "propAnnotationTriple.h"
#include "AnnotationTriple.h"

#include "../api/UI.h"
#include <QVBoxLayout>

PropAnnotationTriple::PropAnnotationTriple(CAnnotationTriple *an, QWidget *parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationTriple::~PropAnnotationTriple()
{
}

PropWidget* PropAnnotationTriple::create(CAnnotationTriple* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropAnnotationTriple(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotationTriple::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotationTriple* m = (CAnnotationTriple*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m), new PropAnnotationTriple(m) });
}

void PropAnnotationTriple::updateProperties()
{
	//PropAnnotation::updateProperties();

	CAnnotationTriple *a = (CAnnotationTriple*)obj;

	if (NULL != a)
	{
		ui.ctrX->blockSignals(true);
		ui.ctrX->setValue(a->origin().X());
		ui.ctrX->blockSignals(false);

		ui.ctrY->blockSignals(true);
		ui.ctrY->setValue(a->origin().Y());
		ui.ctrY->blockSignals(false);

		ui.ctrZ->blockSignals(true);
		ui.ctrZ->setValue(a->origin().Z());
		ui.ctrZ->blockSignals(false);

		ui.normX->blockSignals(true);
		ui.normX->setValue(a->rot().X());
		ui.normX->blockSignals(false);

		ui.normY->blockSignals(true);
		ui.normY->setValue(a->rot().Y());
		ui.normY->blockSignals(false);

		ui.normZ->blockSignals(true);
		ui.normZ->setValue(a->rot().Z());
		ui.normZ->blockSignals(false);
	}
}


void PropAnnotationTriple::changedCtrX(double x)
{
	((CAnnotationTriple*)obj)->origin().X(x);
	UI::updateAllViews();
}

void PropAnnotationTriple::changedCtrY(double y)
{
	((CAnnotationTriple*)obj)->origin().Y(y);
	UI::updateAllViews();
}

void PropAnnotationTriple::changedCtrZ(double z)
{
	((CAnnotationTriple*)obj)->origin().Z(z);
	UI::updateAllViews();
}

void PropAnnotationTriple::changedNormX(double x)
{
	((CAnnotationTriple*)obj)->rot().X(x);
	UI::updateAllViews();
}

void PropAnnotationTriple::changedNormY(double y)
{
	((CAnnotationTriple*)obj)->rot().Y(y);
	UI::updateAllViews();
}

void PropAnnotationTriple::changedNormZ(double z)
{
	((CAnnotationTriple*)obj)->rot().Z(z);
	UI::updateAllViews();
}
