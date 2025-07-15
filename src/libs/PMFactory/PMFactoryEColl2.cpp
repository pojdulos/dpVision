//#include <cmath>
//#include <cstdio>
//#include <cstdlib>
#include "PMFactory.h"

void PMFactory::initial_quadrics()
{
	for (int i = 1; i <= static_cast<int>(vertices.size()); i++)
	{
		quadrics.insert(Matrices::value_type(i, Matrix(0.0)));
	}

	/* compute initial quadric */
	for (CPMFfaces::iterator fit=faces.begin(); fit!=faces.end(); fit++)
	{
		Plane p;
		CalcPlane( p, (*fit).second );
		Matrix m = Matrix( p );

		quadrics[ fit->second.A() ] += m;
		quadrics[ fit->second.B() ] += m;
		quadrics[ fit->second.C() ] += m;
	}
}

void PMFactory::CalcPlane( Plane &p, const CFace &f )
{
	float x0, y0, z0;        /* ax + by + cz = 0 */
	float x1, y1, z1;
	float x2, y2, z2;
	error_type a, b, c, M;

	x0 = vertices[f.A()].X();
	y0 = vertices[f.A()].Y();
	z0 = vertices[f.A()].Z();
	x1 = vertices[f.B()].X();
	y1 = vertices[f.B()].Y();
	z1 = vertices[f.B()].Z();
	x2 = vertices[f.C()].X();
	y2 = vertices[f.C()].Y();
	z2 = vertices[f.C()].Z();
	a = (y1-y0)*(z2-z0) - (z1-z0)*(y2-y0);   /* a1*b2 - a2*b1;        */
	b = (z1-z0)*(x2-x0) - (x1-x0)*(z2-z0);   /* a2*b0 - a0*b2;        */
	c = (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);   /* a0*b1 - a1*b0;        */
	M = sqrt(a*a + b*b + c*c);
	a = a/M;
	b = b/M;
	c = c/M;
	
	p[0] = a;
	p[1] = b;
	p[2] = c;
	p[3] = -1*(a*x0 + b*y0 + c*z0);				/* -1*(a*x + b*y + c*z); */
}

void PMFactory::select_pair()
{
	const double t = 0.12;
	int max_vid;
	int min_vid;
	int i, j;
	bool take_virtual_pair_into_consideration = false;
	
	/* (v1, v2) is an edge */
	/* id_v1 < id_v2*/
	size_t cnter = 0;

	UI::PROGRESSBAR::init( 0, faces.size(), 0 );
	int progress = 0;
	for (CPMFfaces::iterator fit=faces.begin(); fit!=faces.end(); fit++)
	{
		StatusBarManager::printfTimed( 500, QString("Quadrics::select_pair(): f=%1 ").arg(cnter++));
		UI::PROGRESSBAR::setValue( progress++ );

		// sciana ma 3 krawedzie - dla kazdej krawedzi sprawdzam czy juz jest w bazie bledow
		// jesli nie, to wyliczam blad dla tej krawedzi i dopisuje do errors

		min_vid = std::min(fit->second.A(), fit->second.B());
		max_vid = std::max(fit->second.A(), fit->second.B());
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			errors.insert(CPMFerrors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}

		min_vid = std::min(fit->second.A(), fit->second.C());
		max_vid = std::max(fit->second.A(), fit->second.C());
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			errors.insert(CPMFerrors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}

		min_vid = std::min(fit->second.B(), fit->second.C());
		max_vid = std::max(fit->second.B(), fit->second.C());
		if ( errors.find(Pair(min_vid, max_vid)) == errors.end() )
		{
			errors.insert(CPMFerrors::value_type(Pair(min_vid, max_vid), calculate_error(min_vid, max_vid)));
		}
	}
	UI::PROGRESSBAR::hide();

	// to dotyczy sytuacji gdy pozwalamy na redukowanie niepolaczonych wierzcholkow
	// pod warunkiem, ze spelniaja zaleznosc |v1 - v2| < t
	// w obecnej chwili nie korzystam z tego: warunek zawsze = false	
	if (take_virtual_pair_into_consideration)
	{
		for (i = 1; i < static_cast<int>(vertices.size()); i++)
		{
			for (j = i + 1; j < static_cast<int>(vertices.size()); j++)
			{
				if (distance(vertices[i], vertices[j]) < t)
				{
					// (i,j) is an edge 
					errors.insert(CPMFerrors::value_type(Pair(i, j), calculate_error(i, j)));
				}
			}
		}
	}
}





error_type PMFactory::calculate_error(int id_v1, int id_v2, double* vx, double* vy, double* vz)
{
	error_type min_error;
	Matrix q_bar;
	Matrix q_delta;
	bool isReturnVertex = true;
	if (vx == NULL) { vx = new double; isReturnVertex = false; }
	if (vy == NULL) { vy = new double; }
	if (vz == NULL) { vz = new double; }
	

	/* computer quadric of virtual vertex vf */
	q_bar = quadrics[id_v1] + quadrics[id_v2];
	
	/* test if q_bar is symmetric */
	if (q_bar[1] != q_bar[4] || q_bar[2] != q_bar[8] || q_bar[6] != q_bar[9] || 
		q_bar[3] != q_bar[12] || q_bar[7] != q_bar[13] || q_bar[11] != q_bar[14])
	{
		StatusBarManager::setText(QString("ERROR: Matrix q_bar is not symmetric! id_v1 = %1, id_v2 = %2").arg(id_v1).arg(id_v2));
		system("PAUSE");
		exit(-3);
	}

	q_delta = Matrix( q_bar[0], q_bar[1],  q_bar[2],  q_bar[3],
					  q_bar[4], q_bar[5],  q_bar[6],  q_bar[7], 
					  q_bar[8], q_bar[9], q_bar[10], q_bar[11], 
				             0,        0,	      0,        1);

	/*
	Tutaj sprawdziæ czy v1 lub v2 jest na liscie niezmiennych.
	- Jesli tak to nowy wierzcho³ek ma przyj¹æ jego wspó³rzêdne.
	- Jeœli oba sa na tej liœcie, to co? 
		- przyjmuje arbitralnie jeden z nich, ale nadaje tej krawedzi maksymaln¹ wartosc b³edu
	*/

	
	if ( isSolid( vertices[id_v1] ) )
	{
		*vx = vertices[id_v1].X();
		*vy = vertices[id_v1].Y();
		*vz = vertices[id_v1].Z();

		if ( isSolid( vertices[id_v2] ) )
		{
			if (isReturnVertex == false) { delete vx; delete vy; delete vz; }
			return FLT_MAX;
		}
	}
	else if ( isSolid( vertices[id_v2] ) )
	{
		*vx = vertices[id_v2].X();
		*vy = vertices[id_v2].Y();
		*vz = vertices[id_v2].Z();
	}
	else if ( double det = q_delta.det(0, 1, 2, 4, 5, 6, 8, 9, 10) )	/* if q_delta is invertible */
																		/* note that det(q_delta) equals to M44 */
	{
		*vx = -1/det*(q_delta.det(1, 2, 3, 5, 6, 7, 9, 10, 11));	/* vx = A41/det(q_delta) */
		*vy =  1/det*(q_delta.det(0, 2, 3, 4, 6, 7, 8, 10, 11));	/* vy = A42/det(q_delta) */
		*vz = -1/det*(q_delta.det(0, 1, 3, 4, 5, 7, 8, 9, 11));		/* vz = A43/det(q_delta) */	
	}

	/*
	 * if q_delta is NOT invertible, select 
	 * vertex from v1, v2, and (v1+v2)/2 
	 */
	else{
		double vx1 = vertices[id_v1].X();
		double vy1 = vertices[id_v1].Y();
		double vz1 = vertices[id_v1].Z();
		
		double vx2 = vertices[id_v2].X();
		double vy2 = vertices[id_v2].Y();
		double vz2 = vertices[id_v2].Z();
		
		double vx3 = double (vx1+vx2)/2;
		double vy3 = double (vy1+vy2)/2;
		double vz3 = double (vz1+vz2)/2;

		error_type error1 = vertex_error(q_bar, vx1, vy1, vz1);
		error_type error2 = vertex_error(q_bar, vx2, vy2, vz2);
		error_type error3 = vertex_error(q_bar, vx3, vy3, vz3);

		min_error = std::min(error1, std::min(error2, error3));
		
		if (error1 == min_error) { *vx = vx1; *vy = vy1, *vz = vz1; }
		if (error2 == min_error) { *vx = vx2; *vy = vy2, *vz = vz2; }
		if (error3 == min_error) { *vx = vx3; *vy = vy3, *vz = vz3; }
	}

	min_error = vertex_error(q_bar, *vx, *vy, *vz);
	
	if (isReturnVertex == false) { delete vx; delete vy; delete vz; }
	return min_error;
}

inline double PMFactory::distance(CVertex v1, CVertex v2)
{
	return sqrt( pow(v1.X()-v2.X(), 2) + pow(v1.Y()-v2.Y(), 2) + pow(v1.Z()-v2.Z(), 2) );
}

inline double PMFactory::vertex_error(Matrix q, double x, double y, double z)
{
 	return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[5]*y*y
 		+ 2*q[6]*y*z + 2*q[7]*y + q[10]*z*z + 2*q[11]*z + q[15];
}
