#ifndef PROPANNOTATIONSETOFVERTICES_H
#define PROPANNOTATIONSETOFVERTICES_H

#include "propAnnotation.h"

#include "ui_propAnnotationSetOfVertices.h"

class CAnnotationSetOfVertices;

class PropAnnotationSetOfVertices : PropWidget //: public PropAnnotation
{
	Q_OBJECT
	CAnnotationSetOfVertices* obj;

public:
	explicit PropAnnotationSetOfVertices(CAnnotationSetOfVertices *an, QWidget *parent = 0);
	~PropAnnotationSetOfVertices();

	static PropWidget* create(CAnnotationSetOfVertices* m, QWidget* parent = 0);
		
	virtual void updateProperties() override;


public slots:
	void pointSizeChanged(int);

private:
	Ui::propAnnotationSetOfVertices ui;
};

#endif // PROPANNOTATIONSETOFVERTICES_H
