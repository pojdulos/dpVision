#include "propViewer.h"

#include "UI.h"
#include "AP.h"
#include "QPaintEvent"

#include "RGBA.h"

#include "MainWindow.h"
#include "GLViewer.h"

PropViewer::PropViewer(GLViewer* m, QWidget *parent)	: PropWidget( parent )
{
	treeItemLabel = "Camera properties";

	ui.setupUi((QWidget*)this);
}

PropViewer::~PropViewer()
{
}

PropWidget* PropViewer::create(GLViewer* m, QWidget* parent)
{
	return PropWidget::build( PropViewer::create_and_get_subwidgets(m), parent );
}

QVector<PropWidget*> PropViewer::create_and_get_subwidgets(GLViewer* m)
{
	return QVector<PropWidget*>({ new PropViewer(m, nullptr) });
}


void PropViewer::paintEvent(QPaintEvent * event)
{
	QWidget::paintEvent(event);
}

void PropViewer::updateProperties()
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
		ui.spinViewScale->setValue(view->transform().scale().x ); //UWAGA !!! @@@
		ui.spinViewScale->blockSignals(false);

		ui.spinBG->blockSignals(true);
		ui.spinBG->setValue(view->getBGcolor().fR());
		ui.spinBG->blockSignals(false);

		bool isOrtho = view->currentProjectionIs(GLViewer::Projection::ORTHOGONAL);

		ui.radioOrtho->blockSignals(true);
		ui.radioOrtho->setChecked(isOrtho);
		ui.radioOrtho->blockSignals(false);

		ui.spinOrthoViewSize->blockSignals(true);
		ui.spinOrthoViewSize->setValue(view->getOrthoSize());
		ui.spinOrthoViewSize->blockSignals(false);

		ui.spinOrthoViewSize->setEnabled(isOrtho);

		bool isPersp = !isOrtho;

		ui.radioPersp->blockSignals(true);
		ui.radioPersp->setChecked(isPersp);
		ui.radioPersp->blockSignals(false);

		ui.spinAngleOfView->blockSignals(true);
		ui.spinAngleOfView->setValue(view->getVAngle());
		ui.spinAngleOfView->blockSignals(false);

		ui.spinAngleOfView->setEnabled(isPersp);
	}
}

void PropViewer::rotXeditingFinished()
{
	GLViewer* view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		double x = ui.spinViewRotX->value();
		double y = ui.spinViewRotY->value();
		double z = ui.spinViewRotZ->value();

		view->transform().rotation().fromEulerAnglesDeg(CVector3d(x, y, z));

		rot = view->transform().rotation().eulerAnglesDeg();

		UI::updateCurrentView();
	}
}
void PropViewer::changedRotX( double val )
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

void PropViewer::changedRotY( double val )
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

void PropViewer::changedRotZ( double val )
{
	GLViewer* view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		//double v = val;
		double delta = val - rot.Z();

		while (delta > 180.0) delta -= 360.0;

		while (delta <= -180.0) delta += 360.0;

		CQuaternion q(deg2rad(delta), CVector3d::ZAxis());

		view->transform().rotation().multAndSet(q);

		CVector3f e = view->transform().rotation().eulerAnglesDeg();

		ui.spinViewRotZ->blockSignals(true);
		ui.spinViewRotZ->setValue(rot.Z( e.z ));
		ui.spinViewRotZ->blockSignals(false);

		UI::updateCurrentView();
	}

	//GLViewer *view = AP::mainWin().currentViewer();
	//if ( NULL != view )
	//{
	//	double v = val;
	//	if (v > 180.0) v -= 360.0;
	//	else if (v <= -180.0) v += 360.0;

	//	CQuaternion q;
	//	q.FromRotationAxis(deg2rad(v - rot.Z()), 0.0, 0.0, 1.0);

	//	view->transform().rotation().MultAndSet(q);

	//	rot.Z(v);

	//	UI::updateCurrentView();
	//}
}

void PropViewer::changedTraXYZ( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->transform().translation() = CVector3d( ui.spinViewTransX->value(), ui.spinViewTransY->value(), ui.spinViewTransZ->value() );
       
		tra.Set(ui.spinViewTransX->value(), ui.spinViewTransY->value(), ui.spinViewTransZ->value());

		UI::updateCurrentView();
	}
}

void PropViewer::changedScale( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->transform().setScale( ui.spinViewScale->value() );
       
		UI::updateCurrentView();
	}
}

void PropViewer::changedBGcolor( double val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->setBGcolor( ui.spinBG->value() );
       
		UI::updateCurrentView();
	}
}

void PropViewer::changedAngle( int val )
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->setVAngle( ui.spinAngleOfView->value() );

		UI::updateCurrentView();
	}
}

void PropViewer::changedOrthoViewSize(double)
{
	GLViewer* view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		view->setOrthoSize(ui.spinOrthoViewSize->value());

		UI::updateCurrentView();
	}
}

void PropViewer::radioPropToggled(bool t)
{
	(t) ? AP::mainWin().projectionPerspective() : AP::mainWin().projectionOrthogonal();
}
