#include "Utilities.h"
//#include "AP.h"
#include <cmath>

/*
std::string GetResourceString( unsigned int item )
{
	char strbuf[255];
	LoadString( AP::mainApp().hInstance, item, strbuf, 255 );
	return std::string( strbuf );
}
*/

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