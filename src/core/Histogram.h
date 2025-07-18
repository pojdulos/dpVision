#pragma once

#include <vector>
#include <algorithm>
#include "RGBA.h"
#include "Annotation.h"
#include "PointCloud.h"

class  DPVISION_EXPORT CHistogram : public CAnnotation
{
	std::vector<double> m_data;
	CPointCloud* m_destination;

	double m_dataMin;
	double m_dataMax;

	double m_lowerLimit;
	double m_upperLimit;

	int m_levels;
	bool m_absValues;

	double m_mean;
	double m_stdev;

	double m_werror;
	double m_wstdev;

public:
	typedef enum {
		BlueGreenRed,
		Grayscale,
		GreenRed
	} ColorMode;

	const char * colorModeString[3] = {
		"BlueGreenRed",
		"Grayscale",
		"GreenRed"
	};

	std::vector<size_t> m_values;
	size_t m_underRange;
	size_t m_overRange;
	size_t m_undefined;

	bool m_showOutOfRangeData;

	bool showOutlayers = false;

	bool showLabels = false;

	//size_t m_valMax;

	CHistogram( std::vector<double> data, int levels=512 );
	CHistogram( std::shared_ptr<CBaseObject> parent, std::vector<double> data, int levels = 512);
	CHistogram( CPointCloud* cloud, std::vector<double> data, int levels = 512);
	~CHistogram();

	void setData(std::vector<double> &data);
	std::vector<double> & getData();

	void setDest(CPointCloud* dest) { m_destination = dest; };
	CPointCloud* getDest() { return m_destination; };

	double getDataMin() { return m_dataMin; };
	double getDataMax() { return m_dataMax; };

	double getLowerLimit() { return m_lowerLimit; };
	double getUpperLimit() { return m_upperLimit; };
	std::pair<double, double> getLimits() { return std::pair<double, double>(m_lowerLimit, m_upperLimit); }
	void setLowerLimit(double lower = DBL_MAX);
	void setUpperLimit(double upper = -DBL_MAX);
	void setLimits(double lower = DBL_MAX, double upper = -DBL_MAX);

	void setLevels(int levels);
	int getLevels() { return m_levels; };

	void setAbsValues(bool b);
	bool getAbsValues() { return m_absValues; };


	double getMean() { return m_mean; };
	double getStDev() { return m_stdev; };

	double getWerror() { return m_werror; };
	double getWstDev() { return m_wstdev; };

	double getStep() { return (m_upperLimit - m_lowerLimit) / m_levels; };


	int getHistogramIndex( double value );

	void setColorMode(CHistogram::ColorMode mode) { m_colormode = mode; };
	CHistogram::ColorMode getColorMode() { return m_colormode; };
	
	inline CRGBA getColor(int index) { return getColor(index, m_colormode); };
	CRGBA getColor(int index, CHistogram::ColorMode mode );
	QColor getQColor(int index);
	CRGBA getColorGR(int index);
	CRGBA getColorBGR(int index);
	CRGBA getColorGray(int index);

	void repaint();

	virtual void setSelfVisibility(bool sel) override;

	void colorizeParentVertices(bool b=true);

	static std::pair<double,double> unifyLimits(std::vector<std::shared_ptr<CHistogram>>);
	static std::pair<double, double> unifyLimits(std::vector<std::shared_ptr<CHistogram>>, std::pair<double, double> );

	CHistogram::ColorMode m_colormode;

	std::pair<double, double> range(int i);

	size_t value(size_t i) { return m_values.at(i); };

	inline size_t maxBarValue() { return *std::max_element(std::begin(m_values), std::end(m_values)); };

	virtual int type() override { return CAnnotation::HISTOGRAM; }
	virtual std::wstring getTypeWSTR() { return L"histogram"; };

	static void savePLT(std::wstring fdir, std::wstring fname, std::vector<std::shared_ptr<CHistogram>> v, bool eqLevels );
	
	void save(QString fdir, QString fname);
	void save(std::wstring fdir, std::wstring fname);
	static void save(std::vector<std::shared_ptr<CHistogram>>, std::wstring fdir, std::wstring fname, bool showOutOfRangeData=false);

	double getNormalDistributionVal(double x);

	void calcGlobalError(double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX);

private:
	void recalcValues();

};

