#include "DockWidgetViewer.h"

#include "UI.h"
#include "AP.h"

#include "Quaternion.h"
#include "MainWindow.h"

#include "GLViewer.h"

DockWidgetViewer::DockWidgetViewer(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	QObject::connect( ui.spinViewRotX, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotX(double)) );
	QObject::connect( ui.spinViewRotY, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotY(double)) );
	QObject::connect( ui.spinViewRotZ, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotZ(double)) );

	QObject::connect( ui.spinViewTransX, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );
	QObject::connect( ui.spinViewTransY, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );
	QObject::connect( ui.spinViewTransZ, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );

	QObject::connect( ui.spinViewScale, SIGNAL(valueChanged(double)), this, SLOT(updateViewerScale(double)) );
	QObject::connect( ui.spinBG, SIGNAL(valueChanged(double)), this, SLOT(updateViewerBGcolor(double)) );
	QObject::connect( ui.spinAngleOfView, SIGNAL(valueChanged(int)), this, SLOT(updateViewerAngle(int)) );
}

DockWidgetViewer::~DockWidgetViewer()
{
}

void DockWidgetViewer::updateProperties()
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{

		CVector3d e = view->transform().rotation().eulerAnglesDeg();

		ui.spinViewRotX->blockSignals(true);
		ui.spinViewRotX->setValue( rot.X( e.x ) );
		ui.spinViewRotX->blockSignals(false);

		ui.spinViewRotY->blockSignals(true);
		ui.spinViewRotY->setValue( rot.Y( e.y ) );
		ui.spinViewRotY->blockSignals(false);

		ui.spinViewRotZ->blockSignals(true);
		ui.spinViewRotZ->setValue( rot.Z( e.z ) );
		ui.spinViewRotZ->blockSignals(false);

		ui.spinViewTransX->blockSignals(true);
		ui.spinViewTransX->setValue( tra.X(view->transform().translation().X() ) );
		ui.spinViewTransX->blockSignals(false);

		ui.spinViewTransY->blockSignals(true);
		ui.spinViewTransY->setValue( tra.Y(view->transform().translation().Y() ) );
		ui.spinViewTransY->blockSignals(false);

		ui.spinViewTransZ->blockSignals(true);
		ui.spinViewTransZ->setValue( tra.Z(view->transform().translation().Z() ) );
		ui.spinViewTransZ->blockSignals(false);

		ui.spinViewScale->blockSignals(true);
		ui.spinViewScale->setValue(view->transform().scale().x );
		ui.spinViewScale->blockSignals(false);
	}
}


void DockWidgetViewer::updateRotX( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		double v = val;
		if (v > 180.0) v -= 360.0;
		else if (v <= -180.0) v += 360.0;

		CQuaternion q(deg2rad(v - rot.X()), CVector3d::XAxis());

		view->transform().rotation().multAndSet(q);

		rot.X(v);

		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateRotY( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		double v = val;
		if (v > 180.0) v -= 360.0;
		else if (v <= -180.0) v += 360.0;

		CQuaternion q(deg2rad(v - rot.Y()), CVector3d::YAxis());

		view->transform().rotation().multAndSet(q);

		rot.Y(v);

		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateRotZ( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if ( NULL != view )
	{
		double v = val;
		if (v > 180.0) v -= 360.0;
		else if (v <= -180.0) v += 360.0;

		CQuaternion q(deg2rad(v - rot.Z()), CVector3d::ZAxis());

		view->transform().rotation().multAndSet(q);

		rot.Z(v);

		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateTrans( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->transform().translation() = CVector3d( ui.spinViewTransX->value(), ui.spinViewTransY->value(), ui.spinViewTransZ->value() );
       
		tra.Set(ui.spinViewTransX->value(), ui.spinViewTransY->value(), ui.spinViewTransZ->value());

		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateScale( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->transform().setScale( ui.spinViewScale->value() );
       
		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateBGcolor( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->setBGcolor( ui.spinBG->value() );
       
		UI::updateCurrentView();
	}
}

void DockWidgetViewer::updateAngle( int val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->setVAngle( ui.spinAngleOfView->value() );
       
		UI::updateCurrentView();
	}
}
