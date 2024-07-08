#include "propVolumetric.h"
#include "Mesh.h"
#include "UI.h"

#include <QColorDialog>
#include <QPushButton>
#include "MainWindow.h"

#include "Volumetric.h"
//#include "volTKcolorWidget.h"

#include "RangeSlider.h"

PropVolumetric::PropVolumetric(Volumetric *voltk, QWidget *parent) : PropWidget( parent )
{
	this->obj = voltk;
	ui.setupUi((QWidget*)this);

	f_check = { ui.f0CheckBox, ui.f1CheckBox, ui.f2CheckBox, ui.f3CheckBox, ui.f4CheckBox, ui.f5CheckBox, ui.f6CheckBox };
	spin_min = { ui.f0SpinMin, ui.f1SpinMin, ui.f2SpinMin, ui.f3SpinMin, ui.f4SpinMin, ui.f5SpinMin, ui.f6SpinMin };
	spin_max = { ui.f0SpinMax, ui.f1SpinMax, ui.f2SpinMax, ui.f3SpinMax, ui.f4SpinMax, ui.f5SpinMax, ui.f6SpinMax };

	update();
}

PropVolumetric::~PropVolumetric()
{
}

//void PropVolumetric::addColorWidget(int winMax, QColor color, bool active, bool update)
//{
//	//QWidget* widget = new QWidget();
//	//QHBoxLayout* layout = new QHBoxLayout(widget);
//	//layout->setContentsMargins(QMargins(0, 0, 0, 0));
//
//	VolTKcolorWidget* cwidget = new VolTKcolorWidget(this);
//	cwidget->setWinMax(winMax);
//	cwidget->setColor(color);
//	cwidget->setActive(active);
//	cwidget->setVisible(true);
//
//	//layout->addWidget(cwidget);
//
//	//QPushButton* remButton = new QPushButton("x");
//	//layout->addWidget(remButton);
//
//	//widget->resize(layout->sizeHint());
//	//widget->setMinimumSize(layout->sizeHint());
//	//widget->setMaximumSize(layout->sizeHint());
//
//	//widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//
//	ui.scrollAreaWidgetContents->layout()->addWidget(cwidget);
//
//	if (update)
//		updateColorFilters();
//}

PropWidget* PropVolumetric::create(Volumetric* vol, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(vol, widget));
	layout->addWidget(new PropVolumetric(vol, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropVolumetric::create_and_get_subwidgets(CBaseObject* obj)
{
	Volumetric* vol = (Volumetric*)obj;
	return QVector<PropWidget*>({
		new PropBaseObject(vol),
		new PropVolumetric(vol)
		});
}


void PropVolumetric::updateInfo()
{
	ui.info->blockSignals(true);
	ui.info->setText(QString::fromStdWString(obj->infoRow()));
	ui.info->blockSignals(false);
}

//void PropVolumetric::updateFilters()
//{
//	QWidgetList widgets = ui.scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
//
//	QWidgetList::iterator w = widgets.begin();
//	for (CVolTK::ColorFilter cf : obj->colorFilters)
//	{
//		if (w != widgets.end())
//		{
//			((VolTKcolorWidget*)*w)->setWinMax(cf.first);
//			((VolTKcolorWidget*)*w)->setActive(cf.second.first);
//			((VolTKcolorWidget*)*w)->setColor(QColor::fromRgba(cf.second.second.toArgb32()));
//			w++;
//		}
//		else
//		{
//			addColorWidget(cf.first, QColor::fromRgba(cf.second.second.toArgb32()), cf.second.first, false);
//		}
//	}
//
//	while (w != widgets.end())
//	{
//		VolTKcolorWidget* tmp = (VolTKcolorWidget*)*w;
//		w++;
//
//		delete tmp;
//	}
//}

void PropVolumetric::updateProperties()
{
	ui.spinWinMin->blockSignals(true);
	ui.spinWinMin->setMinimum(obj->m_minVal);
	ui.spinWinMin->setMaximum(obj->m_maxDisplWin);
	ui.spinWinMin->setValue(obj->m_minDisplWin);
	ui.spinWinMin->blockSignals(false);

	ui.spinWinMax->blockSignals(true);
	ui.spinWinMax->setMinimum(obj->m_minDisplWin);
	ui.spinWinMax->setMaximum(obj->m_maxVal);
	ui.spinWinMax->setValue(obj->m_maxDisplWin);
	ui.spinWinMax->blockSignals(false);

	ui.xBspin->blockSignals(true);
	ui.xBspin->setValue(obj->m_minColumn);
	ui.xBspin->blockSignals(false);

	ui.yBspin->blockSignals(true);
	ui.yBspin->setValue(obj->m_minRow);
	ui.yBspin->blockSignals(false);

	ui.zBspin->blockSignals(true);
	ui.zBspin->setValue(obj->m_minSlice);
	ui.zBspin->blockSignals(false);

	ui.xEspin->blockSignals(true);
	ui.xEspin->setValue(obj->m_maxColumn);
	ui.xEspin->blockSignals(false);

	ui.yEspin->blockSignals(true);
	ui.yEspin->setValue(obj->m_maxRow);
	ui.yEspin->blockSignals(false);

	ui.zEspin->blockSignals(true);
	ui.zEspin->setValue(obj->m_maxSlice);
	ui.zEspin->blockSignals(false);

	for (int idx = 0; idx < 7; idx++) {
		this->f_check[idx]->blockSignals(true);
		this->f_check[idx]->setChecked(this->obj->m_filters[idx][0] != 0);
		this->f_check[idx]->blockSignals(false);

		this->spin_min[idx]->blockSignals(true);
		this->spin_max[idx]->blockSignals(true);

		this->spin_min[idx]->setMinimum(this->obj->m_minVal);
		this->spin_min[idx]->setValue(this->obj->m_filters[idx][1]);

		this->spin_max[idx]->setMaximum(this->obj->m_maxVal);
		this->spin_max[idx]->setValue(this->obj->m_filters[idx][2]);

		this->spin_max[idx]->setMinimum(std::max(this->obj->m_minVal, (float)this->spin_min[idx]->value()));
		this->spin_min[idx]->setMaximum(std::min(this->obj->m_maxVal, (float)this->spin_max[idx]->value()));

		this->spin_min[idx]->blockSignals(false);
		this->spin_max[idx]->blockSignals(false);
	}

	ui.fastDrawCheckBox->blockSignals(true);
	ui.fastDrawCheckBox->setChecked(obj->m_fastDraw);
	ui.fastDrawCheckBox->blockSignals(false);
	
	ui.renderBoxesCheckBox->blockSignals(true);
	ui.renderBoxesCheckBox->setChecked(obj->m_renderBoxes);
	ui.renderBoxesCheckBox->blockSignals(false);

	updateInfo();
}

//void PropVolumetric::addColorWidgetButtonClicked()
//{
//	addColorWidget(2048, QColor::fromRgbF(1.0, 0.0, 0.0, 1.0), true);
//}

void PropVolumetric::winMinValueChanged(double i)
{
	double val = i;

	if (val > this->obj->m_maxDisplWin)
		val = this->obj->m_maxDisplWin;
	else if (val < this->obj->m_minVal)
		val = this->obj->m_minVal;

	this->obj->m_minDisplWin = val;
	ui.spinWinMax->blockSignals(true);
	ui.spinWinMax->setMinimum(val);
	ui.spinWinMax->blockSignals(false);

	updateInfo();
	UI::updateAllViews();
}

void PropVolumetric::winMaxValueChanged(double i)
{
	double val = i;

	if (val < this->obj->m_minDisplWin)
		val = this->obj->m_minDisplWin;
	else if (val > this->obj->m_maxVal)
		val = this->obj->m_maxVal;

	this->obj->m_maxDisplWin = val;
	ui.spinWinMin->blockSignals(true);
	ui.spinWinMin->setMaximum(val);
	ui.spinWinMin->blockSignals(false);

	updateInfo();
	UI::updateAllViews();
}

//void PropVolumetric::updateColorFilters()
//{
//	obj->colorFilters.clear();
//	for (auto w : ui.scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
//	{
//		int key = ((VolTKcolorWidget*)w)->winMax();
//		QColor val = ((VolTKcolorWidget*)w)->color();
//
//		obj->colorFilters[key].first = ((VolTKcolorWidget*)w)->isActive();
//		obj->colorFilters[key].second = CRGBA(val.redF(), (float)val.greenF(), (float)val.blueF(), (float)val.alphaF());
//	}
//	UI::updateAllViews();
//}
//
//void PropVolumetric::removeFilter()
//{
//	qInfo() << "removeFilter pressed";
//
//	//((QWidget*)sender())->deleteLater();
//	delete (VolTKcolorWidget*)sender();
//	updateColorFilters();
//}

//void PropVolumetric::xValchanged(int i)
//{
//	MyScrollBar* s = (MyScrollBar *) sender();
//
//	obj->m_b.x = i;
//	obj->m_e.x = i + s->size;
//
//	UI::updateAllViews();
//}


void PropVolumetric::xBchanged(int v)
{
	int val = v;

	if (val > this->obj->m_maxColumn)
		val = this->obj->m_maxColumn;
	else if (val < 0)
		val = 0;

	this->obj->m_minColumn = val;
	ui.xEspin->blockSignals(true);
	ui.xEspin->setMinimum(val);
	ui.xEspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::xEchanged(int v)
{
	int val = v;

	if (val < this->obj->m_minColumn)
		val = this->obj->m_minColumn;
	else if (val >= this->obj->m_shape[2])
		val = this->obj->m_shape[2] - 1;

	this->obj->m_maxColumn = val;
	ui.xBspin->blockSignals(true);
	ui.xBspin->setMaximum(val);
	ui.xBspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::yBchanged(int v)
{
	int val = v;

	if (val > this->obj->m_maxRow)
		val = this->obj->m_maxRow;
	else if (val < 0)
		val = 0;

	this->obj->m_minRow = val;
	ui.yEspin->blockSignals(true);
	ui.yEspin->setMinimum(val);
	ui.yEspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::yEchanged(int v)
{
	int val = v;

	if (val < this->obj->m_minRow)
		val = this->obj->m_minRow;
	else if (val >= this->obj->m_shape[1])
		val = this->obj->m_shape[1] - 1;

	this->obj->m_maxRow = val;
	ui.yBspin->blockSignals(true);
	ui.yBspin->setMaximum(val);
	ui.yBspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::zBchanged(int v)
{
	int val = v;

	if (val > this->obj->m_maxSlice)
		val = this->obj->m_maxSlice;
	else if (val < 0)
		val = 0;

	this->obj->m_minSlice = val;
	ui.zEspin->blockSignals(true);
	ui.zEspin->setMinimum(val);
	ui.zEspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::zEchanged(int v)
{
	int val = v;

	if (val < this->obj->m_minSlice)
		val = this->obj->m_minSlice;
	else if (val >= this->obj->m_shape[0])
		val = this->obj->m_shape[0] - 1;

	this->obj->m_maxSlice = val;
	ui.zBspin->blockSignals(true);
	ui.zBspin->setMaximum(val);
	ui.zBspin->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::on_fast_draw_checkbox(bool b)
{
	this->obj->m_fastDraw = b;
	UI::updateAllViews();
}

void PropVolumetric::on_render_boxes_checkbox(bool b)
{
	this->obj->m_renderBoxes = b;
	this->obj->remove_shader_program();
	UI::updateAllViews();
}


void PropVolumetric::change_filter_min(int idx, double d) {
	double val = d;

	if (val > this->obj->m_filters[idx][2])
		val = this->obj->m_filters[idx][2];
	else if (val < this->obj->m_minDisplWin)
		val = this->obj->m_minDisplWin;

	this->obj->m_filters[idx][1] = val;
	this->spin_max[idx]->blockSignals(true);
	this->spin_max[idx]->setMinimum(val);
	this->spin_max[idx]->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::change_filter_max(int idx, double d) {
	double val = d;

	if (val < this->obj->m_filters[idx][1])
		val = this->obj->m_filters[idx][1];
	else if (val > this->obj->m_maxDisplWin)
		val = this->obj->m_maxDisplWin;

	this->obj->m_filters[idx][2] = val;
	this->spin_min[idx]->blockSignals(true);
	this->spin_min[idx]->setMaximum(val);
	this->spin_min[idx]->blockSignals(false);
	UI::updateAllViews();
}

void PropVolumetric::on_f0_min_changed(double d) { change_filter_min(0, d); }
void PropVolumetric::on_f1_min_changed(double d) { change_filter_min(1, d); }
void PropVolumetric::on_f2_min_changed(double d) { change_filter_min(2, d); }
void PropVolumetric::on_f3_min_changed(double d) { change_filter_min(3, d); }
void PropVolumetric::on_f4_min_changed(double d) { change_filter_min(4, d); }
void PropVolumetric::on_f5_min_changed(double d) { change_filter_min(5, d); }
void PropVolumetric::on_f6_min_changed(double d) { change_filter_min(6, d); }

void PropVolumetric::on_f0_max_changed(double d) { change_filter_max(0, d); }
void PropVolumetric::on_f1_max_changed(double d) { change_filter_max(1, d); }
void PropVolumetric::on_f2_max_changed(double d) { change_filter_max(2, d); }
void PropVolumetric::on_f3_max_changed(double d) { change_filter_max(3, d); }
void PropVolumetric::on_f4_max_changed(double d) { change_filter_max(4, d); }
void PropVolumetric::on_f5_max_changed(double d) { change_filter_max(5, d); }
void PropVolumetric::on_f6_max_changed(double d) { change_filter_max(6, d); }

void PropVolumetric::on_f0_checkbox(bool b) { this->obj->m_filters[0][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f1_checkbox(bool b) { this->obj->m_filters[1][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f2_checkbox(bool b) { this->obj->m_filters[2][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f3_checkbox(bool b) { this->obj->m_filters[3][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f4_checkbox(bool b) { this->obj->m_filters[4][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f5_checkbox(bool b) { this->obj->m_filters[5][0] = b ? 1 : 0; UI::updateAllViews(); }
void PropVolumetric::on_f6_checkbox(bool b) { this->obj->m_filters[6][0] = b ? 1 : 0; UI::updateAllViews(); }

