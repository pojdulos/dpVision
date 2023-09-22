#ifndef PROPANNOTATIONPLANE_H
#define PROPANNOTATIONPLANE_H

#include "propAnnotation.h"

#include "ui_propAnnotationPlane.h"

class CAnnotationPlane;

class PropAnnotationPlane : public PropWidget
{
	Q_OBJECT
	CAnnotationPlane* obj;

public:
	explicit PropAnnotationPlane(CAnnotationPlane *an, QWidget *parent = 0);
	~PropAnnotationPlane();

	static PropWidget* create(CAnnotationPlane* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:
	void changedCtrX(double);
	void changedCtrY(double);
	void changedCtrZ(double);

	void changedNormX(double);
	void changedNormY(double);
	void changedNormZ(double);

	void changedSize(int);

private:
	Ui::propAnnotationPlane ui;
};

#endif // PROPANNOTATIONPLANE_H
