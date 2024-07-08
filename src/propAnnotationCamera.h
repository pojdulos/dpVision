#ifndef PROPANNOTATIONCAMERA_H
#define PROPANNOTATIONCAMERA_H

#include "propAnnotation.h"

#include "ui_propAnnotationCamera.h"

class CAnnotationCamera;

class PropAnnotationCamera : public PropWidget
{
	Q_OBJECT
		CAnnotationCamera* obj;

public:
	explicit PropAnnotationCamera(CAnnotationCamera*an, QWidget *parent = 0);
	~PropAnnotationCamera();

	static PropWidget* create(CAnnotationCamera* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);


	virtual void updateProperties() override;

public slots:
	void clearMatrix();
	void copyToClipboard();
	void pasteFromClipboard();

	void updateSensorProperties();


private:
	Ui::propAnnotationCamera ui;
};

#endif // PROPANNOTATIONCAMERA_H
