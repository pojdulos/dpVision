#ifndef PROPMESH_H
#define PROPMESH_H

#include "propPointCloud.h"
#include "ui_propMesh.h"

class CMesh;
class QColorDialog;

class PropMesh : public PropWidget
{
	Q_OBJECT
		CMesh* obj;

public:
	explicit PropMesh(CMesh *mesh, QWidget *parent = 0);
	~PropMesh();

	static PropWidget* create(CMesh* m, QWidget* parent = 0);

	virtual void updateProperties() override;

public slots:
	//void testButtonClicked();
	
	//void rValueChanged(int);
	//void gValueChanged(int);
	//void bValueChanged(int);
	//void aValueChanged(int);
	//
	//void rgbaTextChanged(QString);

	//void meshColorChanged(QColor);
	//void colorButtonClicked();

private:
	Ui::propMesh ui;

	//QColorDialog *cd;

	//void updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	//void updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

#endif // PROPMESH_H
