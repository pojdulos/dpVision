#pragma once
#ifndef _PLANE_H_BY_DP_INCLUDED_
#define _PLANE_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include "Triple.h"
#include "Matrix3x3.h"

#include "BoundVector.h"
#include <set>
#include "Transform.h"
#include "Vertex.h"


class CMesh;

// P�aszczyzna: Ax + By + Cz + D = 0


class DPVISION_EXPORT CPlane {
public:
	CPoint3d m_center;
	CVector3d m_normal;
	double m_d;

	// domyślny konstruktor, je�li nie podano parametrów, płaszczyzna: y = 0
	CPlane()
	{
		m_center.Set(0.0, 0.0, 0.0);
		m_normal.Set(0.0, 1.0, 0.0);
		m_d = recalc_D();
	}

	// konstruktor kopiuj�cy
	CPlane(const CPlane& p)
	{
		m_center = p.m_center;
		m_normal = p.m_normal;

		m_normal.normalize();
		m_d = recalc_D();
	}

	CPlane(const CPoint3d& c, const CVector3d& n)
	{
		m_center = c;
		m_normal = n;
		m_normal.normalize();
		m_d = recalc_D();
	}

	/*
	CPlane( double pA, double pB, double pC, double pD )
	{
		vn.Set( pA, pB, pC );
		d = pD;

		normalize();
	}

	CPlane( const CTriple<double> &p, const CVector3<double> &normal )
	{
		vn = normal;
		vn.normalize();
		d = - vn.dotProduct( (CVector3<double>)p );
	}

	//CPlane( const CPoint3f &Pt, const CVector3<double> &V1, const CVector3<double> &V2)
	//{
	//	CPlane( Pt, V1.crossProduct(V2) );
	//}

	CPlane(CTriple<double> p0, CTriple<double> p1, CTriple<double> p2)
	{
		CPlane( p0, CVector3<double>(p0, p1).crossProduct(CVector3<double>(p0, p2)));
	}
	*/

	//// konstruktor - p�aszczyzna przez 3 punkty
	//CPlane( CPoint3d p0, CPoint3d p1, CPoint3d p2 )
	//{
	//	CMatrix3x3<double> Mx( p0.Y(), p0.Z(),    1.0, p1.Y(), p1.Z(),    1.0, p2.Y(), p2.Z(),    1.0 );
	//	CMatrix3x3<double> My( p0.X(), p0.Z(),    1.0, p1.X(), p1.Z(),    1.0, p2.X(), p2.Z(),    1.0 );
	//	CMatrix3x3<double> Mz( p0.X(), p0.Y(),    1.0, p1.X(), p1.Y(),    1.0, p2.X(), p2.Y(),    1.0 );
	//	//CMatrix3x3<double> M1( p0.X(), p0.Y(), p0.Z(), p1.X(), p1.Y(), p1.Z(), p2.X(), p2.Y(), p2.Z() );

	//	// wyznaczniki
	//	double a = Mx.det();
	//	double b = -My.det();
	//	double c = Mz.det();
	//	//double d = - M1.det();

	//	m_center = p0;
	//	m_normal.Set(a, b, c);
	//	m_normal.normalize();
	//}

	// konstruktor - p�aszczyzna przez 3 punkty
	CPlane(CPoint3d p0, CPoint3d p1, CPoint3d p2)
	{
		CVector3d v01(p0, p1);
		CVector3d v02(p0, p2);

		m_normal = v01.crossProduct(v02).getNormalized();
		m_center = (p0+p1+p2)/3;
		m_d = recalc_D();
	}

	//CPoint3d &center()
	//{
	//	return m_center;
	//}

	CPoint3d getCenter()
	{
		return m_center;
	}

	void setCenter(CPoint3d c)
	{
		m_center = c;

		m_d = recalc_D();
	}

	//CVector3d &normal()
	//{
	//	return m_normal;
	//}

	CVector3d getNormal()
	{
		return m_normal;
	}

	void setNormal(CVector3d n)
	{
		m_normal = n;
		m_normal.normalize();

		m_d = recalc_D();
	}

	void invertNormal()
	{
		m_normal.invert();

		m_d = recalc_D();
	}

	double a() { return m_normal.X(); };
	double b() { return m_normal.Y(); };
	double c() { return m_normal.Z(); };
	double d() { return m_d; };

	double recalc_D()
	{
		// P�aszczyzna: Ax + By + Cz + D = 0
		// D = -( A*x + B*y + C*z )
		return -(m_normal.X() * m_center.X() + m_normal.Y() * m_center.Y() + m_normal.Z() * m_center.Z());
	}

	// zwraca wsp�rz�dne obrazu punktu p w rzucie prostopad�ym na p�aszczyzn�
	CPoint3d projectionOfPoint(CPoint3d p)
	{
		double t = -(m_normal.X() * p.X() + m_normal.Y() * p.Y() + m_normal.Z() * p.Z() + m_d) / m_normal.squaredLength();

		CPoint3d ret = p + m_normal * t;

		return ret;
	}

	bool intersectionWithPlane(CPlane& p2, CPoint3d& r_point, CVector3d& r_normal)
	{
		CVector3d p3_normal = this->m_normal.crossProduct(p2.m_normal);
		double det = p3_normal.squaredLength();

		if (det != 0.0) {
			r_point = (((p3_normal.crossProduct(p2.m_normal) * m_d) + (this->m_normal.crossProduct(p3_normal) * p2.m_d)) / det);
			r_normal = p3_normal;
			return true;
		}
		else { // p�aszczyzny s� r�wnoleg�e
			return false;
		}
	}

	//bool rayIntersection(CBoundVector ray, CPoint3d& p)
	//{
	//	float denom = m_normal.dotProduct(ray.direction());
	//	if (abs(denom) > 0.0001f) // your favorite epsilon
	//	{
	//		float t = (m_center - ray.origin()).dotProduct(m_normal) / denom;
	//		if (t >= 0) return true; // you might want to allow an epsilon here too
	//	}
	//	return false;
	//}

	inline double pointDistance(CPoint3d P)
	{
		return m_normal.dotProduct( CVector3d(CPoint3d(),P) ) + m_d;
	}

	bool rayIntersection(CPoint3d P1, CVector3d v, double maxDist, CPoint3d& outP)
	{
		CPoint3d P2 = P1 + v * maxDist;

		return lineSegmentIntersection(P1, P2, outP);
	}

	bool rayIntersect3D(CVector3d vRay, CPoint3d pP0, CPoint3d& pIntersectionPoint, double& pDistance)
	{
		if (0 == m_normal.length()) return false;

		// wektor od wierzcho�ka tr�jk�ta do punktu na promieniu
		CVector3d w0 = CVector3d(m_center, pP0);

		// iloczyn skalarny -> zero je�li wektory prostopad�e
		double a = -m_normal.dotProduct(w0); // 0 -> w0 prostopad�y do vNorm -> punkt pP0 le�y na p�aszczy�nie tr�jk�ta

		if (a == 0)
		{
			pIntersectionPoint = pP0;
			pDistance = 0.0;

			return true;
		}

		double b = m_normal.dotProduct(vRay);	// b == 0 -> vRay prostopad�y do vNorm -> vRay jest r�wnoleg�y do tr�jk�ta
									// b < 0  -> vRay wpada od przodu -> OK
									// b > 0  -> vRay wpada od ty�u -> NIE OK

		//if ( fabs(b) < prawieZero ) // to jest chyba niepotrzebne o ile sie nie pojawi� b��dy
		if (b == 0)
		{ // vRay jest r�wnoleg�y do p�aszczyzny tr�jk�ta
			return false;
		}

		double r = a / b;

		// Wyznaczam punkt przeci�cia promienia z p�aszczyzna �ciany
		CVector3d vec = (vRay * a) / b;

		pDistance = abs(r);
		pIntersectionPoint = pP0 + vec;

		return true;
	}




	bool lineSegmentIntersection(CPoint3d P1, CPoint3d P2, CPoint3d& outP)
	{
		double d1 = pointDistance(P1), d2 = pointDistance(P2);

		if (d1 == 0)
		{
			if (d2 == 0)
			{
				// ten sam punkt, albo oba punkty na p�aszczy�nie
				return false;
			}
			else
			{
				outP = P1;
				return true;
			}
		}
		else if (d2 == 0)
		{
			outP = P2;
			return true;
		}

		if (d1 * d2 > 0)  // points on the same side of plane
		{
			return false;
		}

		double t = d1 / (d1 - d2); // 'time' of intersection point on the segment
		
		outP = P1 + CVector3d(P1,P2)*t;

		return true;
	}

	//template <typename Container> void regresionFromPoints(Container)
	//{
	//	static_assert(std::is_same<typename Container::value_type, CPoint3d>::value, "BOOM!");
		// print what was deduced
	//	std::cout << __PRETTY_FUNCTION__ << '\n';
	//}

	void regresionFromPoints(std::vector<CPoint3d> pts)
	{
		size_t pts_cnt = pts.size();

		double sum_pts[3];
		sum_pts[0] = 0.0;
		sum_pts[1] = 0.0;
		sum_pts[2] = 0.0;

		for (int ii = 0; ii < pts_cnt; ii++)
		{
			sum_pts[0] += pts[ii].X();
			sum_pts[1] += pts[ii].Y();
			sum_pts[2] += pts[ii].Z();
		}

		CPoint3d centroid(sum_pts[0] / pts_cnt, sum_pts[1] / pts_cnt, sum_pts[2] / pts_cnt);

		// Calculate full 3x3 covariance matrix, excluding symmetries:
		double xx = 0.0, xy = 0.0, xz = 0.0;
		double yy = 0.0, yz = 0.0, zz = 0.0;

		for (int ii = 0; ii < pts_cnt; ii++)
		{
			CVector3d r;
			r = pts[ii] - centroid;
			xx += r.X() * r.X();
			xy += r.X() * r.Y();
			xz += r.X() * r.Z();
			yy += r.Y() * r.Y();
			yz += r.Y() * r.Z();
			zz += r.Z() * r.Z();
		}

		xx /= pts_cnt;
		xy /= pts_cnt;
		xz /= pts_cnt;
		yy /= pts_cnt;
		yz /= pts_cnt;
		zz /= pts_cnt;

		CVector3d weighted_dir(0.0, 0.0, 0.0);

		double det_x = yy * zz - yz * yz;

		CVector3d axis_dir_x(det_x, xz * yz - xy * zz, xy * yz - xz * yy);

		double weight = det_x * det_x;

		if (weighted_dir.dotProduct(axis_dir_x) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		double det_y = xx * zz - xz * xz;
		CVector3d axis_dir_y(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
		weight = det_y * det_y;
		if (weighted_dir.dotProduct(axis_dir_y) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_y * weight;

		double det_z = xx * yy - xy * xy;
		CVector3d axis_dir_z(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
		weight = det_z * det_z;
		if (weighted_dir.dotProduct(axis_dir_z) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		m_center = centroid;
		m_normal = weighted_dir;
		m_normal.normalize();

		m_d = recalc_D();
	}

	bool regresionFromPoints(std::set<CVertex>& pts)
	{
		size_t pts_cnt = pts.size();

		if (pts_cnt < 3) return false;

		CPoint3d centroid = std::accumulate(pts.begin(), pts.end(), CPoint3d(0.0, 0.0, 0.0)) / pts_cnt;

		// Calculate full 3x3 covariance matrix, excluding symmetries:
		double xx = 0.0, xy = 0.0, xz = 0.0;
		double yy = 0.0, yz = 0.0, zz = 0.0;

		//for each (CVertex v in pts)
		for (CVertex v : pts)
		{
			CVector3d r;
			r = (CPoint3d)v - centroid;
			xx += r.X() * r.X();
			xy += r.X() * r.Y();
			xz += r.X() * r.Z();
			yy += r.Y() * r.Y();
			yz += r.Y() * r.Z();
			zz += r.Z() * r.Z();
		}

		xx /= pts_cnt;
		xy /= pts_cnt;
		xz /= pts_cnt;
		yy /= pts_cnt;
		yz /= pts_cnt;
		zz /= pts_cnt;

		CVector3d weighted_dir(0.0, 0.0, 0.0);

		double det_x = yy * zz - yz * yz;

		CVector3d axis_dir_x(det_x, xz * yz - xy * zz, xy * yz - xz * yy);

		double weight = det_x * det_x;

		if (weighted_dir.dotProduct(axis_dir_x) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		double det_y = xx * zz - xz * xz;
		CVector3d axis_dir_y(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
		weight = det_y * det_y;
		if (weighted_dir.dotProduct(axis_dir_y) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_y * weight;

		double det_z = xx * yy - xy * xy;
		CVector3d axis_dir_z(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
		weight = det_z * det_z;
		if (weighted_dir.dotProduct(axis_dir_z) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		m_center = centroid;
		m_normal = weighted_dir;
		m_normal.normalize();

		m_d = recalc_D();

		return true;
	}


	void regresionFromPoints(std::set<size_t>& idxs, std::vector<CPoint3f>& pts)
	{
		size_t pts_cnt = idxs.size();

		double sum_pts[3];
		sum_pts[0] = 0.0;
		sum_pts[1] = 0.0;
		sum_pts[2] = 0.0;

		for (auto ii : idxs)
		{
			sum_pts[0] += pts[ii].X();
			sum_pts[1] += pts[ii].Y();
			sum_pts[2] += pts[ii].Z();
		}

		CPoint3d centroid(sum_pts[0] / pts_cnt, sum_pts[1] / pts_cnt, sum_pts[2] / pts_cnt);

		// Calculate full 3x3 covariance matrix, excluding symmetries:
		double xx = 0.0, xy = 0.0, xz = 0.0;
		double yy = 0.0, yz = 0.0, zz = 0.0;

		for (auto ii : idxs)
		{
			CVector3d r;
			r = pts[ii] - centroid;
			xx += r.X() * r.X();
			xy += r.X() * r.Y();
			xz += r.X() * r.Z();
			yy += r.Y() * r.Y();
			yz += r.Y() * r.Z();
			zz += r.Z() * r.Z();
		}

		xx /= pts_cnt;
		xy /= pts_cnt;
		xz /= pts_cnt;
		yy /= pts_cnt;
		yz /= pts_cnt;
		zz /= pts_cnt;

		CVector3d weighted_dir(0.0, 0.0, 0.0);

		double det_x = yy * zz - yz * yz;

		CVector3d axis_dir_x(det_x, xz * yz - xy * zz, xy * yz - xz * yy);

		double weight = det_x * det_x;

		if (weighted_dir.dotProduct(axis_dir_x) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		double det_y = xx * zz - xz * xz;
		CVector3d axis_dir_y(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
		weight = det_y * det_y;
		if (weighted_dir.dotProduct(axis_dir_y) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_y * weight;

		double det_z = xx * yy - xy * xy;
		CVector3d axis_dir_z(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
		weight = det_z * det_z;
		if (weighted_dir.dotProduct(axis_dir_z) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		m_center = centroid;
		m_normal = weighted_dir;
		m_normal.normalize();

		m_d = recalc_D();
	}

	void regresionFromPoints(std::set<INDEX_TYPE>& idxs, std::vector<CVertex>& pts)
	{
		size_t pts_cnt = idxs.size();

		double sum_pts[3];
		sum_pts[0] = 0.0;
		sum_pts[1] = 0.0;
		sum_pts[2] = 0.0;

		for (auto ii : idxs)
		{
			sum_pts[0] += pts[ii].X();
			sum_pts[1] += pts[ii].Y();
			sum_pts[2] += pts[ii].Z();
		}

		CPoint3d centroid(sum_pts[0] / pts_cnt, sum_pts[1] / pts_cnt, sum_pts[2] / pts_cnt);

		// Calculate full 3x3 covariance matrix, excluding symmetries:
		double xx = 0.0, xy = 0.0, xz = 0.0;
		double yy = 0.0, yz = 0.0, zz = 0.0;

		for (auto ii : idxs)
		{
			CVector3d r;
			r = pts[ii] - centroid;
			xx += r.X() * r.X();
			xy += r.X() * r.Y();
			xz += r.X() * r.Z();
			yy += r.Y() * r.Y();
			yz += r.Y() * r.Z();
			zz += r.Z() * r.Z();
		}

		xx /= pts_cnt;
		xy /= pts_cnt;
		xz /= pts_cnt;
		yy /= pts_cnt;
		yz /= pts_cnt;
		zz /= pts_cnt;

		CVector3d weighted_dir(0.0, 0.0, 0.0);

		double det_x = yy * zz - yz * yz;

		CVector3d axis_dir_x(det_x, xz * yz - xy * zz, xy * yz - xz * yy);

		double weight = det_x * det_x;

		if (weighted_dir.dotProduct(axis_dir_x) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		double det_y = xx * zz - xz * xz;
		CVector3d axis_dir_y(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
		weight = det_y * det_y;
		if (weighted_dir.dotProduct(axis_dir_y) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_y * weight;

		double det_z = xx * yy - xy * xy;
		CVector3d axis_dir_z(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
		weight = det_z * det_z;
		if (weighted_dir.dotProduct(axis_dir_z) < 0.0)
			weight = -weight;
		weighted_dir += axis_dir_x * weight;

		m_center = centroid;
		m_normal = weighted_dir;
		m_normal.normalize();

		m_d = recalc_D();
	}

	double testPointPositionD(CPoint3d p)
	{
		CVector3d cp(m_center, p);
		cp.normalize();

		double test = cp.dotProduct(m_normal);

		return test;
	}

	int testPointPosition(CPoint3d p)
	{
		CVector3d cp(m_center, p);
		cp.normalize();

		double test = cp.dotProduct(m_normal);

		if (test > 0) return 1;
		if (test < 0) return -1;
		return 0;
	}


	CMesh* getMesh( double size, int divX=1, int divY=1 );
	void toMatrix(double* m);
	CTransform toTransform1();
	Eigen::Matrix4d toEigenMatrix4d();
	CTransform toTransform();
	CPlane get_transformed(Eigen::Matrix4d M);
};

#endif /* _PLANE_H_BY_DP_INCLUDED_ */
