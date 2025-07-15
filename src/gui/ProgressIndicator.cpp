#include "ProgressIndicator.h"

ProgressIndicator::ProgressIndicator(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.cancelButton->hide();
}

void ProgressIndicator::setValue(int value)
{
	ui.progressBar->setValue(value);
	QCoreApplication::removePostedEvents(this);
}

void ProgressIndicator::setText(QString text)
{
	ui.workInfo->setText(text);
	ui.workInfo->update();
}

void ProgressIndicator::init(int min, int max, int val, QString text)
{
	ui.progressBar->setMinimum(min);
	ui.progressBar->setMaximum(max);
	ui.progressBar->setValue(val);

	ui.workInfo->setText(text);
	
	this->show();
}

QPushButton* ProgressIndicator::cancelButton()
{
	return ui.cancelButton;
}

