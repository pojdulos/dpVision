#pragma once

#include "dll_global.h"

#include <QtWidgets/QWidget>
#include "ui_ProgressIndicator.h"

class DPVISION_EXPORT ProgressIndicator : public QWidget
{
	Q_OBJECT

public:
	explicit ProgressIndicator(QWidget* parent = 0);
	~ProgressIndicator() {};

	void init(int min, int max, int val, QString text = QString());

	QPushButton* cancelButton();

	bool actionCancelled;

private:
	Ui::ProgressIndicator ui;

private slots:
	void onCancelButton();

public slots:
	void setValue(int value);
	void setText(QString text);
};

