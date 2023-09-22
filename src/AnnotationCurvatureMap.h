#pragma once
#include "Annotation.h"
#include <map>
#include <set>

class __declspec(dllexport) CCurvature {
public:
	CVector3d vnorm; // wektor definiuj�cy p�aszczyzn� styczn� do wierzcho�ka
	double tensor[4]; // tensor krzywizny
	double Kmax; // krzywizna maksymalna
	double Kmin; // krzywizna minimalna
	double Kmean; // krzywizna srednia
	double KG; // krzywizna Gaussa

	// domy�lny konstruktor, je�li nie podano parametr�w
	CCurvature() {
		vnorm.Set(0.0, 1.0, 0.0);
		tensor[0] = 0;
		tensor[1] = 0;
		tensor[2] = 0;
		tensor[3] = 0;
		Kmax = 0;
		Kmin = 0;
		Kmean = 0;
		KG = 0;
	}

	// konstruktor kopiuj�cy
	CCurvature(const CCurvature &src) {
		vnorm = src.vnorm;
		tensor[0] = src.tensor[0];
		tensor[1] = src.tensor[1];
		tensor[2] = src.tensor[2];
		tensor[3] = src.tensor[3];
		Kmax = src.Kmax;
		Kmin = src.Kmin;
		Kmean = src.Kmean;
		KG = src.KG;
	}
};

typedef std::map<INDEX_TYPE, CCurvature> CCurvatureMap;

class __declspec(dllexport) CAnnotationCurvatureMap : public CAnnotation, public CCurvatureMap
{
public:
	CAnnotationCurvatureMap(CBaseObject* parent = nullptr) :CAnnotation(parent),CCurvatureMap()
	{
		setLabel("curvature map");
	};

	std::set<INDEX_TYPE> dajSiodlo();

	~CAnnotationCurvatureMap() override { CCurvatureMap::clear(); };

	virtual int type() override { return CAnnotation::CURVATUREMAP; }

	virtual std::wstring getTypeWSTR() override { return L"curvature map"; };
	virtual std::wstring getInfoRow() override;
};
