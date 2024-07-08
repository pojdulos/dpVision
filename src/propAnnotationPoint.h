#ifndef PROPANNOTATIONPOINT_H
#define PROPANNOTATIONPOINT_H

#include "propAnnotation.h"

#include "ui_propAnnotationPoint.h"

class CAnnotationPoint;

class PropAnnotationPoint : public PropWidget
{
	Q_OBJECT
	CAnnotationPoint* obj;

public:
	explicit PropAnnotationPoint(CAnnotationPoint *an, QWidget *parent = 0);
	~PropAnnotationPoint();

	static PropWidget* create(CAnnotationPoint* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual void updateProperties() override;

public slots:
	void changedCtrX(double);
	void changedCtrY(double);
	void changedCtrZ(double);

	void changedNormX(double x);
	void changedNormY(double y);
	void changedNormZ(double z);

	void toggledShowNorm(bool b);

	//void changedSize(int);

private:
	Ui::propAnnotationPoint ui;
};

#endif // PROPANNOTATIONPOINT_H
