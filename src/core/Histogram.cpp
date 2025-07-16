#include "Histogram.h"

#include "Stat.h"
#include <algorithm>

#include "utils/StringUtils.h"
#include "StatusBarManager.h"

#include "UI.h"

CHistogram::CHistogram(std::vector<double> data, int levels) :CAnnotation()
{
	setLabel("histogram");
	m_colormode = CHistogram::ColorMode::BlueGreenRed;
	m_levels = levels;
	m_showSelf = false;

	m_absValues = false;

	m_showOutOfRangeData = true;// false;
	m_destination = nullptr;
	setData(data);
}

CHistogram::CHistogram(CModel3D* parent, std::vector<double> data, int levels) :CAnnotation()
{
	setLabel("histogram");
	m_colormode = CHistogram::ColorMode::BlueGreenRed;
	m_levels = levels;
	m_showSelf = false;

	m_absValues = false;

	m_showOutOfRangeData = true;// false;

	m_parent = (CBaseObject*)parent;
	m_destination = nullptr;
	setData(data);
}

CHistogram::CHistogram(CPointCloud* cloud, std::vector<double> data, int levels) :CAnnotation()
{
	setLabel("histogram");
	m_colormode = CHistogram::ColorMode::BlueGreenRed;
	m_levels = levels;
	m_showSelf = false;

	m_absValues = false;

	m_showOutOfRangeData = true;// false;

	m_parent = cloud->getParent();
	m_destination = nullptr;
	setData(data);
}


CHistogram::~CHistogram() {}


void CHistogram::setSelfVisibility(bool sel) {
	if (UI::DOCK::HISTOGRAM::getHistogram() == this) {
		UI::DOCK::HISTOGRAM::show(sel);
	}
	else if (sel) {
		if (NULL != UI::DOCK::HISTOGRAM::getHistogram()) {
			UI::DOCK::HISTOGRAM::getHistogram()->setSelfVisibility(false);
			UI::DOCK::WORKSPACE::setItemVisibleById(UI::DOCK::HISTOGRAM::getHistogram()->id(), false);
		}
		UI::DOCK::HISTOGRAM::setHistogram(this);
		UI::DOCK::HISTOGRAM::show(true);
	}

	//colorizeParentVertices(sel);
	if (m_destination != nullptr) colorizeParentVertices(sel);

	CAnnotation::setSelfVisibility(sel);
};

#include "Mesh.h"
#include "Model3D.h"

void CHistogram::colorizeParentVertices(bool b)
{
	if (m_destination == nullptr) return;

	static std::vector<CRGBA> orgColors;

	if (b)
	{
		orgColors = m_destination->vcolors();

		double zakres = m_upperLimit - m_lowerLimit;

		m_destination->vcolors().resize(m_destination->vertices().size());

		for (int i = 0; i < m_destination->vertices().size(); i++)
		{
			StatusBarManager::setTextTimed(1000, QString("colorizing (%1)...").arg(i));

			int idx = getHistogramIndex(m_data[i]);
			m_destination->vcolors()[i] = getColor(idx);
		}
	}
	else
	{
		m_destination->vcolors() = orgColors;
	}
}

void CHistogram::setData(std::vector<double>& data)
{
	m_data = data;

	m_dataMin = DBL_MAX;
	m_dataMax = -DBL_MAX;

	//	for (std::vector<double>::iterator it = data.begin(); it != data.end(); it++)
	for (double val : data)
	{
		//double val = *it;

		if (val != DBL_MAX)
		{
			if (val > m_dataMax)
			{
				m_dataMax = val;
			}
		}

		if (val != -DBL_MAX)
		{
			if (val < m_dataMin)
			{
				m_dataMin = val;
			}
		}
	}

	m_lowerLimit = m_dataMin;
	m_upperLimit = m_dataMax;

	recalcValues();
}

std::vector<double>& CHistogram::getData()
{
	return m_data;
}


void CHistogram::setUpperLimit(double upper)
{
	if (upper != -DBL_MAX)
	{
		m_upperLimit = upper;
	}
	else
	{
		m_upperLimit = m_dataMax;
	}
	recalcValues();
}


void CHistogram::setLowerLimit(double lower)
{
	if (lower != DBL_MAX)
	{
		m_lowerLimit = lower;
	}
	else
	{
		m_lowerLimit = m_dataMin;
	}
	recalcValues();
}


void CHistogram::setLimits(double lower, double upper)
{
	setLowerLimit(lower);
	setUpperLimit(upper);
}


void CHistogram::setLevels(int levels)
{
	m_levels = levels;

	recalcValues();
}

void CHistogram::setAbsValues(bool b)
{
	m_absValues = b;

	if (b)
	{
		m_lowerLimit = 0;
		m_upperLimit = std::max(m_dataMax, abs(m_dataMin));
	}
	else
	{
		if (m_dataMin < 0)
			m_lowerLimit = m_dataMin;
		else
			m_lowerLimit = 0.0;

		m_upperLimit = m_dataMax;
	}

	recalcValues();
}

void CHistogram::recalcValues()
{
	m_underRange = 0;
	m_overRange = 0;
	m_undefined = 0;

	m_values = std::vector<size_t>(m_levels, 0);

	for (double value : m_data)
	{
		if (value == DBL_MAX)
		{
			m_undefined++;
		}
		else
		{
			int i;
			if (m_absValues)
				i = getHistogramIndex(abs(value));
			else
				i = getHistogramIndex(value);

			if (i == -1)
			{
				m_underRange++;
			}
			else if (i == -2)
			{
				m_overRange++;
			}
			else
			{
				m_values[i]++;
			}
		}
	}


	if (m_absValues)
	{
		std::vector<double> tmpData = m_data;
		for (auto& f : tmpData) { f = f < 0 ? -f : f; }
		m_mean = CStat::MeanError(tmpData, m_lowerLimit, m_upperLimit);
		m_stdev = CStat::standardDeviation(tmpData, m_mean, m_lowerLimit, m_upperLimit);
	}
	else
	{
		m_mean = CStat::MeanError(m_data, m_lowerLimit, m_upperLimit);
		m_stdev = CStat::standardDeviation(m_data, m_mean, m_lowerLimit, m_upperLimit);
	}


	calcGlobalError();
}


int CHistogram::getHistogramIndex(double value)
{
	if (value == DBL_MAX) return -3; //undefined
	if (value < m_lowerLimit) return -1; //poza dolnym zakresem
	if (value > m_upperLimit) return -2; //poza g�rnym zakresem

	if ((value == m_lowerLimit) && (value == m_upperLimit)) return (m_levels / 2) + 1;


	int i = floor((value - m_lowerLimit) * m_levels / (m_upperLimit - m_lowerLimit));

	return (i < m_levels) ? i : (m_levels - 1);
}


std::pair<double, double> CHistogram::range(int i)
{
	double lower = 0.0;
	double upper = 0.0;

	if (i == -1)
	{
		return std::pair<double, double>(-DBL_MAX, m_lowerLimit);
	}

	if (i == -2)
	{
		return std::pair<double, double>(m_upperLimit, DBL_MAX);
	}

	double fullRange = m_upperLimit - m_lowerLimit;

	lower = m_lowerLimit + fullRange * i / m_levels;
	upper = m_lowerLimit + fullRange * (i + 1) / m_levels;

	return std::pair<double, double>(lower, upper);
}


CRGBA CHistogram::getColor(int i, CHistogram::ColorMode mode)
{
	switch (mode) {
	case CHistogram::ColorMode::Grayscale:
		return getColorGray(i);
		break;
	case CHistogram::ColorMode::GreenRed:
		return getColorGR(i);
		break;
	case CHistogram::ColorMode::BlueGreenRed:
	default:
		return getColorBGR(i);
		break;
	}
}

QColor CHistogram::getQColor(int i)
{
	switch (m_colormode) {
	case CHistogram::ColorMode::Grayscale:
		return getColorGray(i).toArgb32();
		break;
	case CHistogram::ColorMode::GreenRed:
		return getColorGR(i).toArgb32();
		break;
	case CHistogram::ColorMode::BlueGreenRed:
	default:
		return getColorBGR(i).toArgb32();
		break;
	}
}

CRGBA CHistogram::getColorGray(int i)
{
	if (i == -1)
		if (showOutlayers) return CRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i == -2)
		if (showOutlayers) return CRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i <= -3) return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	i = round(((double)i * 256.0) / (double)m_levels);

	unsigned char r = i;
	unsigned char g = i;
	unsigned char b = i;

	return CRGBA(r, g, b, 255);
}

CRGBA CHistogram::getColorGR(int i)
{
	if (i == -1)
		if (showOutlayers) return CRGBA(0.0f, 0.7f, 0.0f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i == -2)
		if (showOutlayers) return CRGBA(0.7f, 0.0f, 0.0f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i <= -3) return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);


	//if (m_levels < 512)
	i = round(((double)i * 512.0) / (double)m_levels);
	//if (m_levels > 512) i = round(((double)i * 512.0) / (double)m_levels);

	unsigned char r = 255;
	unsigned char g = 255;

	if (i < 255) r = i;
	if (i > 255) g = 255 - (i - 256);

	return CRGBA(r, g, 0, 255);
}


CRGBA CHistogram::getColorBGR(int i)
{
	if (i == -1)
		if (showOutlayers) return CRGBA(0.0f, 0.0f, 0.7f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i == -2)
		if (showOutlayers) return CRGBA(0.7f, 0.0f, 0.0f, 1.0f);
		else return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	else if (i <= -3) return CRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	

	//if (m_levels < 1024)
	i = round(((double)i * 1024.0) / (double)m_levels);
	//if (m_levels > 1024) i = round(((double)i * 1024.0) / (double)m_levels);

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	if ((i >= 0) && (i < 256))
	{
		r = 0;
		g = i;
		b = 255;
	}
	else if ((i > 255) && (i < 512))
	{
		r = 0;
		g = 255;
		b = 255 - (i - 256);
	}
	else if ((i > 511) && (i < 768))
	{
		r = i - 512;
		g = 255;
		b = 0;
	}
	else if ((i > 767) && (i < 1024))
	{
		r = 255;
		g = 255 - (i - 768);
		b = 0;
	}
	return CRGBA(r, g, b, 255);
}

void CHistogram::repaint()
{
	colorizeParentVertices(false);
	UI::DOCK::HISTOGRAM::repaint();
	colorizeParentVertices(true);
}


std::pair<double, double> CHistogram::unifyLimits(std::vector < std::shared_ptr<CHistogram>> v)
{
	std::pair<double, double> limits(v[0]->getDataMin(), v[0]->getDataMax());

	for (int i = 1; i < v.size(); i++)
	{
		limits.first = std::min(limits.first, v[i]->getDataMin());
		limits.second = std::max(limits.second, v[i]->getDataMax());
	}

	return unifyLimits(v, limits);
}

std::pair<double, double> CHistogram::unifyLimits(std::vector<std::shared_ptr<CHistogram>> v, std::pair<double, double> limits)
{
	for (int i = 0; i < v.size(); i++)
	{
		v[i]->setLimits(limits.first, limits.second);
		v[i]->recalcValues();
	}

	return limits;
}

#include <fstream>


void CHistogram::savePLT(std::wstring fdir, std::wstring fname, std::vector<std::shared_ptr<CHistogram>> v, bool eqLevels)
{
	std::wstring wideStr = fdir + L"/" + fname + L".plt";
	std::ofstream plik(std::string(wideStr.begin(), wideStr.end()), std::ios::out);

	if (plik.is_open())
	{
		plik << "invsqrt2pi = 0.398942280401433\n";
		plik << "normal(x) = invsqrt2pi / stdev * exp(-0.5*((x - mean) / stdev)**2)\n";
		//plik << "normal(x, mu, sigma) = sigma <= 0 ? 1 / 0 : invsqrt2pi / sigma * exp(-0.5*((x - mu) / sigma)**2)\n";
		//"#normal(x, mu, sd) = (1 / (sd*sqrt(2 * pi)))*exp(-(x - mu)**2 / (2 * sd**2))"

		if (eqLevels)
		{
			plik << "dane = \"" << dpVisionCore::utils::wstr2str(fname) << ".csv\"\n";
		}
		else
		{
			for (int i = 0; i < v.size(); i++)
			{
				plik << "dane" << i << " = \"" << dpVisionCore::utils::wstr2str(fname) << i << ".csv\"\n";
			}
		}

		plik << "set term windows size 800, 800\n";
		plik << "set termopt enhanced\n";
		plik << "set style data histograms\n";
		plik << "set style fill solid 1.0 border -1\n";
		plik << "set palette defined(1 'blue', 2 'green', 3 'red')\n";

		if (v.size() > 1)
		{
			plik << "set multiplot layout " << v.size() << ", 1 rowsfirst\n";
			plik << "set lmargin at screen 0.09\n";
			for (int i = 0; i < v.size(); i++)
			{
				plik << "mean = " << v[i]->m_mean << '\n';
				plik << "stdev = " << v[i]->m_stdev << '\n';

				if (eqLevels)
				{
					plik << "plot dane using 1:" << i + 2 << ":1 with boxes lc palette title \"\", \\\n";
				}
				else
				{
					plik << "plot dane" << i << " using 1:2:1 with boxes lc palette title \"\", \\\n";
				}

				//plik << "     normal(x, mean, stdev) axes x1y2 with lines lw 3 lc 'red' notitle\n";
				plik << "     normal(x) axes x1y2 with lines lw 3 lc 'red' notitle\n";
			}
			plik << "unset multiplot\n";
		}
		else
		{
			plik << "mean = " << v[0]->m_mean << '\n';
			plik << "stdev = " << v[0]->m_stdev << '\n';

			plik << "plot dane using 1:2:1 with boxes lc palette title \"\"";
			//plik << "     normal(x, mean, stdev) axes x1y2 with lines lw 3 lc 'red' notitle\n";
			plik << ", \\\n     normal(x) axes x1y2 with lines lw 3 lc 'red' notitle";
			plik << "\n";
		}

		plik << "set output\n";

		plik.close();
	}
}

void CHistogram::save(QString fdir, QString fname)
{
	save(fdir.toStdWString(), fname.toStdWString());
}

void CHistogram::save(std::wstring fdir, std::wstring fname)
{
	std::wstring wideStr = fdir + L"/" + fname + L".csv";
	std::ofstream plik(std::string(wideStr.begin(), wideStr.end()), std::ios::out);

	if (plik.is_open())
	{
		if (m_showOutOfRangeData)
		{
			std::pair<double, double> r = range(-1);
			plik << r.second - getStep() << '\t' << m_underRange << std::endl;
		}

		for (int i = 0; i < m_levels; i++)
		{
			std::pair<double, double> r = range(i);

			plik.precision(8);
			plik.setf(std::ios::fixed);
			plik.unsetf(std::ios::scientific);

			plik << (r.first + r.second) / 2 << '\t' << m_values[i] << std::endl;
		}

		if (m_showOutOfRangeData)
		{
			std::pair<double, double> r = range(-2);
			plik << r.first + getStep() << '\t' << m_overRange << std::endl;
		}

		plik.close();

		CHistogram::savePLT(fdir, fname, std::vector<std::shared_ptr<CHistogram>>{std::dynamic_pointer_cast<CHistogram>(this->shared_from_this())}, true);

		StatusBarManager::setText("DONE !");
	}
}


void CHistogram::save(std::vector<std::shared_ptr<CHistogram>> v, std::wstring fdir, std::wstring fname, bool showOutOfRangeData)
{
	if (fname == L"") return;

	bool eqLevels = true;

	if (v.size() > 1)
	{
		//sprawdzam czy jednakowa liczba przedzia��w
		int lev = v[0]->m_levels;

		for (int j = 1; j < v.size(); j++)
		{
			if (lev != v[j]->m_levels)
			{
				eqLevels = false;
				break;
			}
		}
	}

	if (eqLevels)
	{
		std::wstring wideStr = fdir + L"/" + fname + L".csv";
		std::ofstream plik(std::string(wideStr.begin(), wideStr.end()), std::ios::out);

		//std::ofstream plik(fdir + L"/" + fname + L".csv", std::ios::out);

		if (plik.is_open())
		{
			if (showOutOfRangeData)
			{
				std::pair<double, double> r = v[0]->range(-1);
				plik << r.second - v[0]->getStep();

				for (int j = 0; j < v.size(); j++)
				{
					plik << '\t' << v[j]->m_underRange;
				}

				plik << std::endl;
			}

			for (int i = 0; i < v[0]->m_values.size(); i++)
			{
				std::pair<double, double> r = v[1]->range(i);

				//plik << i << '\t' << r.first << '\t' << r.second;
				plik << (r.first + r.second) / 2;

				for (int j = 0; j < v.size(); j++)
				{
					plik << '\t' << v[j]->m_values[i];
				}

				plik << std::endl;
			}

			if (showOutOfRangeData)
			{
				std::pair<double, double> r = v[0]->range(-2);
				plik << r.first + v[0]->getStep();

				for (int j = 0; j < v.size(); j++)
				{
					plik << '\t' << v[j]->m_overRange;
				}

				plik << std::endl;
			}

			plik.close();

		}
	}
	else
	{
		for (int j = 0; j < v.size(); j++)
		{
			std::wstring wideStr = fdir + L"/" + fname + std::to_wstring(j) + L".csv";
			std::ofstream plik(std::string(wideStr.begin(), wideStr.end()), std::ios::out);

			//std::ofstream plik(fdir + L"/" + fname + std::to_wstring(j) + L".csv", std::ios::out);

			if (plik.is_open())
			{
				if (showOutOfRangeData)
				{
					std::pair<double, double> r = v[j]->range(-1);
					plik << r.second - v[j]->getStep() << '\t' << v[j]->m_underRange << std::endl;
				}

				for (int i = 0; i < v[j]->m_values.size(); i++)
				{
					std::pair<double, double> r = v[j]->range(i);

					//plik << i << '\t' << r.first << '\t' << r.second;
					plik << (r.first + r.second) / 2 << '\t' << v[j]->m_values[i] << std::endl;
				}

				if (showOutOfRangeData)
				{
					std::pair<double, double> r = v[j]->range(-2);
					plik << r.first + v[j]->getStep() << '\t' << v[j]->m_overRange << std::endl;
				}

				plik.close();

			}
		}

	}

	CHistogram::savePLT(fdir, fname, v, eqLevels);

	StatusBarManager::setText("DONE !");
}


double CHistogram::getNormalDistributionVal(double x)
{
	return CStat::getNormalDistributionValue(x, m_mean, m_stdev);
}


void CHistogram::calcGlobalError(double lowerLimit, double upperLimit)
{
	double error = 0.0;
	double sumArea = 0.0;

	CModel3D* obj = (CModel3D*)getParent();

	if (NULL == obj) {
		m_werror = NAN;
		m_wstdev = NAN;
		return;
	}

	if (obj->getChild()->type() != CBaseObject::MESH) return;

	CMesh* m = (CMesh*)obj->getChild().get();


	std::vector<double> odleglosci = getData();

	//std::map<size_t, std::set<size_t>> v2fMap;
	std::map<size_t, double> faceArea;

	std::map<size_t, double> vertexWeight;
	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		vertexWeight[iv] = 0.0;
	}

	for (int i = 0; i < m->faces().size(); i++)
	{
		CFace* f = &m->faces()[i];

		faceArea[i] = CFace::triangleArea(m->vertices()[f->A()], m->vertices()[f->B()], m->vertices()[f->C()]);

		//if (odleglosci[f->A()] != DBL_MAX) {
		if ((odleglosci[f->A()] >= m_lowerLimit) && (odleglosci[f->A()] <= m_upperLimit)) {
			sumArea += faceArea[i];
			vertexWeight[f->A()] += faceArea[i];
			//v2fMap[f->A()].insert(i);
		}

		//if (odleglosci[f->B()] != DBL_MAX) {
		if ((odleglosci[f->B()] >= m_lowerLimit) && (odleglosci[f->B()] <= m_upperLimit)) {
			sumArea += faceArea[i];
			vertexWeight[f->B()] += faceArea[i];
			//v2fMap[f->B()].insert(i);
		}

		//if (odleglosci[f->C()] != DBL_MAX) {
		if ((odleglosci[f->C()] >= m_lowerLimit) && (odleglosci[f->C()] <= m_upperLimit)) {
			sumArea += faceArea[i];
			vertexWeight[f->C()] += faceArea[i];
			//v2fMap[f->C()].insert(i);
		}
	}

	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		//if (odleglosci[iv] != DBL_MAX) {
		if ((odleglosci[iv] >= m_lowerLimit) && (odleglosci[iv] <= m_upperLimit)) {
			double e = (odleglosci[iv] * vertexWeight[iv]) / sumArea;
			error += e;
		}
	}

	int licznik = 0;
	double wariancja = 0.0;
	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		//if (odleglosci[iv] != DBL_MAX) {
		if ((odleglosci[iv] >= m_lowerLimit) && (odleglosci[iv] <= m_upperLimit)) {
			wariancja += pow(odleglosci[iv] - error, 2);
			licznik++;
		}
	}
	wariancja /= licznik;

	m_werror = error;
	m_wstdev = sqrt(wariancja);
}

