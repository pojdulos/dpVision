#ifndef PROPHISTOGRAM_H
#define PROPHISTOGRAM_H

#include "propAnnotation.h"

#include "ui_propHistogram.h"

class CHistogram;

//class PropHistogram : public PropBaseObject //: public PropAnnotation
class PropHistogram : public PropWidget
{
	Q_OBJECT
		CHistogram* obj;
public:
	explicit PropHistogram(CHistogram *an, QWidget *parent = 0);
	~PropHistogram();

	static PropWidget* create(CHistogram* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:
	void changedLevels(int);
	void changedABS(int x);
	void changedLowerLimit(double);
	void changedUpperLimit(double);
	void colorSchemeChanged(int);

private:
	Ui::propHistogram ui;

	void showAdditionalInfo();
};

#endif // PROPHISTOGRAM_H
