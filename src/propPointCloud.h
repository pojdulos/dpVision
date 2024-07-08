#ifndef PROPPOINTCLOUD_H
#define PROPPOINTCLOUD_H

#include "propBaseObject.h"
#include "propMaterial.h"
#include "ui_propPointCloud.h"

class CPointCloud;
class QColorDialog;

class PropPointCloud : public PropWidget
{
	Q_OBJECT

	CPointCloud* obj;

	static bool group_visible;

public:
	explicit PropPointCloud(CPointCloud *mesh, QWidget *parent = 0);
	~PropPointCloud();

	static PropWidget* create(CPointCloud* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	virtual void updateProperties() override;

public slots:
	void adjustGroupHeight(bool);

	//void testButtonClicked();

	void pointSizeChanged(int);

	//void rValueChanged(int);
	//void gValueChanged(int);
	//void bValueChanged(int);
	//void aValueChanged(int);
	//
	//void rgbaTextChanged(QString);

	//void meshColorChanged(QColor);
	//void colorButtonClicked();

private:
	Ui::propPointCloud ui;

	//QColorDialog *cd;

	//void updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	//void updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

#endif // PROPPOINTCLOUD_H
