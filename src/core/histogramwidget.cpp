#include "histogramwidget.h"
#include <QPainter>
#include "Histogram.h"

HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent), m_histogram(NULL) {}
HistogramWidget::~HistogramWidget() {}

void HistogramWidget::setHistogram(CHistogram *histogram)
{
    m_histogram = histogram;
    update();
}

void HistogramWidget::writeValue(QPainter &painter, double low, double up, int x, int y)
{
	painter.save();
	painter.translate(x, y);  // Przesuñ punkt (0,0) do pozycji (x,y)
	painter.rotate(-90);       // Obróæ o 90 stopni wokó³ punktu (0,0)
	painter.drawText(0, 0, QString::number(low) + " - " + QString::number(up));
	painter.restore();

	//painter.drawText(x, y, QString::number(value));
}

void HistogramWidget::drawBar(QPainter& painter, int ix, qreal w, int colNb, size_t maxBarVal)
{
	QColor c = m_histogram->getQColor(colNb);

	qreal h;
	if (colNb == -1)
		h = (qreal)(m_histogram->m_underRange) * height() / maxBarVal;
	else if (colNb==-2)
		h = (qreal)(m_histogram->m_overRange) * height() / maxBarVal;
	else 
		h = (qreal)m_histogram->m_values[colNb] * height() / maxBarVal;

	int left = w * ix;
	int top = height() - h;

	painter.fillRect(left, top, w, h, c);
	painter.drawRect(left, top, w, h);
}

void HistogramWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if ( ( m_histogram == NULL ) || (m_histogram->m_values.empty() ) ) {
        painter.fillRect(0, 0, width(), height(), QColor::fromRgb(0, 128, 0));
        return;
    }

	size_t nbBars = m_histogram->m_values.size();
	size_t maxBarVal = m_histogram->maxBarValue();

	if (m_histogram->showOutlayers)
	{
		if (m_histogram->m_underRange > 0)
		{
			nbBars++;
			maxBarVal = std::max(maxBarVal, m_histogram->m_underRange);
		}

		if (m_histogram->m_overRange > 0)
		{
			nbBars++;
			maxBarVal = std::max(maxBarVal, m_histogram->m_overRange);
		}
	}

	qreal barWidth = width() / (qreal)nbBars;

	int ix = 0;

	int trueHeight = height();

	if ( (m_histogram->showOutlayers) && (m_histogram->m_underRange > 0) )
	{
		drawBar(painter, ix, barWidth, -1, maxBarVal);
		ix++;
	}

    for (int i = 0; i < m_histogram->m_values.size(); i++) {

		drawBar(painter, ix, barWidth, i, maxBarVal);

		if (m_histogram->showLabels)
		{
			std::pair<double, double> r = m_histogram->range(i);
			writeValue(painter, r.first, r.second, barWidth * ix, trueHeight - 10);
		}

		ix++;
	}


	if ( (m_histogram->showOutlayers) && (m_histogram->m_overRange > 0) )
	{
		drawBar(painter, ix, barWidth, -2, maxBarVal);
	}

	//i++;
	//h = (qreal)(m_histogram->m_undefined) * height() / m_histogram->maxBarValue();
	//color = m_histogram->getColor(-3);
	//c = QColor(color.R(), color.G(), color.B(), color.A());
	//painter.fillRect(barWidth * i, height() - h, barWidth * (i + 1), height(), c);
	//painter.fillRect(barWidth * i, 0, barWidth * (i + 1), height() - h, Qt::gray);


	//size_t sumVal = std::accumulate(std::begin(m_histogram->m_values), std::end(m_histogram->m_values), 0);

	//int meanPos = (m_histogram->getMean() - m_histogram->getLowerLimit()) * width() / (m_histogram->getUpperLimit() - m_histogram->getLowerLimit());
	//int stdevPos1 = ((m_histogram->getMean() - m_histogram->getStDev()) - m_histogram->getLowerLimit()) * width() / (m_histogram->getUpperLimit() - m_histogram->getLowerLimit());
	//int stdevPos2 = ((m_histogram->getMean() + m_histogram->getStDev()) - m_histogram->getLowerLimit()) * width() / (m_histogram->getUpperLimit() - m_histogram->getLowerLimit());

	//draw mean and stdev
	//QPen penHLines(QColor("#ff0000"), 1, Qt::DotLine, Qt::FlatCap, Qt::RoundJoin);
	//painter.setPen(penHLines);
	//painter.drawLine(stdevPos1, 0, stdevPos1, height());

	//painter.drawLine(meanPos, 0, meanPos, height());

	//painter.drawLine(stdevPos2, 0, stdevPos2, height());

	//------------------

	//double maxVal = m_histogram->getNormalDistributionVal(m_histogram->getMean());

	//double step = (m_histogram->getUpperLimit() - m_histogram->getLowerLimit()) / (double)width();
	//double x1 = m_histogram->getLowerLimit();
	//double y1 = m_histogram->getNormalDistributionVal(x1);
	//double yy1 = y1 * height() / maxVal;
	//
	//QPen pen(QColor("#0000ff"), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
	//painter.setPen(pen);

	//for (int i = 0; i < (width() - 1); i++)
	//{
	//	double x2 = m_histogram->getLowerLimit() + step * (i + 1);
	//	double y2 = m_histogram->getNormalDistributionVal(x2);
	//	double yy2 = y2 * height() / maxVal;

	//	painter.drawLine(i, height() - yy1, i + 1, height() - yy2);

	//	x1 = x2;
	//	y1 = y2;
	//	yy1 = yy2;
	//}

}

