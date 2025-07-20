#include "DockWidgetLights.h"

#include "../api/UI.h"
#include "../api/AP.h"

#include "Workspace.h"

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
	_light &src = AP::getWorkspace()->getLightRef(source);

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
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).active = ( val != 0 );
	UI::updateAllViews();
}

void DockWidgetLights::setFixed( int val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).fixed = ( val != 0 );
	UI::updateAllViews();
}

void DockWidgetLights::ambientR( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).ambient[0] = val;
	UI::updateAllViews();
}

void DockWidgetLights::ambientG( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).ambient[1] = val;
	UI::updateAllViews();
}

void DockWidgetLights::ambientB( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).ambient[2] = val;
	UI::updateAllViews();
}

void DockWidgetLights::ambientA( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).ambient[3] = val;
	UI::updateAllViews();
}

void DockWidgetLights::difuseR( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).diffuse[0] = val;
	UI::updateAllViews();
}

void DockWidgetLights::difuseG( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).diffuse[1] = val;
	UI::updateAllViews();
}

void DockWidgetLights::difuseB( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).diffuse[2] = val;
	UI::updateAllViews();
}

void DockWidgetLights::difuseA( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).diffuse[3] = val;
	UI::updateAllViews();
}

void DockWidgetLights::specularR( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).specular[0] = val;
	UI::updateAllViews();
}

void DockWidgetLights::specularG( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).specular[1] = val;
	UI::updateAllViews();
}

void DockWidgetLights::specularB( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).specular[2] = val;
	UI::updateAllViews();
}

void DockWidgetLights::specularA( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).specular[3] = val;
	UI::updateAllViews();
}

void DockWidgetLights::positionX( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).position[0] = val;
	UI::updateAllViews();
}

void DockWidgetLights::positionY( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).position[1] = val;
	UI::updateAllViews();
}

void DockWidgetLights::positionZ( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).position[2] = val;
	UI::updateAllViews();
}

void DockWidgetLights::spotDirX( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).spot_direction[0] = val;
	UI::updateAllViews();
}

void DockWidgetLights::spotDirY( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).spot_direction[1] = val;
	UI::updateAllViews();
}

void DockWidgetLights::spotDirZ( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).spot_direction[2] = val;
	UI::updateAllViews();
}

void DockWidgetLights::spotAngle( double val )
{
	AP::getWorkspace()->getLightRef( ui.comboLightSource->currentIndex() ).spot_cut_off = val;
	UI::updateAllViews();
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
