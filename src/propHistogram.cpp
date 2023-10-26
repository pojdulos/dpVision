#include "propHistogram.h"
#include "Histogram.h"

#include "UI.h"
#include <QVBoxLayout>

//PropHistogram::PropHistogram(CHistogram *an, QWidget *parent) : PropBaseObject( an, parent ) //: PropAnnotation( an, parent )
PropHistogram::PropHistogram(CHistogram* an, QWidget* parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);
}

PropHistogram::~PropHistogram()
{
}

PropWidget* PropHistogram::create(CHistogram* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropHistogram(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

void PropHistogram::updateProperties()
{
	//PropAnnotation::updateProperties();
	//PropBaseObject::updateProperties();

	CHistogram *a = (CHistogram*)obj;

	if (NULL != a)
	{
		ui.levels->blockSignals(true);
		ui.levels->setValue(a->getLevels());
		ui.levels->blockSignals(false);

		ui.absCheckBox->blockSignals(true);
		ui.absCheckBox->setChecked(a->getAbsValues());
		ui.absCheckBox->blockSignals(false);

		ui.lowerLimit->blockSignals(true);
		ui.lowerLimit->setValue(a->getLowerLimit());
		ui.lowerLimit->blockSignals(false);

		ui.upperLimit->blockSignals(true);
		ui.upperLimit->setValue(a->getUpperLimit());
		ui.upperLimit->blockSignals(false);

		ui.colorSchemeCombo->blockSignals(true);
		ui.colorSchemeCombo->clear();
		for (auto c : a->colorModeString) { ui.colorSchemeCombo->addItem(c); }
		ui.colorSchemeCombo->setCurrentIndex( a->getColorMode() );
		ui.colorSchemeCombo->blockSignals(false);

		ui.lblsBox->blockSignals(true);
		ui.lblsBox->setChecked(a->showLabels);
		ui.lblsBox->blockSignals(false);

		showAdditionalInfo();
	}
}

void PropHistogram::showAdditionalInfo()
{
	CHistogram *a = (CHistogram*)obj;
	QString info;
	info += "Data Min = " + QString::number(a->getDataMin()) + "\n";
	info += "Data Max = " + QString::number( a->getDataMax()) + "\n";
	info += "MeanDist.= " + QString::number(a->getMean()) + "\n";
	info += "StdDev = " + QString::number(a->getStDev()) + "\n";
	info += "Wghtd.Err.= " + QString::number(a->getWerror()) + "\n";
	info += "Wghtd.Sdv.= " + QString::number(a->getWstDev()) + "\n";
	info += "Unused data:\n";
	info += "Values under range: " + QString::number(a->m_underRange) + "\n";
	info += "Values over range: " + QString::number(a->m_overRange) + "\n";
	info += "Undefined values: " + QString::number(a->m_undefined) + "\n";

	for (int i = 0; i < a->m_values.size(); i++)
	{
		std::pair<double, double> r = a->range(i);
		char ttt[255];
		sprintf(ttt, "%3d, %lf, %lf, %zu\n", i, r.first, r.second, a->m_values[i]);
		info += ttt;
	}

	ui.textEdit->setText(info);
}

void PropHistogram::changedLevels(int x)
{
	((CHistogram*)obj)->setLevels(x);
	((CHistogram*)obj)->repaint();
	UI::updateAllViews();
}

void PropHistogram::changedABS(int x)
{
	((CHistogram*)obj)->setAbsValues(x!=0);
	((CHistogram*)obj)->repaint();
	UI::updateAllViews();
	updateProperties();
}


void PropHistogram::changedLowerLimit(double y)
{
	((CHistogram*)obj)->setLowerLimit(y);
	if (y > ((CHistogram*)obj)->getUpperLimit())
	{
		((CHistogram*)obj)->setUpperLimit(y);

		ui.upperLimit->blockSignals(true);
		ui.upperLimit->setValue(y);
		ui.upperLimit->blockSignals(false);
	}

	((CHistogram*)obj)->repaint();
	showAdditionalInfo();
	UI::updateAllViews();
}

void PropHistogram::changedUpperLimit(double z)
{
	((CHistogram*)obj)->setUpperLimit(z);
	if (z < ((CHistogram*)obj)->getLowerLimit())
	{
		((CHistogram*)obj)->setLowerLimit(z);

		ui.lowerLimit->blockSignals(true);
		ui.lowerLimit->setValue(z);
		ui.lowerLimit->blockSignals(false);
	}

	((CHistogram*)obj)->repaint();
	showAdditionalInfo();
	UI::updateAllViews();
}

void PropHistogram::colorSchemeChanged(int i)
{
	((CHistogram*)obj)->setColorMode( (CHistogram::ColorMode) i );
	((CHistogram*)obj)->repaint();
	showAdditionalInfo();
	UI::updateAllViews();
}

void PropHistogram::lblBoxChanged(bool b)
{
	((CHistogram*)obj)->showLabels = b;
	UI::DOCK::HISTOGRAM::repaint();
}

