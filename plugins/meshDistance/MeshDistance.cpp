#define _HAS_TRADITIONAL_STL

#include "StdAfx.h"
#include "MeshDistance.h"

#include "BoundingBox.h"
#include "BoundVector.h"

#include <iterator>


void xyzVertexMap( CMesh* m2, xyzMapType &xyzMap )
{
	xyzMap.clear();
	for (int i = 0; i < m2->vertices().size(); i++)
	{
		CVertex *v = &m2->vertices()[i];

		CTriple<int> indeks(floor(v->X()), floor(v->Y()), floor(v->Z()));

		xyzMap[indeks].insert(i);
		xyzMap[indeks].insert(i);
		xyzMap[indeks].insert(i);
	}
}

void xyzFaceMap_Old(CMesh* m2, xyzMapType &xyzMap)
{
	xyzMap.clear();
	for (int i = 0; i < m2->faces().size(); i++)
	{
		CFace *f = &m2->faces()[i];

		CVertex *vA = &m2->vertices()[f->A()];
		CVertex *vB = &m2->vertices()[f->B()];
		CVertex *vC = &m2->vertices()[f->C()];

		CTriple<int> indeksA(floor(vA->X()), floor(vA->Y()), floor(vA->Z()));
		CTriple<int> indeksB(floor(vB->X()), floor(vB->Y()), floor(vB->Z()));
		CTriple<int> indeksC(floor(vC->X()), floor(vC->Y()), floor(vC->Z()));

		xyzMap[indeksA].insert(i);
		xyzMap[indeksB].insert(i);
		xyzMap[indeksC].insert(i);
	}
}

void xyzFaceMap(CMesh* m2, xyzMapType &xyzMap, double precision = 1.0 )
{
	xyzMap.clear();
	
	for (int i = 0; i < m2->faces().size(); i++)
	{
		UI::STATUSBAR::printfTimed(1000, L"%d...", m2->faces().size()-i);
		CFace *f = &m2->faces()[i];

		CVertex *vA = &m2->vertices()[f->A()];
		CVertex *vB = &m2->vertices()[f->B()];
		CVertex *vC = &m2->vertices()[f->C()];

		CPunkt3D p_min(*vA);
		CPunkt3D p_max(*vA);

		p_min.SetIfSmaller(*vB);
		p_max.SetIfBigger(*vB);

		p_min.SetIfSmaller(*vC);
		p_max.SetIfBigger(*vC);

		int d = 0;

		for (int z = floor( precision * p_min.Z() ) - d; z < ceil( precision * p_max.Z() ) + d; z++)
			for (int y = floor( precision * p_min.Y() ) - d; y < ceil( precision * p_max.Y() ) + d; y++)
				for (int x = floor( precision * p_min.X() ) - d; x < ceil( precision * p_max.X() ) + d; x++)
				{
					CTriple<int> indeks(x, y, z);
					xyzMap[indeks].insert(i);
				}
	}
}

void xyFaceMap(CMesh* m2, xyMapType &xyMap, double precision = 1.0)
{
	xyMap.clear();

	for (int i = 0; i < m2->faces().size(); i++)
	{
		UI::STATUSBAR::printfTimed(1000, L"%d...", m2->faces().size() - i);
		CFace *f = &m2->faces()[i];

		CVertex *vA = &m2->vertices()[f->A()];
		CVertex *vB = &m2->vertices()[f->B()];
		CVertex *vC = &m2->vertices()[f->C()];

		CPunkt3D p_min(*vA);
		CPunkt3D p_max(*vA);

		p_min.SetIfSmaller(*vB);
		p_max.SetIfBigger(*vB);

		p_min.SetIfSmaller(*vC);
		p_max.SetIfBigger(*vC);

		int d = 0;

		//for (int z = floor(precision * p_min.Z()) - d; z < ceil(precision * p_max.Z()) + d; z++)
			for (int y = floor(precision * p_min.Y()) - d; y < ceil(precision * p_max.Y()) + d; y++)
				for (int x = floor(precision * p_min.X()) - d; x < ceil(precision * p_max.X()) + d; x++)
				{
					std::pair<int,int> indeks(x, y);
					xyMap[indeks].insert(i);
				}
	}
}

std::vector<double> CMeshDistance::meshDistance_Vertex2Vertex(CModel3D* obj1, CModel3D* obj2)
{
	CMesh* m1 = (CMesh*) obj1->getChild();
	CMesh* m2 = (CMesh*) obj2->getChild();

	xyzMapType xyzMap;

	xyzVertexMap( m2, xyzMap );

	std::vector<double> odleglosci( m1->vertices().size() );

	//m1->calcVN();

	for ( int i = 0; i < m1->vertices().size(); i++ )
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);

		CPunkt3D p0 = m1->vertices()[i];

		odleglosci[i] = DBL_MAX;

		for ( int z = floor(p0.Z()) - 10; z < floor(p0.Z()) + 11; z++ )
			for (int y = floor(p0.Y()) - 10; y < floor(p0.Y()) + 11; y++)
				for (int x = floor(p0.X()) - 10; x < floor(p0.X()) + 11; x++)
				{
					CTriple<int> indeks( x, y, z );

					for (std::set<INDEX_TYPE>::iterator it = xyzMap[indeks].begin(); it != xyzMap[indeks].end(); it++)
					{
						CPunkt3D p1 = m2->vertices()[*it];
						//odleglosci[i] = min( odleglosci[i], CWektor3D( p0, p1 ).length());

						CVector3d v(p0, p1);
						//CVector3d ray = -m1->vnormals()[i];
						
						double o = v.length();

						//if (ray.dotProduct(v) < 0)
						//	o = -v.length();

						if (v.length() < abs(odleglosci[i]))
							odleglosci[i] = o;
					}
				}
	}

	return odleglosci;
}


std::vector<double> CMeshDistance::meshDistance_Vertex2VertexNEW(CModel3D* obj1, CModel3D* obj2)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CPointCloud* m2 = (CPointCloud*)obj2->getChild();

	std::vector<double> odleglosci(m1->vertices().size());

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);

		CPunkt3D p0 = m1->vertices()[i];

		odleglosci[i] = DBL_MAX;

		int idx = m2->getKDtree(CPointCloud::KDtree::PRESERVE).closest_to_pt(p0);

		if (idx != -1)
		{
			CVector3d v(p0, m2->vertices()[idx]);
			double o = v.length();

			if (v.length() < abs(odleglosci[i]))
				odleglosci[i] = o;
		}
	}

	return odleglosci;
}


#include <algorithm>

void intersection_test()
{
	std::vector<int> v1{ 1,2,3,4,5,6,7,8 };
	std::vector<int> v2{ 5,  7,  9,10 };
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());

	std::vector<int> v_intersection;

	std::set_intersection(v1.begin(), v1.end(),
		v2.begin(), v2.end(),
		std::back_inserter(v_intersection));
	for (int n : v_intersection)
		std::cout << n << ' ';
}

template<class T, class Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
	return assert(!comp(hi, lo)),
		comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	return clamp(v, lo, hi, std::less<>());
}

CPunkt3D closesPointOnTriangle(CPunkt3D pA, CPunkt3D pB, CPunkt3D pC, const CPunkt3D &sourcePosition)
{
	CWektor3D edge0 = pB - pA;
	CWektor3D edge1 = pC - pA;
	CWektor3D v0 = pA - sourcePosition;

	float a = edge0.dotProduct(edge0);
	float b = edge0.dotProduct(edge1);
	float c = edge1.dotProduct(edge1);
	float d = edge0.dotProduct(v0);
	float e = edge1.dotProduct(v0);

	float det = a*c - b*b;
	float s = b*e - c*d;
	float t = b*d - a*e;

	if (s + t < det)
	{
		if (s < 0.f)
		{
			if (t < 0.f)
			{
				if (d < 0.f)
				{
					s = clamp(-d / a, 0.f, 1.f);
					t = 0.f;
				}
				else
				{
					s = 0.f;
					t = clamp(-e / c, 0.f, 1.f);
				}
			}
			else
			{
				s = 0.f;
				t = clamp(-e / c, 0.f, 1.f);
			}
		}
		else if (t < 0.f)
		{
			s = clamp(-d / a, 0.f, 1.f);
			t = 0.f;
		}
		else
		{
			float invDet = 1.f / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		if (s < 0.f)
		{
			float tmp0 = b + d;
			float tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				float numer = tmp1 - tmp0;
				float denom = a - 2 * b + c;
				s = clamp(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				t = clamp(-e / c, 0.f, 1.f);
				s = 0.f;
			}
		}
		else if (t < 0.f)
		{
			if (a + d > b + e)
			{
				float numer = c + e - b - d;
				float denom = a - 2 * b + c;
				s = clamp(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				s = clamp(-e / c, 0.f, 1.f);
				t = 0.f;
			}
		}
		else
		{
			float numer = c + e - b - d;
			float denom = a - 2 * b + c;
			s = clamp(numer / denom, 0.f, 1.f);
			t = 1.f - s;
		}
	}

	return pA + edge0 * s + edge1 * t;
}



double CMeshDistance::searchInIndex(CPunkt3D p0, CWektor3D v0, CMesh* m2, xyzMapType &xyzMap, int x, int y, int z, bool useAbsValues)
{
	CTriple<int> indeks(x, y, z);

	double r = DBL_MAX;

	for (std::set<INDEX_TYPE>::iterator it = xyzMap[indeks].begin(); it != xyzMap[indeks].end(); it++)
	{
		INDEX_TYPE j = *it;
		CWektor3D vf = m2->faces()[j].getNormal(m2->vertices());

		CPunkt3D p1;

		CVertex *vA = &m2->vertices()[m2->faces()[j].A()];
		CVertex *vB = &m2->vertices()[m2->faces()[j].B()];
		CVertex *vC = &m2->vertices()[m2->faces()[j].C()];

		p1 = closesPointOnTriangle(*vA, *vB, *vC, p0);

		CVector3d v(p0, p1);

		if (abs(r) > v.length())
		{
			if (!useAbsValues)
			{
				CVector3d ray = -v0;
				if (ray.dotProduct(v) < 0)
					r = -v.length();
			}

			r = v.length();
		}
	}

	return r;
}

std::vector<double> CMeshDistance::meshDistance_Vertex2Face_New(CModel3D* obj1, CModel3D* obj2, bool useNormals, bool useAbsValues)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CMesh* m2 = (CMesh*)obj2->getChild();

	UI::STATUSBAR::printf(L"entering Vertex2Face New method...");

	xyzMapType xyzMap;
	//xyzFaceMap_Old( m2, xyzMap );
	xyzFaceMap(m2, xyzMap);

	std::vector<double> odleglosci(m1->vertices().size());

	m1->calcVN();

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);

		CPunkt3D p0 = m1->vertices()[i];
		CWektor3D v0 = m1->vnormals()[i];

		CPunkt3D iP;
		INDEX_TYPE fIdx;

		odleglosci[i] = DBL_MAX;

		int lowLIM = 5;
		int uprLIMx = 5;
		int uprLIMy = 5;
		int uprLIMz = 5;

		bool hit = false;
		int currDist = 0;

		for (int z = 0; !hit && (z < uprLIMz); z++)
		{
			for (int y = 0; !hit && (y < uprLIMy); y++)
			{
				for (int x = 0; !hit && (x < uprLIMx); x++)
				{
					double o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) - x, floor(p0.Y()) - y, floor(p0.Z()) - z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) + x, floor(p0.Y()) - y, floor(p0.Z()) - z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) - x, floor(p0.Y()) + y, floor(p0.Z()) - z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) + x, floor(p0.Y()) + y, floor(p0.Z()) - z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) - x, floor(p0.Y()) - y, floor(p0.Z()) + z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) + x, floor(p0.Y()) - y, floor(p0.Z()) + z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) - x, floor(p0.Y()) + y, floor(p0.Z()) + z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}

					o = searchInIndex(p0, v0, m2, xyzMap, floor(p0.X()) + x, floor(p0.Y()) + y, floor(p0.Z()) + z, useAbsValues);

					if (abs(o) < abs(odleglosci[i]))
					{
						odleglosci[i] = o;
						hit = true;
					}
				}
			}
		}

	}

	return odleglosci;
}


std::vector<double> CMeshDistance::meshDistance_Vertex2Face(CModel3D* obj1, CModel3D* obj2, bool useNormals, bool useAbsValues)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CMesh* m2 = (CMesh*)obj2->getChild();

	UI::STATUSBAR::printf(L"entering Vertex2Face method...");

	xyzMapType xyzMap;
	xyzFaceMap(m2, xyzMap);

	std::vector<double> odleglosci(m1->vertices().size());

	std::set<INDEX_TYPE> visitedFaces;

	m1->calcVN();

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);
		
		visitedFaces.clear();

		CPunkt3D p0 = m1->vertices()[i];
		CWektor3D v0 = m1->vnormals()[i];

		CPunkt3D iP;
		INDEX_TYPE fIdx;

		odleglosci[i] = DBL_MAX;

		int LIM = 3;

		for (int z = floor(p0.Z()) - LIM; z < floor(p0.Z()) + LIM + 1; z++)
			for (int y = floor(p0.Y()) - LIM; y < floor(p0.Y()) + LIM + 1; y++)
				for (int x = floor(p0.X()) - LIM; x < floor(p0.X()) + LIM + 1; x++)
				{
					CTriple<int> indeks(x, y, z);

					for (std::set<INDEX_TYPE>::iterator it = xyzMap[indeks].begin(); it != xyzMap[indeks].end(); it++)
					{
						if (visitedFaces.find(*it) == visitedFaces.end())
						{
							INDEX_TYPE j = *it;
							visitedFaces.insert(j);

							CWektor3D vf = m2->faces()[j].getNormal(m2->vertices());

							if (useNormals && (v0.dotProduct(vf) < 0)) {
								;
							}
							else {
								CPunkt3D p1;
								double r;

								CVertex *vA = &m2->vertices()[m2->faces()[j].A()];
								CVertex *vB = &m2->vertices()[m2->faces()[j].B()];
								CVertex *vC = &m2->vertices()[m2->faces()[j].C()];

								p1 = closesPointOnTriangle(*vA, *vB, *vC, p0);

								//odleglosci[i] = min(odleglosci[i], CWektor3D(p0, p1).length());

								CVector3d v(p0, p1);
								CVector3d ray = -m1->vnormals()[i];

								double o = v.length();

								if (!useAbsValues && (ray.dotProduct(v) < 0))
									o = -v.length();

								if (v.length() < abs(odleglosci[i]))
									odleglosci[i] = o;
							}
						}
					}
				}
	}

	xyzMap.clear();
	visitedFaces.clear();

	return odleglosci;
}



std::vector<double> CMeshDistance::meshDistance_Vertex2Face2D(CModel3D* obj1, CModel3D* obj2, bool useNormals, bool useAbsValues)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CMesh* m2 = (CMesh*)obj2->getChild();

	UI::STATUSBAR::printf(L"entering Vertex2Face method...");

	xyMapType xyMap;
	xyFaceMap(m2, xyMap);

	std::vector<double> odleglosci(m1->vertices().size());

	std::set<INDEX_TYPE> visitedFaces;

	m1->calcVN();

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);

		visitedFaces.clear();

		CPunkt3D p0 = m1->vertices()[i];
		CWektor3D v0 = m1->vnormals()[i];

		CPunkt3D iP;
		INDEX_TYPE fIdx;

		odleglosci[i] = DBL_MAX;

		int LIM = 3;

		//for (int z = floor(p0.Z()) - LIM; z < floor(p0.Z()) + LIM + 1; z++)
			for (int y = floor(p0.Y()) - LIM; y < floor(p0.Y()) + LIM + 1; y++)
				for (int x = floor(p0.X()) - LIM; x < floor(p0.X()) + LIM + 1; x++)
				{
					std::pair<int,int> indeks(x, y);

					for (std::set<INDEX_TYPE>::iterator it = xyMap[indeks].begin(); it != xyMap[indeks].end(); it++)
					{
						if (visitedFaces.find(*it) == visitedFaces.end())
						{
							INDEX_TYPE j = *it;
							visitedFaces.insert(j);

							CWektor3D vf = m2->faces()[j].getNormal(m2->vertices());

							if (useNormals && (v0.dotProduct(vf) < 0)) {
								;
							}
							else {
								CPunkt3D p1;
								double r;

								CVertex *vA = &m2->vertices()[m2->faces()[j].A()];
								CVertex *vB = &m2->vertices()[m2->faces()[j].B()];
								CVertex *vC = &m2->vertices()[m2->faces()[j].C()];

								p1 = closesPointOnTriangle(*vA, *vB, *vC, p0);

								//odleglosci[i] = min(odleglosci[i], CWektor3D(p0, p1).length());

								CVector3d v(p0, p1);
								CVector3d ray = -m1->vnormals()[i];

								double o = v.length();

								if (!useAbsValues && (ray.dotProduct(v) < 0))
									o = -v.length();

								if (v.length() < abs(odleglosci[i]))
									odleglosci[i] = o;
							}
						}
					}
				}
	}

	xyMap.clear();
	visitedFaces.clear();

	return odleglosci;
}


bool CMeshDistance::myClosestFace(CMesh *m, CPoint3d pkt0, CVector3d vRay, std::set<INDEX_TYPE> podzbior, CPoint3d & IntersectionPoint, INDEX_TYPE & indx, bool onlyFront, bool useNormals)
{
	INDEX_TYPE id = 0;
	double dist = DBL_MAX;
	CPoint3d tmpP;
	double r;
	bool foundAny = false;

	for (std::set<INDEX_TYPE>::iterator si = podzbior.begin(); si != podzbior.end(); si++)
	{
		INDEX_TYPE j = *si;

		//if (!myTriBoxOverlap(m->vertices()[m->faces()[j].A()], m->vertices()[m->faces()[j].B()], m->vertices()[m->faces()[j].C()], pkt0, 1.0)) continue;

		CVector3d vf( m->faces()[j].getNormal(m->vertices()) );
		CVector3d v0( - vRay );

		if (useNormals && ( v0.dotProduct(vf) < 0)) {
			;
		}
		else {
			int result = m->rayTriangleIntersect3D(j, vRay, pkt0, tmpP, r);

			// result == -2 ---> promieñ le¿y na trójk¹cie (ma wiecej punktów wspólnych ni¿ 1)
			// result == -1 ---> trójk¹t jest zdegenerowany (co najmniej jedna krawêdŸ ma zerow¹ d³ugoœæ)
			// result == 0 ---> promieñ nie trafi³ w trójk¹t
			// result == 1 ---> trójk¹t trafiony od przodu (punkt przeciêcia zwracany w tmpP)
			// result == 2 ---> trójk¹t trafiony od ty³u (punkt przeciêcia zwracany w tmpP)

			if (result > 0)
			{
				double tmp = CVector3d(tmpP, pkt0).length();

				if (tmp < dist)
				{
					dist = tmp;
					IntersectionPoint = tmpP;
					indx = j;

					if (result == 1)
					{
						foundAny = true;
					}
					else if (result == 2)
					{
						if (onlyFront)
							foundAny = false;				 // jeœli najbli¿ej jest œciana odwrócona ty³em, to nie powinno mi zwróciæ punktu
						else
							foundAny = true;				// chyba ¿e dok³¹dnie tego chcê
					}
				}
			}
		}
		/* UWAGA
		W przypadku gdy result == -2 mo¿emy mieæ do czynienia z sytuacjami:
		a) œciana jest ustawiona bokiem, ale ma s¹siadów wiêc promieñ powinien w któregoœ z nich trafiæ i temu s¹siadowi zostanie przypisane trafienie
		b) j.w., ale wszyscy trafieni s¹siedzi sa odwróceni "ty³em" to zn. ¿e promieñ przeœlizn¹³ sie po wewn. stronie siatki - mo¿na punkt pomin¹æ.
		c) œciana nie ma ¿adnego s¹siada - nieprawid³owa siatka ??? - mozna punkt pomin¹æ
		d) œciana jest na brzegu siatki - jedynie tu nale¿a³oby jakoœ wyznaczyæ punkt przeciêcia, ale to jest chyba przypadek marginalny
		*/

	}

	return foundAny;
}


bool CMeshDistance::calcDistance_VNF( CMesh *m2, CPoint3d p0, CVector3d ray, std::set<INDEX_TYPE> &scianki, double maxDistance, double &odleglosc, bool useAbsValues)
{
	CPoint3d iP;
	INDEX_TYPE fIdx;

	if (myClosestFace(m2, p0, ray, scianki, iP, fIdx, false, true))
	{
		CVector3d v(p0, iP);

		if (maxDistance >= v.length())
		{
			double o = v.length();

			if ( !useAbsValues && (ray.dotProduct(v) < 0) )
				o = -v.length();

			if ( v.length() < abs(odleglosc) )
				odleglosc = o;
		}
		return true;
	}
	return false;
}

typedef std::multimap<float, INDEX_TYPE> bbType;


void bbMapsCreate(CMesh* m2, std::vector<CBoundingBox> &bboxes, bbType &bbX, bbType &bbY, bbType &bbZ )
{
	bbX.clear();
	bbY.clear();
	bbZ.clear();

	bboxes.clear();

	for (int i = 0; i < m2->faces().size(); i++)
	{
		CFace f = m2->faces()[i];

		CVertex vA = m2->vertices()[f.A()];
		CVertex vB = m2->vertices()[f.B()];
		CVertex vC = m2->vertices()[f.C()];

		CBoundingBox bb(vA, vB, vC);

		bboxes.push_back(bb);
		//CPoint3f ctr = bb.ctr();

		bbX.insert(bbType::value_type( floor(10.0*bb.getMin().X()) / 10.0, i));
		bbY.insert(bbType::value_type( floor(10.0*bb.getMin().Y()) / 10.0, i));
		bbZ.insert(bbType::value_type( floor(10.0*bb.getMin().Z()) / 10.0, i));
	}
}


void set_int(std::set<INDEX_TYPE> in1, std::set<INDEX_TYPE> in2, std::set<INDEX_TYPE> &out)
{
	out.clear();

	for (std::set<INDEX_TYPE>::iterator it = in1.begin(); it != in1.end(); it++)
	{
		if (in2.find(*it) != in2.end())	out.insert(*it);
	}
}

void set_int(std::set<INDEX_TYPE> in1, std::set<INDEX_TYPE> in2, std::set<INDEX_TYPE> in3, std::set<INDEX_TYPE> &out)
{
	out.clear();

	for (std::set<INDEX_TYPE>::iterator it = in1.begin(); it != in1.end(); it++)
	{
		if ((in2.find(*it) != in2.end()) && (in3.find(*it) != in3.end()))	out.insert(*it);
	}
}


//auto select1st = [](std::pair< float, INDEX_TYPE > const& pair) {return pair.first; };
//auto select2nd = [](std::pair< float, INDEX_TYPE > const& pair) {return pair.second; };

//#define AUTO_RETURN(...) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}

//template <typename Pair> auto select1st() AUTO_RETURN(std::bind(&Pair::first, std::placeholders::_1))
//template <typename Pair> auto select2nd() AUTO_RETURN(std::bind(&Pair::second, std::placeholders::_1))

auto select1st = [](auto&& pair) {return std::forward<decltype(pair)>(pair).first; };
auto select2nd = [](auto&& pair) {return std::forward<decltype(pair)>(pair).second; };

std::vector<double> CMeshDistance::meshDistance_withNormal_test(CModel3D* obj1, CModel3D* obj2, double maxDistance, bool useNormals)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CMesh* m2 = (CMesh*)obj2->getChild();

	UI::STATUSBAR::printf(L"calculating xyzMap...");

	std::vector<CBoundingBox> bboxes;
	bbType bbX;
	bbType bbY;
	bbType bbZ;

	bbMapsCreate( m2, bboxes, bbX, bbY, bbZ );

	UI::STATUSBAR::printf(L"entering VertexNormalRay method...");

	std::vector<double> odleglosci(m1->vertices().size());

	m1->calcVN();

	float delta = 2.5f;

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);

		odleglosci[i] = DBL_MAX;

		CPunkt3D p0 = m1->vertices()[i];

		bbType::iterator xLow = bbX.lower_bound(p0.X() - delta);
		bbType::iterator xUpp = bbX.upper_bound(p0.X() + delta);

		if (xLow != xUpp)
		{
			std::set<INDEX_TYPE> xSet;
			xSet.clear();

			std::transform( xLow, xUpp, std::inserter( xSet, xSet.begin() ), select2nd );
			//copy( xSet.begin(), xSet.end(), ostream_iterator<int>(cout, ","));

			std::multimap<float, INDEX_TYPE>::iterator yLow = bbY.lower_bound(p0.Y() - delta);
			std::multimap<float, INDEX_TYPE>::iterator yUpp = bbY.upper_bound(p0.Y() + delta);

			if (yLow != yUpp)
			{
				std::set<INDEX_TYPE> ySet;
				ySet.clear();
				std::transform(yLow, yUpp, std::inserter(ySet, ySet.begin()), select2nd);

				std::multimap<float, INDEX_TYPE>::iterator zLow = bbZ.lower_bound(p0.Z() - delta);
				std::multimap<float, INDEX_TYPE>::iterator zUpp = bbZ.upper_bound(p0.Z() + delta);
				
				if (zLow != zUpp)
				{
					std::set<INDEX_TYPE> zSet;
					zSet.clear();
					std::transform(zLow, zUpp, std::inserter(zSet, zSet.begin()), select2nd);
					//for (std::multimap<float, INDEX_TYPE>::iterator itz = zLow; itz != zUpp; itz++)
					//{
					//	zSet.insert(itz->second);
					//}

					std::set<INDEX_TYPE> tmp;
					tmp.clear();
					set_int( xSet, ySet, zSet, tmp );

					CWektor3D ray = -m1->vnormals()[i];

					for ( int j = 0; j<tmp.size(); j++ )
					{
						if (bboxes[j].intersects( CBoundVector( (CPoint3d) p0, (CVector3d) ray) ))
						{
							CWektor3D vf = m2->faces()[j].getNormal(m2->vertices());
							CWektor3D v0 = -ray;

							if (useNormals && (v0.dotProduct(vf) < 0)) {
								;
							}
							else {
								CPunkt3D tmpP;
								double r;

								int result = m2->rayTriangleIntersect3D( j, ray, p0, tmpP, r );

								// result == -2 ---> promieñ le¿y na trójk¹cie (ma wiecej punktów wspólnych ni¿ 1)
								// result == -1 ---> trójk¹t jest zdegenerowany (co najmniej jedna krawêdŸ ma zerow¹ d³ugoœæ)
								// result == 0 ---> promieñ nie trafi³ w trójk¹t
								// result == 1 ---> trójk¹t trafiony od przodu (punkt przeciêcia zwracany w tmpP)
								// result == 2 ---> trójk¹t trafiony od ty³u (punkt przeciêcia zwracany w tmpP)

								if (result == 1) // jeœli celujemy te¿ "od ty³u" to wstawiæ > zamiast ==
								{
									if (maxDistance >= CWektor3D(p0, tmpP).length())
									{
										odleglosci[i] = std::min(odleglosci[i], CWektor3D(p0, tmpP).length());
									}
								}
							}
						}

					}
					
				}
			}

		}
	}

	
	return odleglosci;
}


std::vector<double> CMeshDistance::meshDistance_withNormal(CModel3D* obj1, CModel3D* obj2, double maxDistance, bool useNormals, bool useAbsValues)
{
	CMesh* m1 = (CMesh*)obj1->getChild();
	CMesh* m2 = (CMesh*)obj2->getChild();

	UI::STATUSBAR::printf(L"calculating xyzMap...");

	double precision = 1.0;

	xyzMapType xyzMap;
	xyzFaceMap( m2, xyzMap, precision );

	//std::set<INDEX_TYPE> scianki;
	//for (int i = 0; i < m2->faces().size(); i++)
	//{
	//	scianki.insert(i);
	//}



	UI::STATUSBAR::printf(L"entering VertexNormalRay method...");

	std::vector<double> odleglosci(m1->vertices().size());

	m1->calcVN();

	for (int i = 0; i < m1->vertices().size(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"measuring distance (%d)...", i);
		//UI::STATUSBAR::printf( L"measuring distance (%d)...", i );

		CPoint3d p0( m1->vertices()[i] );
		CVector3d ray( - m1->vnormals()[i] );

		odleglosci[i] = DBL_MAX;


		//bool res = calcDistance_VNF(m2, p0, ray, scianki, maxDistance, odleglosci[i]);
		


		for (int z = floor(p0.Z()) - 1; z < ceil(p0.Z()) + 2; z++)
			for (int y = floor(p0.Y()) - 1; y < ceil(p0.Y()) + 2; y++)
				for (int x = floor(p0.X()) - 1; x < ceil(p0.X()) + 2; x++)
				{
					CTriple<int> map_idx(x, y, z);

					if (xyzMap.find(map_idx) != xyzMap.end())
					{
						bool res = calcDistance_VNF(m2, p0, ray, xyzMap[map_idx], maxDistance, odleglosci[i], useAbsValues);
					}
					//else {
					//	; //UI::MESSAGEBOX::error(L"brak scianek");
					//}
				}

		
		//CTriple<int> map_idx(floor(precision*p0.X()), floor(precision*p0.Y()), floor(precision*p0.Z()));
		//if (xyzMap.find(map_idx) != xyzMap.end())
		//{
		//	bool res = calcDistance_VNF(m2, p0, ray, xyzMap[map_idx], maxDistance, odleglosci[i]);
		//}
		//else {
		//	; //UI::MESSAGEBOX::error(L"brak scianek");
		//}
	}

	return odleglosci;
}



//double triangleArea( CPoint3d n0, CPoint3d n1, CPoint3d n2 )
//{
//	CVector3d e1(n1, n0);
//	CVector3d e2(n2, n0);
//
//	CVector3d e3 = e1.crossProduct(e2);
//
//	double area = 0.5*e3.length();
//
//	return area;
//}



double CMeshDistance::calcGlobalError2(CModel3D* obj, std::vector<double> &odleglosci, double &wariancja, double lowerLimit, double upperLimit)
{
	double error = 0.0;
	double sumArea = 0.0;
	CMesh* m = (CMesh*)obj->getChild();

	//std::map<INDEX_TYPE, std::set<INDEX_TYPE>> v2fMap;
	std::map<INDEX_TYPE, double> faceArea;

	std::vector<double> vertexWeight( odleglosci.size(), 0.0 );

	double max_err = DBL_MAX;

	for (int i = 0; i < m->faces().size(); i++)
	{
		CFace *f = &m->faces()[i];

		faceArea[i] = CFace::triangleArea(m->vertices()[f->A()], m->vertices()[f->B()], m->vertices()[f->C()]);

		if (odleglosci[f->A()] != DBL_MAX) {
			sumArea += faceArea[i];
			vertexWeight[f->A()] += faceArea[i];
			//v2fMap[f->A()].insert(i);
		}

		if (odleglosci[f->B()] != DBL_MAX) {
			sumArea += faceArea[i];
			vertexWeight[f->B()] += faceArea[i];
			//v2fMap[f->B()].insert(i);
		}

		if (odleglosci[f->C()] != DBL_MAX) {
			sumArea += faceArea[i];
			vertexWeight[f->C()] += faceArea[i];
			//v2fMap[f->C()].insert(i);
		}
	}

	double avgVertexWeight = 0.0;
	int licznik = 0;
	for (int iv = 0; iv < vertexWeight.size(); iv++)
	{
		if ( odleglosci[iv] != DBL_MAX )
		{
			avgVertexWeight += vertexWeight[iv];
			licznik++;
		}
	}

	avgVertexWeight /= licznik;

	for (int iv = 0; iv < odleglosci.size(); iv++)
	{
		if (odleglosci[iv] != DBL_MAX)
		{
			odleglosci[iv] = (odleglosci[iv] * vertexWeight[iv]) / avgVertexWeight;
			error += odleglosci[iv];
		}
	}

	error /= licznik;

	wariancja = 0.0;
	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		if (odleglosci[iv] != DBL_MAX)
		{
			wariancja += pow(odleglosci[iv] - error, 2);
		}
	}

	wariancja /= licznik;

	return error;
}


double CMeshDistance::calcGlobalError(CModel3D* obj, std::vector<double> odleglosci, double &wariancja, double lowerLimit, double upperLimit)
{
	double error = 0.0;
	double sumArea = 0.0;
	CMesh* m = (CMesh*)obj->getChild();

	//std::map<INDEX_TYPE, std::set<INDEX_TYPE>> v2fMap;
	std::map<INDEX_TYPE, double> faceArea;

	std::map<INDEX_TYPE, double> vertexWeight;
	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		vertexWeight[iv] = 0.0;
	}

	for (int i = 0; i < m->faces().size(); i++)
	{
		CFace *f = &m->faces()[i];

		faceArea[i] = CFace::triangleArea( m->vertices()[f->A()], m->vertices()[f->B()], m->vertices()[f->C()] );

		//if (odleglosci[f->A()] != DBL_MAX) {
		if ( (odleglosci[f->A()] >= lowerLimit) && (odleglosci[f->A()] <= upperLimit) ) {
			sumArea += faceArea[i];
			vertexWeight[f->A()] += faceArea[i];
			//v2fMap[f->A()].insert(i);
		}

		//if (odleglosci[f->B()] != DBL_MAX) {
		if ((odleglosci[f->B()] >= lowerLimit) && (odleglosci[f->B()] <= upperLimit)) {
			sumArea += faceArea[i];
			vertexWeight[f->B()] += faceArea[i];
			//v2fMap[f->B()].insert(i);
		}

		//if (odleglosci[f->C()] != DBL_MAX) {
		if ((odleglosci[f->C()] >= lowerLimit) && (odleglosci[f->C()] <= upperLimit)) {
			sumArea += faceArea[i];
			vertexWeight[f->C()] += faceArea[i];
			//v2fMap[f->C()].insert(i);
		}
	}

	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		//if (odleglosci[iv] != DBL_MAX) {
		if ((odleglosci[iv] >= lowerLimit) && (odleglosci[iv] <= upperLimit)) {
			double e = (odleglosci[iv] * vertexWeight[iv]) / sumArea;
			error += e;
		}
	}

	int licznik = 0;
	wariancja = 0.0;
	for (int iv = 0; iv < m->vertices().size(); iv++)
	{
		//if (odleglosci[iv] != DBL_MAX) {
		if ((odleglosci[iv] >= lowerLimit) && (odleglosci[iv] <= upperLimit)) {
			wariancja += pow(odleglosci[iv] - error, 2);
			licznik++;
		}
	}
	wariancja /= licznik;

	return error;
}


