#include "DockWidgetImageViewer.h"

#include "UI.h"
#include "AP.h"

#include "Quaternion.h"
#include "MainWindow.h"

#include "GLViewer.h"


DockWidgetImageViewer::DockWidgetImageViewer(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	//QObject::connect( ui.spinViewRotX, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotX(double)) );
	//QObject::connect( ui.spinViewRotY, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotY(double)) );
	//QObject::connect( ui.spinViewRotZ, SIGNAL(valueChanged(double)), this, SLOT(updateViewerRotZ(double)) );

	//QObject::connect( ui.spinViewTransX, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );
	//QObject::connect( ui.spinViewTransY, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );
	//QObject::connect( ui.spinViewTransZ, SIGNAL(valueChanged(double)), this, SLOT(updateViewerTrans(double)) );

	//QObject::connect( ui.spinViewScale, SIGNAL(valueChanged(double)), this, SLOT(updateViewerScale(double)) );
	//QObject::connect( ui.spinBG, SIGNAL(valueChanged(double)), this, SLOT(updateViewerBGcolor(double)) );
	//QObject::connect( ui.spinAngleOfView, SIGNAL(valueChanged(int)), this, SLOT(updateViewerAngle(int)) );
}

DockWidgetImageViewer::~DockWidgetImageViewer()
{
}


void DockWidgetImageViewer::drawImage()
{
	QRect rect = this->rect();  //ui.scrollArea->rect();
	double asp = (double)m_image.width() / m_image.height();
	double aspW = (double)rect.width() / rect.height();

	if (asp >= aspW)
	{
		ui.label->setPixmap(m_image.scaledToWidth(rect.width()));
	}
	else
	{
		ui.label->setPixmap(m_image.scaledToHeight(rect.height()));
	}
}

void DockWidgetImageViewer::resizeEvent(QResizeEvent* event)
{
	QDockWidget::resizeEvent(event);
	
	if (event->oldSize().width() > 0)
	{
		drawImage();
	}
}


void DockWidgetImageViewer::updateProperties()
{
}

void DockWidgetImageViewer::setImage(QString fname)
{
	m_image = QPixmap(fname);
}


