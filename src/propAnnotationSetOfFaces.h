#ifndef PROPANNOTATIONVFACES_H
#define PROPANNOTATIONVFACES_H

#include "propAnnotation.h"

#include "ui_propAnnotationSetOfFaces.h"

class CAnnotationSetOfFaces;

class PropAnnotationSetOfFaces : PropWidget //: public PropAnnotation
{
	Q_OBJECT
	CAnnotationSetOfFaces* obj;

public:
	explicit PropAnnotationSetOfFaces(CAnnotationSetOfFaces *an, QWidget *parent = 0);
	~PropAnnotationSetOfFaces();

	static PropWidget* create(CAnnotationSetOfFaces* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual void updateProperties() override;

public slots:
	void pointSizeChanged(int);

private:
	Ui::propAnnotationSetOfFaces ui;
};

#endif // PROPANNOTATIONVFACES_H
