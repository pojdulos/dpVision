#include "ProgressIndicator.h"

ProgressIndicator::ProgressIndicator(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.cancelButton->hide();
	actionCancelled = false;

	//connect(this->ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButton()));
}

void ProgressIndicator::setValue(int value)
{
	ui.progressBar->setValue(value);
	QCoreApplication::removePostedEvents(this);
}

void ProgressIndicator::setText(QString text)
{
	ui.workInfo->setText(text);
}

void ProgressIndicator::init(int min, int max, int val, QString text)
{
	ui.progressBar->setMinimum(min);
	ui.progressBar->setMaximum(max);
	ui.progressBar->setValue(val);

	ui.workInfo->setText(text);
	
	ui.cancelButton->hide();
	actionCancelled = false;

	this->show();
}

QPushButton* ProgressIndicator::cancelButton()
{
	return ui.cancelButton;
}

void ProgressIndicator::onCancelButton()
{
	ui.workInfo->setText("Cancelled ! Please wait...");
	actionCancelled = true;
}