#include "propModel.h"

#include "../core/Workspace.h"

#include "Model3D.h"
#include "QScrollArea"

PropModel::PropModel(CModel3D *m, QWidget *parent) : PropWidget( parent )
{
	obj = m;

	ui.setupUi((QWidget*)this);



}

PropModel::~PropModel()
{
}


//void PropModel::paintEvent(QPaintEvent * event)
//{
//	QWidget::paintEvent(event);
//}


void PropModel::updateProperties()
{
	return;
	//PropTransform::updateProperties();

	CModel3D *m = (CModel3D *)this->obj;

	if ( NULL != m )
	{
		CTransform &tr = m->getTransform();

		CVector3d e = tr.rotation().eulerAnglesDeg();

		ui.spinRotX->blockSignals(true);
		ui.spinRotX->setValue( rot.X( e.x ).x );
		ui.spinRotX->blockSignals(false);

		ui.spinRotY->blockSignals(true);
		ui.spinRotY->setValue( rot.Y( e.y ).y );
		ui.spinRotY->blockSignals(false);

		ui.spinRotZ->blockSignals(true);
		ui.spinRotZ->setValue( rot.Z( e.z ).z );
		ui.spinRotZ->blockSignals(false);

		ui.spinTransX->blockSignals(true);
		ui.spinTransX->setValue( tra.X( tr.translation().X() ).x );
		ui.spinTransX->blockSignals(false);

		ui.spinTransY->blockSignals(true);
		ui.spinTransY->setValue( tra.Y( tr.translation().Y() ).y );
		ui.spinTransY->blockSignals(false);

		ui.spinTransZ->blockSignals(true);
		ui.spinTransZ->setValue( tra.Z( tr.translation().Z() ).z );
		ui.spinTransZ->blockSignals(false);

		ui.spinScale->blockSignals(true);
		ui.spinScale->setValue( tr.scale().x ); //UWAGA !!!! @@@
		ui.spinScale->blockSignals(false);

		ui.checkCentered->blockSignals(true);
		ui.checkCentered->setChecked( tr.isTheOriginMovedToTheCenterOfRotation() );
		ui.checkCentered->blockSignals(false);

		//Kwaternion
		//updateQuaternion(tr.rotation());

		ui.originCoords->blockSignals(true);
		ui.originCoords->setItem(0, 0, new QTableWidgetItem(QString::number(tr.origin().X())));
		ui.originCoords->setItem(0, 1, new QTableWidgetItem(QString::number(tr.origin().Y())));
		ui.originCoords->setItem(0, 2, new QTableWidgetItem(QString::number(tr.origin().Z())));
		ui.originCoords->blockSignals(false);

	}
}

PropWidget* PropModel::create(CModel3D* m, QWidget* parent)
{
	return PropWidget::build(create_and_get_subwidgets(m), parent);
}

QVector<PropWidget*> PropModel::create_and_get_subwidgets(CBaseObject *obj)
{
	CModel3D* m = (CModel3D*)obj;
	return QVector<PropWidget*>({
			new PropBaseObject(m, nullptr), //, false),
			new PropTransform(&m->getTransform(), nullptr, false, m)
		});
}

namespace
{
	void notifyModelChanged(CModel3D* model)
	{
		if (model != nullptr)
		{
			CWorkspace::instance()->notifyObjectStateChanged(model->id());
		}
	}
}

void PropModel::changedRotX( double val )
{
	if (obj != nullptr)
	{
		double v = val;
		if (v > 180.000000)
		{
			v -= 360.0;
			ui.spinRotX->blockSignals(true);
			ui.spinRotX->setValue(v);
			ui.spinRotX->blockSignals(false);
		}
		else if (v < -179.999999)
		{
			v += 360.0;
			ui.spinRotX->blockSignals(true);
			ui.spinRotX->setValue(v);
			ui.spinRotX->blockSignals(false);
		}

		CQuaternion q(deg2rad(v - rot.X()), CVector3d::XAxis());
		
		obj->getTransform().rotation().multAndSet( q );

		rot.X(v);

		//updateQuaternion(obj->getTransform().rotation());

		notifyModelChanged(obj);
	}
}

void PropModel::changedRotY( double val )
{
	if (obj != nullptr)
	{
		double v = val;
		if (v > 180.000000)
		{
			v -= 360.0;
			ui.spinRotY->blockSignals(true);
			ui.spinRotY->setValue(v);
			ui.spinRotY->blockSignals(false);
		}
		else if (v < -179.999999)
		{
			v += 360.0;
			ui.spinRotY->blockSignals(true);
			ui.spinRotY->setValue(v);
			ui.spinRotY->blockSignals(false);
		}

		CQuaternion q(deg2rad(v - rot.Y()), CVector3d::YAxis());

		obj->getTransform().rotation().multAndSet(q);

		rot.Y(v);

		//updateQuaternion(obj->getTransform().rotation());

		notifyModelChanged(obj);
	}
}

void PropModel::changedRotZ( double val )
{
	if (obj != nullptr)
	{
		double v = val;
		if (v > 180.000000)
		{
			v -= 360.0;
			ui.spinRotZ->blockSignals(true);
			ui.spinRotZ->setValue(v);
			ui.spinRotZ->blockSignals(false);
		}
		else if (v < -179.999999)
		{
			v += 360.0;
			ui.spinRotZ->blockSignals(true);
			ui.spinRotZ->setValue(v);
			ui.spinRotZ->blockSignals(false);
		}

		CQuaternion q(deg2rad(v - rot.Z()), CVector3d::ZAxis());

		obj->getTransform().rotation().multAndSet(q);

		rot.Z(v);

		//updateQuaternion(obj->getTransform().rotation());

		notifyModelChanged(obj);
	}
}


void PropModel::changedTraXYZ( double val )
{
	if (obj != nullptr)
	{
		obj->getTransform().translation().Set(ui.spinTransX->value(), ui.spinTransY->value(), ui.spinTransZ->value());

		tra.Set( ui.spinTransX->value(), ui.spinTransY->value(), ui.spinTransZ->value() );

		notifyModelChanged(obj);
	}
}

void PropModel::changedScale( double val )
{
	if (obj != nullptr)
	{
		obj->getTransform().setScale( val );

		notifyModelChanged(obj);
	}
}

void PropModel::changedCentered( int val )
{
	if (obj != nullptr)
	{
		obj->getTransform().moveTheOriginToTheCenterOfRotation( val == Qt::Checked );

		notifyModelChanged(obj);
	}
}

void PropModel::changedQX(double x)
{
	if (obj != nullptr)
	{
		obj->getTransform().rotation().setX(x);
		rot = obj->getTransform().rotation().eulerAnglesDeg();
		notifyModelChanged(obj);
	}
}

void PropModel::changedQY(double y)
{
	if (obj != nullptr)
	{
		obj->getTransform().rotation().setY(y);
		rot = obj->getTransform().rotation().eulerAnglesDeg();
		notifyModelChanged(obj);
	}
}

void PropModel::changedQZ(double z)
{
	if (obj != nullptr)
	{
		obj->getTransform().rotation().setZ(z);
		rot = obj->getTransform().rotation().eulerAnglesDeg();
		notifyModelChanged(obj);
	}
}

void PropModel::changedQW(double w)
{
	if (obj != nullptr)
	{
		obj->getTransform().rotation().setW(w);
		rot = obj->getTransform().rotation().eulerAnglesDeg();
		notifyModelChanged(obj);
	}
}

