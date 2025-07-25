#ifndef DOCKWIDGETVIEWER_H
#define DOCKWIDGETVIEWER_H

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetViewer.h"
#include "GLViewer.h"

#include "Wektor3D.h"

class DockWidgetViewer : public QDockWidget
{
	Q_OBJECT

	CVector3<double> rot;
	CVector3<double> tra;
public:
	explicit DockWidgetViewer(QWidget *parent = 0);
	~DockWidgetViewer();

	void updateProperties();

public slots:
	void updateRotX( double );
	void updateRotY( double );
	void updateRotZ( double );
	void updateTrans( double );
	void updateScale( double );
	void updateBGcolor( double );
	void updateAngle( int );

private:
	Ui::DockWidgetViewer ui;
};

#endif // DOCKWIDGETVIEWER_H
