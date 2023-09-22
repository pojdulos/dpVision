#ifndef PROPANNOTATIONSPHERE_H
#define PROPANNOTATIONSPHERE_H

#include "propAnnotation.h"

#include "ui_propAnnotationSphere.h"

class CAnnotationSphere;

class PropAnnotationSphere : public PropWidget
{
	Q_OBJECT
	CAnnotationSphere* obj;

public:
	explicit PropAnnotationSphere(CAnnotationSphere *an, QWidget *parent = 0);
	~PropAnnotationSphere();

	static PropWidget* create(CAnnotationSphere* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:
	void changedCtrX(double);
	void changedCtrY(double);
	void changedCtrZ(double);

	void changedRadius(double);

private:
	Ui::propAnnotationSphere ui;
};

#endif // PROPANNOTATIONSPHERE_H
