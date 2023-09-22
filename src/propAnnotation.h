#ifndef PROPANNOTATION_H
#define PROPANNOTATION_H

#include "propBaseObject.h"
#include "ui_propAnnotation.h"

class CAnnotation;
#include "RGBA.h"

class PropAnnotation : public PropWidget
{
	Q_OBJECT
		CAnnotation* obj;

public:

	explicit PropAnnotation(CAnnotation *a, QWidget *parent = 0);
	~PropAnnotation();

	static PropWidget* create(CAnnotation* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:
	//void changedColR(int);
	//void changedColG(int);
	//void changedColB(int);
	//void changedColA(int);

	//void changedSelR(int);
	//void changedSelG(int);
	//void changedSelB(int);
	//void changedSelA(int);

	void colorButtonPressed();
	void selcolorButtonPressed();

private:
	Ui::propAnnotation ui;

	void updateColorButton(CRGBA col);
	void updateSelColorButton(CRGBA col);

};

#endif // PROPANNOTATION_H
