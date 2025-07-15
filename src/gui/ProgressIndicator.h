#pragma once

#include "dll_global.h"
#include <QDebug>
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

private:
	Ui::ProgressIndicator ui;

public slots:
	void onInit(int min, int max, int val, QString text = "") { init(min, max, val, text); }
	void onSetValue(int value) { setValue(value); }
	void onSetText(QString text) { setText(text); };
	void onHide() { hide(); }

	// Dla zgodnoœci ze starym API:
	void setValue(int value);
	void setText(QString text);
};

