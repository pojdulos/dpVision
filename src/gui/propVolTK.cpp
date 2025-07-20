#include "propVolTK.h"
#include "Mesh.h"
#include "../api/UI.h"

#include <QColorDialog>
#include <QPushButton>
#include "MainWindow.h"

#include "VolTK.h"
#include "volTKcolorWidget.h"

#include "RangeSlider.h"

PropVolTK::PropVolTK(CVolTK *voltk, QWidget *parent) : PropWidget( parent )
{
	obj = voltk;
	ui.setupUi((QWidget*)this);

	QVBoxLayout* layout = new QVBoxLayout(ui.scrollAreaWidgetContents);
	layout->setAlignment(Qt::AlignTop);
	layout->setContentsMargins(QMargins(0, 0, 0, 0));
	ui.scrollAreaWidgetContents->setLayout(layout);

	//RangeSlider* rs = new RangeSlider( Qt::Orientation::Horizontal, RangeSlider::DoubleHandles, ui.volTK );
	//rs->setObjectName(QString::fromUtf8("rsX"));
	//rs->setGeometry(QRect(10, 480, 160, 22));
	//rs->SetMinimum(obj->m_xMin);
	//rs->SetMaximum(obj->m_xMax+1);
	////connect(rs, SIGNAL(rangeChanged(int,int)), this, SLOT(xRangeChanged(int,int)));
	//connect(rs, SIGNAL(lowerValueChanged(int)), this, SLOT(xBchanged(int)));
	//connect(rs, SIGNAL(upperValueChanged(int)), this, SLOT(xEchanged(int)));

	int xwidth = 300;
	MyScrollBar* horizontalScrollBar = new MyScrollBar(ui.volTK);
	horizontalScrollBar->setObjectName(QString::fromUtf8("horizontalScrollBar"));
	horizontalScrollBar->setGeometry(QRect(10, 510, 160, 16));
	horizontalScrollBar->setOrientation(Qt::Horizontal);
	horizontalScrollBar->setRange(obj->m_Min.x, obj->m_Max.x - xwidth + 1);
	horizontalScrollBar->setSingleStep(1);
	//horizontalScrollBar->setPageStep(xwidth);
	connect(horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(xValchanged(int)));

	update();
	//ui.volTK->layout()->addWidget(rs);
}

PropVolTK::~PropVolTK()
{
}

void PropVolTK::addColorWidget(int winMax, QColor color, bool active, bool update)
{
	//QWidget* widget = new QWidget();
	//QHBoxLayout* layout = new QHBoxLayout(widget);
	//layout->setContentsMargins(QMargins(0, 0, 0, 0));

	VolTKcolorWidget* cwidget = new VolTKcolorWidget(this);
	cwidget->setWinMax(winMax);
	cwidget->setColor(color);
	cwidget->setActive(active);
	cwidget->setVisible(true);

	//layout->addWidget(cwidget);

	//QPushButton* remButton = new QPushButton("x");
	//layout->addWidget(remButton);

	//widget->resize(layout->sizeHint());
	//widget->setMinimumSize(layout->sizeHint());
	//widget->setMaximumSize(layout->sizeHint());

	//widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	ui.scrollAreaWidgetContents->layout()->addWidget(cwidget);

	if (update)
		updateColorFilters();
}

PropWidget* PropVolTK::create(CVolTK* vol, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(vol, widget));
	layout->addWidget(new PropVolTK(vol, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}


void PropVolTK::updateInfo()
{
	ui.info->blockSignals(true);
	ui.info->setText(QString::fromStdWString(obj->infoRow()));
	ui.info->blockSignals(false);
}

void PropVolTK::updateFilters()
{
	QWidgetList widgets = ui.scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

	QWidgetList::iterator w = widgets.begin();
	for (CVolTK::ColorFilter cf : obj->colorFilters)
	{
		if (w != widgets.end())
		{
			((VolTKcolorWidget*)*w)->setWinMax(cf.first);
			((VolTKcolorWidget*)*w)->setActive(cf.second.first);
			((VolTKcolorWidget*)*w)->setColor(QColor::fromRgba(cf.second.second.toArgb32()));
			w++;
		}
		else
		{
			addColorWidget(cf.first, QColor::fromRgba(cf.second.second.toArgb32()), cf.second.first, false);
		}
	}

	while (w != widgets.end())
	{
		VolTKcolorWidget* tmp = (VolTKcolorWidget*)*w;
		w++;

		delete tmp;
	}
}

void PropVolTK::updateProperties()
{
	ui.spinWinMin->blockSignals(true);
	ui.spinWinMin->setMinimum(obj->m_dataIntensityRange.lower);
	ui.spinWinMin->setMaximum(obj->m_dataIntensityRange.upper);
	ui.spinWinMin->setValue(obj->m_currentIntensityRange.lower);
	ui.spinWinMin->blockSignals(false);

	ui.spinWinMax->blockSignals(true);
	ui.spinWinMax->setMinimum(obj->m_dataIntensityRange.lower);
	ui.spinWinMax->setMaximum(obj->m_dataIntensityRange.upper);
	ui.spinWinMax->setValue(obj->m_currentIntensityRange.upper);
	ui.spinWinMax->blockSignals(false);

	ui.xBspin->blockSignals(true);
	ui.xBspin->setValue(obj->m_b.x);
	ui.xBspin->blockSignals(false);

	ui.yBspin->blockSignals(true);
	ui.yBspin->setValue(obj->m_b.y);
	ui.yBspin->blockSignals(false);

	ui.zBspin->blockSignals(true);
	ui.zBspin->setValue(obj->m_b.z);
	ui.zBspin->blockSignals(false);

	ui.xEspin->blockSignals(true);
	ui.xEspin->setValue(obj->m_e.x);
	ui.xEspin->blockSignals(false);

	ui.yEspin->blockSignals(true);
	ui.yEspin->setValue(obj->m_e.y);
	ui.yEspin->blockSignals(false);

	ui.zEspin->blockSignals(true);
	ui.zEspin->setValue(obj->m_e.z);
	ui.zEspin->blockSignals(false);

	updateFilters();

	updateInfo();
}

void PropVolTK::addColorWidgetButtonClicked()
{
	addColorWidget(2048, QColor::fromRgbF(1.0, 0.0, 0.0, 1.0), true);
}

void PropVolTK::winMinValueChanged(int i)
{
	if (i < 0)
	{
		i = 0;

		//ui.spinWinMin->blockSignals(true);
		//ui.spinWinMin->setValue(i);
		//ui.spinWinMin->blockSignals(false);
	}
	else if (i > obj->m_currentIntensityRange.upper)
	{
		i = obj->m_currentIntensityRange.upper;

		//ui.spinWinMin->blockSignals(true);
		//ui.spinWinMin->setValue(i);
		//ui.spinWinMin->blockSignals(false);
	}

	obj->m_currentIntensityRange.lower = i;

	updateInfo();

	UI::updateAllViews();
}

void PropVolTK::winMaxValueChanged(int i)
{
	if (i > 0xffff)
	{
		i = 0xffff;

		//ui.spinWinMax->blockSignals(true);
		//ui.spinWinMax->setValue(i);
		//ui.spinWinMax->blockSignals(false);
	}
	else if (i < obj->m_currentIntensityRange.lower)
	{
		i = obj->m_currentIntensityRange.lower;

		//ui.spinWinMax->blockSignals(true);
		//ui.spinWinMax->setValue(i);
		//ui.spinWinMax->blockSignals(false);
	}


	obj->m_currentIntensityRange.upper = i;

	updateInfo();

	UI::updateAllViews();
}

void PropVolTK::updateColorFilters()
{
	obj->colorFilters.clear();
	for (auto w : ui.scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
	{
		int key = ((VolTKcolorWidget*)w)->winMax();
		QColor val = ((VolTKcolorWidget*)w)->color();

		obj->colorFilters[key].first = ((VolTKcolorWidget*)w)->isActive();
		obj->colorFilters[key].second = CRGBA(val.redF(), (float)val.greenF(), (float)val.blueF(), (float)val.alphaF());
	}
	UI::updateAllViews();
}

void PropVolTK::removeFilter()
{
	qInfo() << "removeFilter pressed";

	//((QWidget*)sender())->deleteLater();
	delete (VolTKcolorWidget*)sender();
	updateColorFilters();
}

void PropVolTK::xValchanged(int i)
{
	MyScrollBar* s = (MyScrollBar *) sender();

	obj->m_b.x = i;
	obj->m_e.x = i + s->size;

	UI::updateAllViews();
}


void PropVolTK::xBchanged(int v)
{
	if (v >= obj->m_Min.x)
	{
		if (v >= obj->m_e.x)
		{
			if (v < obj->m_Max.x)
			{
				obj->m_e.x = v + 1;

				ui.xEspin->blockSignals(true);
				ui.xEspin->setValue(obj->m_e.x);
				ui.xEspin->blockSignals(false);
			}
		}
		obj->m_b.x = v;
		UI::updateAllViews();
	}
}

void PropVolTK::xEchanged(int v)
{
	if (v <= obj->m_Max.x)
	{
		if (v <= obj->m_b.x)
		{
			if (v > obj->m_Min.x)
			{
				obj->m_b.x = v - 1;

				ui.xBspin->blockSignals(true);
				ui.xBspin->setValue(obj->m_b.x);
				ui.xBspin->blockSignals(false);
			}
		}
		obj->m_e.x = v;
		UI::updateAllViews();

	}
}

void PropVolTK::yBchanged(int v)
{
	if (v >= obj->m_Min.y)
	{
		if (v >= obj->m_e.y)
		{
			if (v < obj->m_Max.y)
			{
				obj->m_e.y = v + 1;

				ui.yEspin->blockSignals(true);
				ui.yEspin->setValue(obj->m_e.y);
				ui.yEspin->blockSignals(false);
			}
		}
		obj->m_b.y = v;
		UI::updateAllViews();
	}
}

void PropVolTK::yEchanged(int v)
{
	if (v <= obj->m_Max.y)
	{
		if (v <= obj->m_b.y)
		{
			if (v > obj->m_Min.y)
			{
				obj->m_b.y = v - 1;

				ui.yBspin->blockSignals(true);
				ui.yBspin->setValue(obj->m_b.y);
				ui.yBspin->blockSignals(false);
			}
		}
		obj->m_e.y = v;
		UI::updateAllViews();

	}
}

void PropVolTK::zBchanged(int v)
{
	if (v >= obj->m_Min.z)
	{
		if (v >= obj->m_e.z)
		{
			if (v < obj->m_Max.z)
			{
				obj->m_e.z = v + 1;

				ui.zEspin->blockSignals(true);
				ui.zEspin->setValue(obj->m_e.z);
				ui.zEspin->blockSignals(false);
			}
		}
		obj->m_b.z = v;
		UI::updateAllViews();
	}
}

void PropVolTK::zEchanged(int v)
{
	if (v <= obj->m_Max.z)
	{
		if (v <= obj->m_b.z)
		{
			if (v > obj->m_Min.z)
			{
				obj->m_b.z = v - 1;

				ui.zBspin->blockSignals(true);
				ui.zBspin->setValue(obj->m_b.z);
				ui.zBspin->blockSignals(false);
			}
		}
		obj->m_e.z = v;
		UI::updateAllViews();
		
	}
}

void PropVolTK::xRangeChanged(int min, int max)
{
	obj->m_b.x = min;
	obj->m_e.x = max;

	updateProperties();
	UI::updateAllViews();
}
