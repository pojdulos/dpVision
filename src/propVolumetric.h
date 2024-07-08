#ifndef PROPVOLUMETRIC_H
#define PROPVOLUMETRIC_H

#include "propBaseObject.h"
#include "ui_propVolumetric.h"

//#include <QScrollBar>
//#include <QWheelEvent>
//class MyScrollBar : public QScrollBar
//{
//	Q_OBJECT;
//public:
//	explicit MyScrollBar(QWidget* parent = 0) :QScrollBar(parent)
//	{
//		size = 300;
//		setPageStep(size);
//	};
//
//	int size;
//protected:
//	void wheelEvent(QWheelEvent* event) override
//	{
//		int val = this->value();
//
//		const int maxsize = 512-val;
//		const int minsize = 1;
//
//		int d = event->angleDelta().y();
//
//		if ((d > 0) && (size < maxsize))
//		{
//			size++;
//		}
//		else if ((d < 0) && (size > minsize))
//		{
//			size--;
//		}
//
//		int newMax = 512-size;
//
//		setMaximum(newMax);
//
//		setPageStep(size);
//
//		emit(valueChanged(val));
//	};
//};

class Volumetric;
class QColorDialog;

class PropVolumetric : public PropWidget
{
	Q_OBJECT

	Volumetric* obj;
	std::vector<QCheckBox*> f_check;
	std::vector<QDoubleSpinBox*> spin_min;
	std::vector<QDoubleSpinBox*> spin_max;

	//QMap<int, QWidget*> filtry;
public:
	explicit PropVolumetric(Volumetric *voltk, QWidget *parent = 0);
	~PropVolumetric();

	//void addColorWidget(int winMax, QColor color, bool active, bool update=true);

	static PropWidget* create(Volumetric* vol, QWidget* parent);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	void updateInfo();

	//void updateFilters();

	virtual void updateProperties() override;

	void change_filter_min(int idx, double d);
	void change_filter_max(int idx, double d);

public slots:
	void winMinValueChanged(double);
	void winMaxValueChanged(double);
	void xBchanged(int);
	void yBchanged(int);
	void zBchanged(int);
	void xEchanged(int);
	void yEchanged(int);
	void zEchanged(int);
	void on_fast_draw_checkbox(bool);
	void on_render_boxes_checkbox(bool);
	void on_f0_min_changed(double);
	void on_f0_max_changed(double);
	void on_f0_checkbox(bool);
	void on_f1_min_changed(double d);
	void on_f1_max_changed(double d);
	void on_f1_checkbox(bool b);
	void on_f2_min_changed(double d);
	void on_f2_max_changed(double d);
	void on_f2_checkbox(bool b);
	void on_f3_min_changed(double d);
	void on_f3_max_changed(double d);
	void on_f3_checkbox(bool b);
	void on_f4_min_changed(double d);
	void on_f4_max_changed(double d);
	void on_f4_checkbox(bool b);
	void on_f5_min_changed(double d);
	void on_f5_max_changed(double d);
	void on_f5_checkbox(bool b);
	void on_f6_min_changed(double d);
	void on_f6_max_changed(double d);
	void on_f6_checkbox(bool b);
private:
	Ui::propVolumetric ui;

	//QColorDialog *cd;

	//void updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	//void updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

#endif // PROPVOLUMETRIC_H
