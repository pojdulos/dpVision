#include "DockWidgetLights.h"

#include "../core/AppStateManager.h"
#include "Workspace.h"

namespace
{
	CWorkspace* workspace()
	{
		return CWorkspace::instance();
	}

	_light& lightRef(int source)
	{
		return workspace()->getLightRef(source);
	}
}

DockWidgetLights::DockWidgetLights(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	selectSource( 0 );
}

DockWidgetLights::~DockWidgetLights()
{
}

void DockWidgetLights::selectSource( int source )
{
	_light &src = lightRef(source);

	setValueOf( ui.ambientR, src.ambient[0] );
	setValueOf( ui.ambientG, src.ambient[1] );
	setValueOf( ui.ambientB, src.ambient[2] );
	setValueOf( ui.ambientA, src.ambient[3] );

	setValueOf( ui.difuseR, src.diffuse[0] );
	setValueOf( ui.difuseG, src.diffuse[1] );
	setValueOf( ui.difuseB, src.diffuse[2] );
	setValueOf( ui.difuseA, src.diffuse[3] );

	setValueOf( ui.specularR, src.specular[0] );
	setValueOf( ui.specularG, src.specular[1] );
	setValueOf( ui.specularB, src.specular[2] );
	setValueOf( ui.specularA, src.specular[3] );

	setValueOf( ui.positionX, src.position[0] );
	setValueOf( ui.positionY, src.position[1] );
	setValueOf( ui.positionZ, src.position[2] );

	setValueOf( ui.spotX, src.spot_direction[0] );
	setValueOf( ui.spotY, src.spot_direction[1] );
	setValueOf( ui.spotZ, src.spot_direction[2] );

	setValueOf( ui.spotAngle, src.spot_cut_off );

	setChecked( ui.checkActive, src.active );
	setChecked( ui.checkFixed, src.fixed );
}

void DockWidgetLights::setActive( int val )
{
	lightRef(ui.comboLightSource->currentIndex()).active = ( val != 0 );
	AppStateManager::updateAllViews();
}

void DockWidgetLights::setFixed( int val )
{
	lightRef(ui.comboLightSource->currentIndex()).fixed = ( val != 0 );
	AppStateManager::updateAllViews();
}

void DockWidgetLights::ambientR( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).ambient[0] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::ambientG( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).ambient[1] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::ambientB( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).ambient[2] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::ambientA( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).ambient[3] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::difuseR( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).diffuse[0] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::difuseG( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).diffuse[1] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::difuseB( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).diffuse[2] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::difuseA( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).diffuse[3] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::specularR( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).specular[0] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::specularG( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).specular[1] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::specularB( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).specular[2] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::specularA( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).specular[3] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::positionX( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).position[0] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::positionY( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).position[1] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::positionZ( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).position[2] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::spotDirX( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).spot_direction[0] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::spotDirY( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).spot_direction[1] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::spotDirZ( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).spot_direction[2] = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::spotAngle( double val )
{
	lightRef(ui.comboLightSource->currentIndex()).spot_cut_off = val;
	AppStateManager::updateAllViews();
}

void DockWidgetLights::setValueOf(QDoubleSpinBox * spin, double value, bool callback)
{
	if (callback)
	{
		spin->setValue(value);
	}
	else
	{
		spin->blockSignals(true);
		spin->setValue(value);
		spin->blockSignals(false);
	}
}

void DockWidgetLights::setChecked(QCheckBox * box, bool b, bool callback)
{
	if (callback)
	{
		box->setChecked(b);
	}
	else
	{
		box->blockSignals(true);
		box->setChecked(b);
		box->blockSignals(false);
	}
}
