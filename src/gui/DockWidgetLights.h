#ifndef DOCKWIDGETLIGHTS_H
#define DOCKWIDGETLIGHTS_H

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetLights.h"

class DockWidgetLights : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetLights(QWidget *parent = 0);
	~DockWidgetLights();

public slots:
	void selectSource( int );

	void setActive( int );
	void setFixed( int );

	void ambientR( double );
	void ambientG( double );
	void ambientB( double );
	void ambientA( double );

	void difuseR( double );
	void difuseG( double );
	void difuseB( double );
	void difuseA( double );

	void specularR( double );
	void specularG( double );
	void specularB( double );
	void specularA( double );

	void positionX( double );
	void positionY( double );
	void positionZ( double );

	void spotDirX( double );
	void spotDirY( double );
	void spotDirZ( double );

	void spotAngle( double );
private:
	Ui::DockWidgetLights ui;

	void setValueOf(QDoubleSpinBox* spin, double value, bool callback = false);
	void setChecked(QCheckBox* box, bool b, bool callback = false);
};

#endif // DOCKWIDGETLIGHTS_H
