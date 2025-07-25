#include "Utilities.h"

#include <cmath>

double smartRound(double d, int decimal)
{
	double dec = 1.0;
	for (int i = 0; i < decimal; i++) { dec *= 10.0; }

	double czescCalkowita;
	double czescUlamkowa = modf(d, &czescCalkowita);

	czescUlamkowa *= dec;
	czescUlamkowa = round(czescUlamkowa) / dec;

	return czescCalkowita + czescUlamkowa;
}
