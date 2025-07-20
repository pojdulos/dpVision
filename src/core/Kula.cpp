#include "Kula.h"
#include <cfloat>

#define MINDIST 0.00001

/*
* A, B, C - wierzcho�ki tr�jk�ta
*/
Kula* Kula::fromTriangle(CPoint3d A, CPoint3d B, CPoint3d C)
{
	/*
	* a, b, c - d�ugosci bok�w le��cych naprzeciw odpowiedniego wierzcho�ka
	*/
	double a = B.distanceTo(C);
	double b = C.distanceTo(A);
	double c = A.distanceTo(B);

	if ((a < MINDIST) || (b < MINDIST) || (c < MINDIST)) return nullptr;

	/*
	* k - kosinus k�ta mi�dzy wektorami CA i CB
	* s - sinus j/w
	*/
	double k = CVector3d(C, A).dotProduct(CVector3d(C, B)) / (b*a);
	double s = sqrt(1 - (k * k));

	if (s < MINDIST) return nullptr;


	/*
	* definiuje uk�ad wsp�rz�dnych PQ na p�aszczy�nie tr�jk�ta
	* z pocz�tkiem w punkcie C i osi� P wyznaczon� przez wektor CB
	*/
	double p_C = 0.0;
	double q_C = 0.0;

	double p_B = a;
	double q_B = 0.0;

	double p_A = b * k;
	double q_A = b * s;


	/*
	* �rodek okr�gu opisanego (z przekszta�ce� wzor�w na symetralne)
	*/
	double p_S = a / 2;
	double q_S = (b - (a * k)) / (2 * s);


	double n = q_S / q_A;
	double m = (p_S - (n * p_A)) / p_B;

	CPoint3d S = C + (B - C) * m + (A - C) * n;

//	double p = (a + b + c) / 2;
//	double P = sqrt(p * (p - a) * (p - b) * (p - c));

//	double R = (a * b * c) / (4 * P);

	/*
	* promie�
	*/
	double R = S.distanceTo(A);

	return new Kula(S, R);
}

//#include "../api/UI.h"
#include "StatusBarManager.h"

Kula* Kula::fromPoints(std::vector<CVertex> pts)
{
	CPoint3d A = pts[0];
	CPoint3d B = pts[1];
	CPoint3d C = pts[2];
	Kula* kula = Kula::fromTriangle(A, B, C);
	

	for (int i = 0; i < pts.size(); i++)
	{
		StatusBarManager::setTextTimed(500, QString::number(i));

		bool isA = false;
		bool isB = false;
		bool isC = false;

		CPoint3d D = pts[i];

		bool zmiana = false;

		if (! kula->contains(D)) // D jest poza kul�
		{
			Kula* kulaA = Kula::fromTriangle(D, B, C);
			Kula* kulaB = Kula::fromTriangle(A, D, C);
			Kula* kulaC = Kula::fromTriangle(A, B, D);

			if (kulaA->contains(*kula)) isA = true;
			else delete kulaA;

			if (kulaB->contains(*kula)) isB = true;
			else delete kulaB;

			if (kulaC->contains(*kula)) isC = true;
			else delete kulaC;

			double minR = DBL_MAX;

			if (isA && (minR > kulaA->radius()))
			{
				minR = kulaA->radius();

				delete kula;
				kula = kulaA;

				A = D;
			}
			else
			{
				delete kulaA;
			}

			if (isB && (minR > kulaB->radius()))
			{
				minR = kulaB->radius();

				delete kula;
				kula = kulaB;

				B = D;
			}
			else
			{
				delete kulaB;
			}

			if (isC && (minR > kulaC->radius()))
			{
				minR = kulaC->radius();

				delete kula;
				kula = kulaC;

				C = D;
			}
			else
			{
				delete kulaC;
			}
		}
	}
	
	return kula;
}

Kula* Kula::bound(Kula k1, Kula k2)
{
	if (k1.contains(k2)) return new Kula(k1);

	if (k2.contains(k1)) return new Kula(k2);

	double newR = (k1.radius() + k1.center().distanceTo(k2.center()) + k2.radius()) / 2.0;

	double cor = (k1.radius() - k2.radius()) / 2;
	CVector3d corV = CVector3d(k2.center(), k1.center()).getNormalized() * cor;

	CPoint3d newC = ( (k1.center() + k2.center()) / 2 ) + corV;

	return new Kula( newC, newR );
}

void Kula::bound2(Kula k1, Kula k2)
{
	if (k1.contains(k2))
	{
		m_c = k1.m_c;
		m_r = k1.m_r;
	}
	else if (k2.contains(k1))
	{
		m_c = k2.m_c;
		m_r = k2.m_r;
	}
	else
	{
		double newR = (k1.radius() + k1.center().distanceTo(k2.center()) + k2.radius()) / 2.0;

		//double cor = k1.radius() / k2.radius());
		//CVector3d corV = CVector3d(k2.center(), k1.center()).getNormalized() * cor;

		CVector3d displC(k1.center(), k2.center());
		CPoint3d newC = k1.center() + ( displC / 2 );// +corV;

		m_c = newC;
		m_r = newR;
	}
}

void Kula::scal(Kula k2)
{
	if (this->contains(k2)) return;

	if (k2.contains(*this))
	{
		m_c = k2.m_c;
		m_r = k2.m_r;

		return;
	}

	CVector3d centerDispl(m_c, k2.m_c);

	double newR = (m_r + k2.m_r + centerDispl.length()) / 2.0;

	double cor = ( k2.m_r - m_r ) / 2;
	
	CVector3d corV = centerDispl.getNormalized() * cor;

		
	CPoint3d newC = ((m_c + k2.m_c) / 2) + corV;

	m_c = newC;
	m_r = newR;
	
}


Kula* Kula::fromPoints3(std::vector<CVertex> &pts)
{
	std::pair<INDEX_TYPE, INDEX_TYPE> result;

	double radius = Kula::findMaxDist_BruteForce(pts, result) / 2;

	CPoint3d ctr = (CPoint3d(pts[result.first]) + CPoint3d(pts[result.second])) / 2;

	return new Kula( ctr, radius);
}

Kula* Kula::fromPoints2(std::vector<CVertex>& pts)
{
	Kula* kula = Kula::fromTriangle(pts[0], pts[1], pts[2]);

	if (pts.size() > 3)
	{
		for (int i = 1; i < 3; i++)
		{
			StatusBarManager::setTextTimed(500, QString::number(i));

			Kula* kula2 = Kula::fromTriangle(pts[i], pts[i + 1], pts[i + 2]);

			if (kula2 != nullptr)
			{
				if (kula2->isValid()) kula->scal(*kula2);

				delete kula2;
			}
		}
	}
	return kula;
}


double Kula::findMaxDist_BruteForce(std::vector<CVertex>& pts, std::pair<INDEX_TYPE, INDEX_TYPE>& result)
{
	double Max = 0.0;

	// Iterate over all possible pairs
	for (int i = 0; i < pts.size(); i++)
	{
		for (int j = i + 1; j < pts.size(); j++)
		{
			double dist = pts[i].squaredDistanceTo(pts[j]);

			if (dist > Max)
			{
				result.first = i;
				result.second = j;
				Max = dist;
			}
		}
	}

	return sqrt(Max);
}

void Kula::findBiggestTriangle(std::vector<CVertex>& pts, INDEX_TYPE result[3] )
{
	result[0] = 0;
	result[1] = 1;
	result[2] = 2;

	double Max = pts[0].squaredDistanceTo(pts[1]) + pts[1].squaredDistanceTo(pts[2]) + pts[0].squaredDistanceTo(pts[1]);

	//for (int i = 3; i < pts.size(); i++)
	//{
	//	for (int j = i + 1; j < pts.size(); j++)
	//	{
	//		double dist = pts[i].squaredDistanceTo(pts[j]);

	//		if (dist > Max)
	//		{
	//			result.first = i;
	//			result.second = j;
	//			Max = dist;
	//		}
	//	}
	//}

}



Kula* Kula::fromMesh(std::vector<CFace> &fcs, std::vector<CVertex> &pts)
{
	Kula* kula = Kula::fromTriangle(pts[fcs[0].A()], pts[fcs[0].B()], pts[fcs[0].C()]);

	for (int i = 1; i < fcs.size(); i++)
	{
		StatusBarManager::setTextTimed(500, QString::number(i));

		Kula* kula2 = Kula::fromTriangle(pts[fcs[i].A()], pts[fcs[i].B()], pts[fcs[i].C()]);

		if (kula2->isValid()) kula->scal(*kula2);

		delete kula2;
	}

	return kula;
}


//bool helper(CPoint3d p1, CPoint3d p2, CPoint3d p3 )
//{
//	// triangle "edges"
//	const CVector3d t(p1, p2);
//	const CVector3d u(p1, p3);
//	const CVector3d v(p2, p3);
//
//	// triangle normal
//	CVector3d w = t.crossProduct(u);
//	double wsl = w.squaredLength();
//	
//	if (wsl < 10e-14) return false; // area of the triangle is too small (you may additionally check the points for colinearity if you are paranoid)
//
//	// helpers
//	const double iwsl2 = 1.0 / (2.0 * wsl);
//	const double tt = t.dotProduct(t);  //kwadrat modu�u ???
//	const double uu = u.dotProduct(u);
//
//	// result circle
//	CVector3d circCenter = CVector3d(CPoint3d(0.0,0.0,0.0),p1) + (u * (tt * u.dotProduct(v)) - t * (uu * t.dotProduct(v))) * iwsl2;
//	double   circRadius = sqrt(tt * uu * v.dotProduct(v) * iwsl2 * 0.5);
//	CVector3d circAxis = w / sqrt(wsl);
//
//}
//
