#ifndef PROPMODEL_H
#define PROPMODEL_H


#include "propTransform.h"

#include "ui_propModel.h"

#include "Wektor3D.h"
#include "Quaternion.h"

class CModel3D;

class PropModel : public PropWidget
{
	Q_OBJECT
		CModel3D* obj;

		CVector3<double> rot;
		CVector3<double> tra;
public:
	explicit PropModel(CModel3D *m, QWidget *parent = 0);
	~PropModel();

	static PropWidget* create(CModel3D* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* m);

	//virtual void paintEvent(QPaintEvent * event) override;

	virtual void updateProperties() override;

	//void showModelProperties( bool b ) { ui.groupBoxModel->setVisible( b ); }

public slots:
	void changedRotX( double );
	void changedRotY( double );
	void changedRotZ( double );
	void changedTraXYZ( double );
	void changedScale( double );
	void changedCentered( int );

	void changedQX(double x);
	void changedQY(double y);
	void changedQZ(double z);
	void changedQW(double w);

private:
	Ui::propModel ui;

	//void updateQuaternion(CQuaternion &q);

};

#endif // PROPMODEL_H
