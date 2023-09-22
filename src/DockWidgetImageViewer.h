#ifndef DOCKWIDGETIMAGEVIEWER_H
#define DOCKWIDGETIMAGEVIEWER_H

#include <QtWidgets/QDockWidget>
#include <QtGui/QImage>

#include "ui_dockWidgetImageViewer.h"

#include "GLViewer.h"

#include "Wektor3D.h"

class DockWidgetImageViewer : public QDockWidget
{
	Q_OBJECT

	//void paintEvent(QPaintEvent*) override;
	void resizeEvent(QResizeEvent*) override;

	QPixmap m_image;
	//CVector3<double> rot;
	//CVector3<double> tra;
public:
	explicit DockWidgetImageViewer(QWidget *parent = 0);
	~DockWidgetImageViewer();

	void updateProperties();
	void setImage(QString fname);
	void drawImage();


public slots:
	//void updateRotX( double );
	//void updateRotY( double );
	//void updateRotZ( double );
	//void updateTrans( double );
	//void updateScale( double );
	//void updateBGcolor( double );
	//void updateAngle( int );

private:
	Ui::DockWidgetImageViewer ui;
};

#endif // DOCKWIDGETIMAGEVIEWER_H
