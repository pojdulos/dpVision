#ifndef PROPVOLTK_H
#define PROPVOLTK_H

#include "propBaseObject.h"
#include "ui_propVolTK.h"

#include <QScrollBar>
#include <QWheelEvent>
class MyScrollBar : public QScrollBar
{
	Q_OBJECT;
public:
	explicit MyScrollBar(QWidget* parent = 0) :QScrollBar(parent)
	{
		size = 300;
		setPageStep(size);
	};

	int size;
protected:
	void wheelEvent(QWheelEvent* event) override
	{
		int val = this->value();

		const int maxsize = 512-val;
		const int minsize = 1;

		int d = event->angleDelta().y();

		if ((d > 0) && (size < maxsize))
		{
			size++;
		}
		else if ((d < 0) && (size > minsize))
		{
			size--;
		}

		int newMax = 512-size;

		setMaximum(newMax);

		setPageStep(size);

		emit(valueChanged(val));
	};
};

class CVolTK;
class QColorDialog;

class PropVolTK : public PropWidget
{
	Q_OBJECT

	CVolTK* obj;
	
	//QMap<int, QWidget*> filtry;
public:
	explicit PropVolTK(CVolTK *voltk, QWidget *parent = 0);
	~PropVolTK();

	void addColorWidget(int winMax, QColor color, bool active, bool update=true);

	static PropWidget* create(CVolTK* vol, QWidget* parent);

	void updateInfo();

	void updateFilters();

	virtual void updateProperties() override;

public slots:
	void addColorWidgetButtonClicked();
	
	void winMinValueChanged(int);
	void winMaxValueChanged(int);
	void updateColorFilters();
	//void gValueChanged(int);
	//void bValueChanged(int);
	//void aValueChanged(int);
	//
	//void rgbaTextChanged(QString);

	//void meshColorChanged(QColor);
	//void colorButtonClicked();
	void removeFilter();
	void xValchanged(int);
	void xBchanged(int);
	void yBchanged(int);
	void zBchanged(int);
	void xEchanged(int);
	void yEchanged(int);
	void zEchanged(int);
	void xRangeChanged(int, int);

private:
	Ui::propVolTK ui;

	//QColorDialog *cd;

	//void updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	//void updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

#endif // PROPVOLTK_H
