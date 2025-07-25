#ifndef DOCKWIDGETMODEL_H
#define DOCKWIDGETMODEL_H																											MODELPROPERTIES

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetModel.h"
//#include "GLViewer.h"

#include "Wektor3D.h"

class DockWidgetModel : public QDockWidget
{
	Q_OBJECT

		CVector3<double> rot;
		CVector3<double> tra;
public:
	explicit DockWidgetModel(QWidget *parent = 0);
	~DockWidgetModel();

	void updateProperties();

	void showModelProperties( bool b ) { ui.groupBoxModel->setVisible( b ); }

public slots:
	void updateModelRotX( double );
	void updateModelRotY( double );
	void updateModelRotZ( double );
	void updateModelTrans( double );
	void updateModelScale( double );
	void updateModelCentered( int );
	void updateTranslations(double, double, double);

private:
	Ui::DockWidgetModel ui;
};

#endif // DOCKWIDGETMODEL_H
