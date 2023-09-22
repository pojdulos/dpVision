#ifndef DOCKWIDGETHISTOGRAM_H
#define DOCKWIDGETHISTOGRAM_H

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetHistogram.h"
#include "histogramwidget.h"

class CAnnotation;
class CFace;

class __declspec(dllexport) DockWidgetHistogram : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetHistogram(QWidget *parent = 0);
	~DockWidgetHistogram();

	HistogramWidget *hw;

	//void processData(std::vector<double> data, double min=DBL_MAX, double max=DBL_MIN)
	//{
	//	hw->processData(data, min, max);
	//}

	void setHistogram(CHistogram *histogram)
	{
		hw->setHistogram(histogram);
	}

	CHistogram* getHistogram();

public slots:

private:
	Ui::DockWidgetHistogram ui;

private slots:
};

#endif // DOCKWIDGETHISTOGRAM_H
