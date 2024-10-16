#pragma once

#include "dll_global.h"

#include <vector>
#include <cstddef>
#include <cfloat>

class  DPVISION_EXPORT CStat
{
public:
	typedef enum {
		Random,
		Normal
	} DistType;

	typedef enum {
		Sample,
		Population
	} DataType;

	static double KolmogorovSmirnovTest(std::vector<double>& data);

	static double avg(std::vector<double>& data);

	static double MeanError(std::vector<double>& data, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX);
	static double MeanSquareError(std::vector<double>& data, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX);
	static double Variance(std::vector<double>& data, double mean, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX, CStat::DataType dtype = CStat::DataType::Population);
	static double standardDeviation(std::vector<double>& data, double mean, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX, CStat::DataType dtype = CStat::DataType::Population);
	static double quantile(std::vector<double>& data, double q);
	static double quantile2(std::vector<double>& data, double q);

	static double getNormalDistributionValue(double x, double mean, double stdev);
	static std::vector<double> genData(CStat::DistType type, size_t size, double mean, double stdev);
private:
	static std::vector<double> genDataNormal(size_t size, double mean, double stdev);
	static std::vector<double> genDataRandom(size_t size, double dmin, double dmax);
};
