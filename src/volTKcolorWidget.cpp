#include "volTKcolorWidget.h"
#include <QColorDialog>

VolTKcolorWidget::VolTKcolorWidget(QWidget* parent):QWidget(parent)
{
	ui.setupUi((QWidget*)this);

	if (parent != 0)
	{
		connect(this, SIGNAL(imModyfied()), parent, SLOT(updateColorFilters()));
		connect(this, SIGNAL(removeMe()), parent, SLOT(removeFilter()));
	}
}

void VolTKcolorWidget::colorButtonPressed()
{
	QColor color = QColorDialog::getColor(QColor(ui.colorButton->text()), nullptr, "", QColorDialog::ShowAlphaChannel);
	
	QString c = color.name(QColor::HexArgb);
	
	ui.colorButton->setStyleSheet("background-color: " + c + ";");
	ui.colorButton->setText(c);
	emit imModyfied();
}

void VolTKcolorWidget::winMaxSpinBoxValueChanged(int)
{
	emit imModyfied();
}

void VolTKcolorWidget::activeCheckBoxStateChanged(int)
{
	emit imModyfied();
}

void VolTKcolorWidget::setWinMax(int winMax)
{
	ui.winMaxSpinBox->blockSignals(true);
	ui.winMaxSpinBox->setValue(winMax);
	ui.winMaxSpinBox->blockSignals(false);
}

int VolTKcolorWidget::winMax()
{
	return ui.winMaxSpinBox->value();
}

void VolTKcolorWidget::setColor(QColor color)
{
	QString c = color.name(QColor::HexArgb);
	ui.colorButton->blockSignals(true);
	ui.colorButton->setStyleSheet("background-color: " + c + ";");
	ui.colorButton->setText(c);
	ui.colorButton->blockSignals(false);
}

QColor VolTKcolorWidget::color()
{
	return QColor(ui.colorButton->text());
}

void VolTKcolorWidget::setActive(bool active)
{
	ui.activeCheckBox->blockSignals(true);
	ui.activeCheckBox->setChecked(active);
	ui.activeCheckBox->blockSignals(false);
}

bool VolTKcolorWidget::isActive()
{
	return ui.activeCheckBox->isChecked();
}

void VolTKcolorWidget::removeButtonPressed()
{
	emit removeMe();
}
