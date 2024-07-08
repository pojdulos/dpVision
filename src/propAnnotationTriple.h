#ifndef PROPANNOTATIONTRIPLE_H
#define PROPANNOTATIONTRIPLE_H

#include "propAnnotation.h"

#include "ui_propAnnotationTriple.h"

class CAnnotationTriple;

class PropAnnotationTriple : public PropWidget
{
	Q_OBJECT
		CAnnotationTriple* obj;
public:
	explicit PropAnnotationTriple(CAnnotationTriple *an, QWidget *parent = 0);
	~PropAnnotationTriple();

	static PropWidget* create(CAnnotationTriple* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual void updateProperties() override;

public slots:
	void changedCtrX(double);
	void changedCtrY(double);
	void changedCtrZ(double);

	void changedNormX(double);
	void changedNormY(double);
	void changedNormZ(double);

private:
	Ui::propAnnotationTriple ui;
};

#endif // PROPANNOTATIONTRIPLE_H
