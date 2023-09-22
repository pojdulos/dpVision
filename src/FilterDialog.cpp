#include "FilterDialog.h"

#include "QDoubleSpinBox"

FilterDialog::FilterDialog(Filter* fltr, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_Filter(fltr)
{
	ui.setupUi(this);

	//while (QDoubleSpinBox* w = ui.kernelGrid->findChild<QDoubleSpinBox*>())
	//	delete w;

	for (int r = 0; r < m_Filter->getSize(); r++)
		for (int c = 0; c < m_Filter->getSize(); c++)
		{
			QDoubleSpinBox* spin = new QDoubleSpinBox();

			double& i = (*m_Filter)(r, c);
			spin->setValue(i);
			spin->setObjectName("Spin_R" + QString::number(r) + "_C" + QString::number(c));
			QObject::connect(spin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged(double)));
			ui.kernelGrid->addWidget(spin, r, c);
		}
}


void FilterDialog::spinValueChanged(double val)
{
	QDoubleSpinBox* spin = (QDoubleSpinBox*)sender();
	QString name = spin->objectName();

	int r = QString(name[6]).toDouble();
	int c = QString(name[9]).toDouble();

	(*m_Filter)(r, c) = val;
}