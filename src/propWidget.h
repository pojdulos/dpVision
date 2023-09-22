#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QtWidgets/QWidget>

class PropWidget : public QWidget
{
public:
	explicit PropWidget(QWidget *parent = 0) : QWidget(parent) { /*updateProperties();*/ };
	~PropWidget() {};

	//static PropWidget* create(QWidget *parent = 0) {};

	//virtual void paintEvent(QPaintEvent * event) override ;
	virtual void updateProperties();

	static PropWidget* build(QVector<PropWidget*> content, QWidget* parent = 0);
};

#endif // PROPWIDGET_H
