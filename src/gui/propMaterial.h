#ifndef PROPMATERIAL_H
#define PROPMATERIAL_H

#include "propBaseObject.h"
#include "ui_propMaterial.h"

class CMesh;
class QColorDialog;

class PropMaterial : public PropWidget
{
	Q_OBJECT

	CBaseObject* obj;

	static bool group_visible;
public:
	explicit PropMaterial(CBaseObject *mesh, QWidget *parent = 0);
	~PropMaterial();

	static PropWidget* create(CBaseObject* m, QWidget* parent = 0);


	virtual void updateProperties() override;

public slots:
	//void testButtonClicked();
	
	void rValueChanged(int);
	void gValueChanged(int);
	void bValueChanged(int);
	void aValueChanged(int);

	void rgbaTextChanged(QString);

	void meshColorChanged(QColor);
	void colorButtonClicked();

	void onColorButton();
	
	void checkShowClicked(int);

private:
	Ui::propMaterial ui;

	void updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

};

#endif // PROPMATERIAL_H
