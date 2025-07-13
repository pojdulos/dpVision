#ifndef VOLTKCOLORWIDGET_H
#define VOLTKCOLORWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_volTKcolorWidget.h"

class VolTKcolorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit VolTKcolorWidget(QWidget *parent = 0);
	~VolTKcolorWidget() {};

	void setWinMax(int winMax);
	int winMax();
	void setColor(QColor color);
	QColor color();
	void setActive(bool active);

	bool isActive();

signals:
	void imModyfied();
	void removeMe();

private slots:
	void winMaxSpinBoxValueChanged(int);
	void activeCheckBoxStateChanged(int);
	void colorButtonPressed();
	void removeButtonPressed();
private:
	Ui::volTKcolorWidget ui;
};

#endif // VOLTKCOLORWIDGET_H
