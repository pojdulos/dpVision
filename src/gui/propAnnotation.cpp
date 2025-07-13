#include "propAnnotation.h"
#include "Annotation.h"
#include "UI.h"
#include <QVBoxLayout>

PropAnnotation::PropAnnotation(CAnnotation *a, QWidget *parent) : PropWidget( parent )
{
	obj = a;
	ui.setupUi(this);
}

PropAnnotation::~PropAnnotation()
{
}


PropWidget* PropAnnotation::create(CAnnotation* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotation::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotation* m = (CAnnotation*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m) });
}

void PropAnnotation::updateProperties()
{
	//PropBaseObject::updateProperties();

	CAnnotation *a = (CAnnotation*) obj;

	updateColorButton(a->getColor());
	updateSelColorButton(a->getSelColor());

	//QString s("background-color: rgb(" + QString::number(a->getColor().R()) + ", " + QString::number(a->getColor().G()) + ", " + QString::number(a->getColor().B()) + ");");
	//ui.colorGroupBox->setStyleSheet(s);

	//ui.colorR->blockSignals(true);
	//ui.colorR->setValue(a->getColor().R());
	//ui.colorR->blockSignals(false);

	//ui.colorG->blockSignals(true);
	//ui.colorG->setValue(a->getColor().G());
	//ui.colorG->blockSignals(false);

	//ui.colorB->blockSignals(true);
	//ui.colorB->setValue(a->getColor().B());
	//ui.colorB->blockSignals(false);

	//ui.colorA->blockSignals(true);
	//ui.colorA->setValue(a->getColor().A());
	//ui.colorA->blockSignals(false);

	//ui.selR->blockSignals(true);
	//ui.selR->setValue(a->getSelColor().R());
	//ui.selR->blockSignals(false);

	//ui.selG->blockSignals(true);
	//ui.selG->setValue(a->getSelColor().G());
	//ui.selG->blockSignals(false);

	//ui.selB->blockSignals(true);
	//ui.selB->setValue(a->getSelColor().B());
	//ui.selB->blockSignals(false);

	//ui.selA->blockSignals(true);
	//ui.selA->setValue(a->getSelColor().A());
	//ui.selA->blockSignals(false);

}


//void PropAnnotation::changedColR(int c)
//{
//	((CAnnotation*)obj)->getColor().R(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedColG(int c)
//{
//	((CAnnotation*)obj)->getColor().G(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedColB(int c)
//{
//	((CAnnotation*)obj)->getColor().B(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedColA(int c)
//{
//	((CAnnotation*)obj)->getColor().A(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedSelR(int c)
//{
//	((CAnnotation*)obj)->getSelColor().R(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedSelG(int c)
//{
//	((CAnnotation*)obj)->getSelColor().G(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedSelB(int c)
//{
//	((CAnnotation*)obj)->getSelColor().B(c);
//	UI::updateAllViews();
//}
//
//void PropAnnotation::changedSelA(int c)
//{
//	((CAnnotation*)obj)->getSelColor().A(c);
//	UI::updateAllViews();
//}

#include <QColorDialog>

inline QColor fromCRGBA(CRGBA col) { return QColor(col.R(), col.G(), col.B(), col.A()); }
inline CRGBA fromQColor(QColor col) { return CRGBA((unsigned char)col.red(), col.green(), col.blue(), col.alpha()); }

void PropAnnotation::colorButtonPressed()
{
	CRGBA &col = ((CAnnotation*)obj)->getColor();
	
	QColor color = QColorDialog::getColor( fromCRGBA(col), this, "Select color", QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
	if (color.isValid())
	{
		updateColorButton(col = fromQColor(color));
		UI::updateAllViews();
	}
}

void PropAnnotation::selcolorButtonPressed()
{
	CRGBA &col = ((CAnnotation*)obj)->getSelColor();

	QColor color = QColorDialog::getColor(fromCRGBA(col), this, "Select color", QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
	if (color.isValid())
	{
		updateSelColorButton(col = fromQColor(color));
		UI::updateAllViews();
	}
}

void PropAnnotation::updateColorButton(CRGBA col)
{
	QString s("background-color: rgb(" + QString::number(col.R()) + ", " + QString::number(col.G()) + ", " + QString::number(col.B()) + ");");
	ui.colorButton->setStyleSheet(s);
}

void PropAnnotation::updateSelColorButton(CRGBA col)
{
	QString s("background-color: rgb(" + QString::number(col.R()) + ", " + QString::number(col.G()) + ", " + QString::number(col.B()) + ");");
	ui.selcolorButton->setStyleSheet(s);
}

