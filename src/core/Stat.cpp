#include "Stat.h"

#include <algorithm>
//#include "UI.h"
#include <random>
#include <cmath>

#include <numeric>

double CStat::avg(std::vector<double>& data)
{
	auto n = data.size();
	double average = 0.0;
	if (n != 0) {
		average = std::accumulate(data.begin(), data.end(), 0.0) / n;
	}

	return average;
}

double CStat::MeanError(std::vector<double>& data, double lowerLimit, double upperLimit)
{
	int cnt = 0;
	double sum = 0.0;

	std::for_each(std::begin(data), std::end(data), [&](const double d) {
		if ((d != -DBL_MAX) && (d != DBL_MAX))
		{
			if ((d >= lowerLimit) && (d <= upperLimit))
			{
				sum += d;
				cnt++;
			}
		}
		});

	return sum / cnt;
}

double CStat::MeanSquareError(std::vector<double>& data, double lowerLimit, double upperLimit)
{
	int cnt = 0;
	double sum = 0.0;

	std::for_each(std::begin(data), std::end(data), [&](const double d) {
		if ((d != -DBL_MAX) && (d != DBL_MAX))
		{
			if ((d >= lowerLimit) && (d <= upperLimit))
			{
				sum += d*d;
				cnt++;
			}
		}
		});

	return sum / cnt;
}


double CStat::Variance(std::vector<double>& data, double mean, double lowerLimit, double upperLimit, CStat::DataType dtype)
{
	double accum = 0.0;
	size_t cnt = 0;

	std::for_each(std::begin(data), std::end(data), [&](const double d) {
		if ((d != -DBL_MAX) && (d != DBL_MAX))
		{
			if ((d >= lowerLimit) && (d <= upperLimit))
			{
				accum += (d - mean) * (d - mean);
				cnt++;
			}
		}
		});

	if (dtype == CStat::DataType::Sample) cnt--;

	return accum / cnt;
}

double CStat::standardDeviation(std::vector<double>& data, double mean, double lowerLimit, double upperLimit, CStat::DataType dtype)
{
	return sqrt( CStat::Variance( data, mean, lowerLimit, upperLimit, dtype) );
}

double CStat::getNormalDistributionValue(double x, double mean, double stdev)
{
	double invsqrt2pi = 0.398942280401433;

	return invsqrt2pi / stdev * exp(-0.5 * ((x - mean) / stdev) * ((x - mean) / stdev));
	//return (sigma <= 0) ? 1 / 0 : invsqrt2pi / sigma * exp( -0.5 * ((x - mu) / sigma) * ((x - mu) / sigma) );
	//return (1 / (sd*sqrt(2 * PI)))*exp(-(x - mu)*(x - mu) / (2 * sd*sd));
}

std::vector<double> CStat::genData(CStat::DistType type, size_t size, double val1, double val2)
{
	switch (type)
	{
	case Normal:
	default:
		// val1 -> mean, val2 -> stdev
		return genDataNormal(size, val1, val2);
		break;
	case Random:
		// val1 -> dmin, val2 -> dmax
		return genDataRandom(size, val1, val2);
		break;
	}
}

std::vector<double> CStat::genDataNormal(size_t size, double mean, double stdev)
{
	std::normal_distribution<double> d{ mean, stdev };
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::vector<double> dane(size);

	for (int i = 0; i < size; i++)
	{
		dane[i] = d(gen);
	}

	return dane;
}

std::vector<double> CStat::genDataRandom(size_t size, double dmin, double dmax)
{
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::uniform_real_distribution<double> dist(dmin, dmax);
	std::vector<double> dane(size);

	for (int i = 0; i < size; i++)
	{
		dane[i] = dist(gen);
	}

	return dane;
}

double CStat::quantile(std::vector<double>& data, double q)
{
	double floorIdx;
	double fracIdx = modf(q * data.size(), &floorIdx);

	size_t n14 = floorIdx;
	double kwantyl;

	if (fracIdx != 0.0) //trafienie w indeks elementu
	{
		std::nth_element(data.begin(), data.begin() + n14, data.end());
		kwantyl = data[n14];
	}
	else //trafienie pomiêdzy dwa elementy
	{
		std::nth_element(data.begin(), data.begin() + n14, data.end());
		kwantyl = data[n14];

		std::nth_element(data.begin(), data.begin() + (n14 + 1), data.end());
		kwantyl += data[n14 + 1];

		kwantyl /= 2;
	}

	return kwantyl;
}

double CStat::quantile2(std::vector<double>& data, double q)
{
	double floorIdx;
	double fracIdx = modf(q * data.size(), &floorIdx);

	size_t n14 = floorIdx;
	double kwantyl;

	if (fracIdx != 0.0) //trafienie w indeks elementu
	{
		std::nth_element(data.begin(), data.begin() + n14, data.end());
		kwantyl = data[n14];
	}
	else //trafienie pomiêdzy dwa elementy
	{
		double d1 = 1.0 - fracIdx;
		double d2 = fracIdx;

		std::nth_element(data.begin(), data.begin() + n14, data.end());
		kwantyl = d1 * data[n14];

		std::nth_element(data.begin(), data.begin() + (n14 + 1), data.end());
		kwantyl += d2 * data[n14 + 1];

		//kwantyl /= 2;
	}

	return kwantyl;
}

void mMultiply(double* A, double* B, double* C, int m)
{
	int i, j, k;
	double s;
	
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
		{
			s = 0.;
			for (k = 0; k < m; k++)
				s += A[i * m + k] * B[k * m + j];
			C[i * m + j] = s;
		}
}

void mPower(double* A, int eA, double* V, int* eV, int m, int n)
{
	double* B; int eB, i;
	if (n == 1)
	{
		for (i = 0; i < m * m; i++)
			V[i] = A[i]; *eV = eA;
		return;
	}
	
	mPower(A, eA, V, eV, m, n / 2);
	
	B = (double*)malloc(((size_t)m * m) * sizeof(double));
	
	mMultiply(V, V, B, m);
	eB = 2 * (*eV);
	
	if (n % 2 == 0)
	{
		for (i = 0; i < m * m; i++)
			V[i] = B[i];
		*eV = eB;
	}
	else
	{
		mMultiply(A, B, V, m);
		*eV = eA + eB;
	}
	
	if (V[(m / 2) * m + (m / 2)] > 1e140)
	{
		for (i = 0; i < m * m; i++)
			V[i] = V[i] * 1e-140;
		*eV += 140;
	}
	free(B);
}

double K(int n, double d)
{
	int k, m, i, j, g, eH, eQ;
	double h, s, * H, * Q;
	// OMIT NEXT LINE IF YOU REQUIRE >7 DIGIT ACCURACY IN THE RIGHT TAIL
	s = d * d * n;
	if (s > 7.24 || (s > 3.76 && n > 99))
		return 1 - 2 * exp(-(2.000071 + .331 / sqrt((double)n) + 1.409 / n) * s);
	k = (int)(n * d) + 1;
	m = 2 * k - 1;
	h = k - n * d;
	H = (double*)malloc(((size_t)m * m) * sizeof(double));
	Q = (double*)malloc(((size_t)m * m) * sizeof(double));
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			if (i - j + 1 < 0)
				H[i * m + j] = 0;
			else
				H[i * m + j] = 1;
	for (i = 0; i < m; i++) {
		H[i * m] -= pow(h, i + 1);
		H[(m - 1) * m + i] -= pow(h, (m - i));
	}
	H[(m - 1) * m] += (2 * h - 1 > 0 ? pow(2 * h - 1, m) : 0);
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			if (i - j + 1 > 0)
				for (g = 1; g <= i - j + 1; g++)
					H[i * m + j] /= g;
	eH = 0;
	mPower(H, eH, Q, &eQ, m, n);
	s = Q[(k - 1) * m + k - 1];
	for (i = 1; i <= n; i++) {
		s = s * i / n;
		if (s < 1e-140) {
			s *= 1e140;
			eQ -= 140;
		}
	}
	s *= pow(10., eQ);
	free(H);
	free(Q);
	return s;
}

double CStat::KolmogorovSmirnovTest(std::vector<double>& data)
{
	std::vector<double> num = data;

	size_t n = num.size();
	double N = (double)n;

	std::sort(num.begin(), num.end());

	std::vector<double> dataP(n);
	std::vector<double> dataN(n);

	for (int i = 0; i < n; i++)
	{
		dataP[i] = ((double)(i + 1) / N) - num[i];

		dataN[i] = num[i] - ((double)i / N);
	}

	double dplus = *std::max_element(dataP.begin(), dataP.end());
	double dminus = *std::max_element(dataN.begin(), dataN.end());

	double dvalue = (dplus > dminus) ? dplus : dminus;

	return dvalue;

	//double dalpha = 0.565;  // for alpha = 0.05

	//return (dalpha > dvalue);
}