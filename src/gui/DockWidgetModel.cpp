#include "DockWidgetModel.h"

#include "UI.h"
#include "AP.h"

#include "Transform.h"
#include "Model3D.h"

#include "MainWindow.h"
#include "GLViewer.h"

DockWidgetModel::DockWidgetModel(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	QObject::connect( ui.spinRotX, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotX(double)) );
	QObject::connect( ui.spinRotY, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotY(double)) );
	QObject::connect( ui.spinRotZ, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotZ(double)) );

	QObject::connect( ui.spinTransX, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );
	QObject::connect( ui.spinTransY, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );
	QObject::connect( ui.spinTransZ, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );

	QObject::connect( ui.spinScale, SIGNAL(valueChanged(double)), this, SLOT(updateModelScale(double)) );
	QObject::connect( ui.checkCentered, SIGNAL(stateChanged(int)), this, SLOT(updateModelCentered(int)) );

	//ChildWindow *child = (ChildWindow*)AP::mainWin().ui.mdiArea->activeSubWindow();
	//if (NULL != child)
	//{
	//	GLViewer *v = child->getViewer();
	//	QObject::connect( v, SLOT(translationChanged(double, double, double)), this, SLOT(updateTranslations(double, double, double)), Qt::DirectConnection );
	//}


}

DockWidgetModel::~DockWidgetModel()
{
}


void DockWidgetModel::updateProperties()
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		CTransform tr = obj->getTransform();

		CVector3d e = tr.rotation().eulerAnglesDeg();

		ui.spinRotX->blockSignals(true);
		ui.spinRotX->setValue( rot.X( e.x ) );
		ui.spinRotX->blockSignals(false);

		ui.spinRotY->blockSignals(true);
		ui.spinRotY->setValue( rot.Y( e.y ) );
		ui.spinRotY->blockSignals(false);

		ui.spinRotZ->blockSignals(true);
		ui.spinRotZ->setValue( rot.Z( e.z ) );
		ui.spinRotZ->blockSignals(false);

		ui.spinTransX->blockSignals(true);
		ui.spinTransX->setValue( tra.X( tr.translation().X() ) );
		ui.spinTransX->blockSignals(false);

		ui.spinTransY->blockSignals(true);
		ui.spinTransY->setValue( tra.Y( tr.translation().Y() ) );
		ui.spinTransY->blockSignals(false);

		ui.spinTransZ->blockSignals(true);
		ui.spinTransZ->setValue( tra.Z( tr.translation().Z() ) );
		ui.spinTransZ->blockSignals(false);

		ui.spinScale->blockSignals(true);
		ui.spinScale->setValue( tr.scale().x );
		ui.spinScale->blockSignals(false);

		ui.checkCentered->blockSignals(true);
		ui.checkCentered->setChecked( tr.isTheOriginMovedToTheCenterOfRotation() );
		ui.checkCentered->blockSignals(false);
	}
}

void DockWidgetModel::updateTranslations(double x, double y, double z)
{
	GLViewer *view = AP::mainWin().currentViewer();
	if (NULL != view)
	{
		;
	}

	UI::STATUSBAR::printfTimed(500, "!! updateTranslations !!");
}


void DockWidgetModel::updateModelRotX( double val )
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if ( NULL != obj )
	{
		double v = val;
		if ( v > 180.0 ) v -= 360.0;
		else if ( v <= -180.0 ) v += 360.0;

		//CQuaternion q = CQuaternion::fromAxisAndAngle(CVector3d::XAxis(), deg2rad(v - rot.X()));
		CQuaternion q(deg2rad(v - rot.X()), CVector3d::XAxis());

		obj->getTransform().rotation().multAndSet( q );

		rot.X(v);

		UI::updateAllViews();
	}
}

void DockWidgetModel::updateModelRotY( double val )
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if (NULL != obj)
	{
		double v = val;
		if (v > 180.0) v -= 360.0;
		else if (v <= -180.0) v += 360.0;

		CQuaternion q(deg2rad(v - rot.Y()), CVector3d::YAxis());

		obj->getTransform().rotation().multAndSet(q);

		rot.Y(v);

		UI::updateAllViews();
	}
}

void DockWidgetModel::updateModelRotZ( double val )
{
	std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

	if (NULL != obj)
	{
		double v = val;
		if (v > 180.0) v -= 360.0;
		else if (v <= -180.0) v += 360.0;

		CQuaternion q(deg2rad(v - rot.Z()), CVector3d::ZAxis());
		obj->getTransform().rotation().multAndSet(q);

		rot.Z(v);

		UI::updateAllViews();
	}
}


void DockWidgetModel::updateModelTrans( double val )
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->getTransform().translation().Set( ui.spinTransX->value(), ui.spinTransY->value(), ui.spinTransZ->value() );

		tra.Set( ui.spinTransX->value(), ui.spinTransY->value(), ui.spinTransZ->value() );

		UI::updateAllViews();
	}
}

void DockWidgetModel::updateModelScale( double val )
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->getTransform().setScale( ui.spinScale->value() );

		UI::updateAllViews();
	}
}

void DockWidgetModel::updateModelCentered( int val )
{
	if ( NULL != AP::WORKSPACE::getCurrentModel() )
	{
		AP::WORKSPACE::getCurrentModel()->getTransform().moveTheOriginToTheCenterOfRotation( ui.checkCentered->isChecked() );

		UI::updateAllViews();
	}
}

