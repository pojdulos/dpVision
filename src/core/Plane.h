#pragma once

#include "dll_global.h"

#include "Triple.h"
#include "Matrix3x3.h"

#include "BoundVector.h"
#include <set>
#include "Transform.h"
#include "Vertex.h"


class CMesh;


/**
 * @brief Class representing a 3D plane defined by a point and a normal vector.
 * 
 * Provides geometric operations such as projection, intersection, regression from points,
 * and various utility methods for plane manipulation and queries.
 */
class DPVISION_EXPORT CPlane {
public:
	CPoint3d m_center;   ///< A point on the plane (center)
	CVector3d m_normal;  ///< Normal vector of the plane (normalized)
	double m_d;          ///< Plane equation parameter D (Ax + By + Cz + D = 0)

	/**
	 * @brief Default constructor. Initializes the plane as y = 0.
	 */
	CPlane()
	{
		m_center.Set(0.0, 0.0, 0.0);
		m_normal.Set(0.0, 1.0, 0.0);
		m_d = recalc_D();
	}

	/**
	 * @brief Copy constructor.
	 * @param p Plane to copy
	 */
	CPlane(const CPlane& p)
	{
		m_center = p.m_center;
		m_normal = p.m_normal;

		m_normal.normalize();
		m_d = recalc_D();
	}

	/**
	 * @brief Construct plane from a point and a normal vector.
	 * @param c Point on the plane
	 * @param n Normal vector
	 */
	CPlane(const CPoint3d& c, const CVector3d& n)
	{
		m_center = c;
		m_normal = n;
		m_normal.normalize();
		m_d = recalc_D();
	}

	/**
	 * @brief Construct plane from three points.
	 * @param p0 First point
	 * @param p1 Second point
	 * @param p2 Third point
	 */
	CPlane(CPoint3d p0, CPoint3d p1, CPoint3d p2)
	{
		CVector3d v01(p0, p1);
		CVector3d v02(p0, p2);

		m_normal = v01.crossProduct(v02).getNormalized();
		m_center = (p0+p1+p2)/3;
		m_d = recalc_D();
	}



	/// @name Getters and Setters
	/// @{

	/// Get the center point of the plane.
	CPoint3d getCenter() const noexcept { return m_center; }
	/// Get the normal vector of the plane.
	CVector3d getNormal() const noexcept { return m_normal; }
	/// Get the A coefficient of the plane equation.
	double a() const noexcept { return m_normal.X(); }
	/// Get the B coefficient of the plane equation.
	double b() const noexcept { return m_normal.Y(); }
	/// Get the C coefficient of the plane equation.
	double c() const noexcept { return m_normal.Z(); }
	/// Get the D coefficient of the plane equation.
	double d() const noexcept { return m_d; }

	/// Set the center point of the plane.
	void setCenter(CPoint3d c)
	{
		m_center = c;
		m_d = recalc_D();
	}

	/// Set the normal vector of the plane.
	void setNormal(CVector3d n)
	{
		m_normal = n;
		m_normal.normalize();
		m_d = recalc_D();
	}

	/// Invert the normal vector of the plane.
	void invertNormal()
	{
		m_normal.invert();
		m_d = recalc_D();
	}
	/// @}

	/**
	 * @brief Recalculate the D parameter of the plane equation.
	 * @return The D value
	 */
	double recalc_D() const noexcept {
		return -(m_normal.X() * m_center.X() + m_normal.Y() * m_center.Y() + m_normal.Z() * m_center.Z());
	}

	/**
	 * @brief Project a point onto the plane.
	 * @param p Point to project
	 * @return Projected point on the plane
	 */
	CPoint3d projectionOfPoint(CPoint3d p) const {
		double t = -(m_normal.X() * p.X() + m_normal.Y() * p.Y() + m_normal.Z() * p.Z() + m_d) / m_normal.squaredLength();
		CPoint3d ret = p + m_normal * t;
		return ret;
	}

	/**
	 * @brief Compute the intersection line of this plane with another plane.
	 * @param p2 The other plane
	 * @param r_point Output: a point on the intersection line
	 * @param r_normal Output: direction vector of the intersection line
	 * @return true if planes intersect, false if parallel
	 */
	bool intersectionWithPlane(CPlane& p2, CPoint3d& r_point, CVector3d& r_normal) const
	{
		CVector3d p3_normal = this->m_normal.crossProduct(p2.m_normal);
		double det = p3_normal.squaredLength();

		if (det != 0.0) {
			r_point = (((p3_normal.crossProduct(p2.m_normal) * m_d) + (this->m_normal.crossProduct(p3_normal) * p2.m_d)) / det);
			r_normal = p3_normal;
			return true;
		}
		else { // planes are parallel
			return false;
		}
	}


	/**
	 * @brief Signed distance from a point to the plane.
	 * @param P Point to test
	 * @return Signed distance
	 */
	inline double pointDistance(CPoint3d P) const noexcept {
		return m_normal.dotProduct( CVector3d(CPoint3d(),P) ) + m_d;
	}

	/**
	 * @brief Intersect a ray (point + direction) with the plane, up to maxDist.
	 * @param P1 Ray origin
	 * @param v Ray direction
	 * @param maxDist Maximum distance along the ray
	 * @param outP Output: intersection point
	 * @return true if intersection occurs within maxDist
	 */
	bool rayIntersection(CPoint3d P1, CVector3d v, double maxDist, CPoint3d& outP) const
	{
		CPoint3d P2 = P1 + v * maxDist;
		return lineSegmentIntersection(P1, P2, outP);
	}

	/**
	 * @brief Intersect a 3D ray with the plane.
	 * @param vRay Ray direction
	 * @param pP0 Ray origin
	 * @param pIntersectionPoint Output: intersection point
	 * @param pDistance Output: distance from origin to intersection
	 * @return true if intersection occurs
	 */
	bool rayIntersect3D(CVector3d vRay, CPoint3d pP0, CPoint3d& pIntersectionPoint, double& pDistance) const
	{
		if (0 == m_normal.length()) return false;

		CVector3d w0 = CVector3d(m_center, pP0);
		double a = -m_normal.dotProduct(w0);

		if (a == 0)
		{
			pIntersectionPoint = pP0;
			pDistance = 0.0;
			return true;
		}

		double b = m_normal.dotProduct(vRay);

		if (b == 0)
		{
			return false;
		}

		double r = a / b;
		CVector3d vec = (vRay * a) / b;

		pDistance = abs(r);
		pIntersectionPoint = pP0 + vec;

		return true;
	}


	/**
	 * @brief Intersect a line segment with the plane.
	 * @param P1 First endpoint
	 * @param P2 Second endpoint
	 * @param outP Output: intersection point
	 * @return true if intersection occurs
	 */
	bool lineSegmentIntersection(CPoint3d P1, CPoint3d P2, CPoint3d& outP) const
	{
		double d1 = pointDistance(P1), d2 = pointDistance(P2);

		if (d1 == 0)
		{
			if (d2 == 0)
			{
				// both points on the plane
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


	/**
	 * @brief Returns the signed value indicating the position of a point relative to the plane.
	 * @param p Point to test
	 * @return Signed value (positive: in front, negative: behind, zero: on the plane)
	 */
	double testPointPositionD(CPoint3d p) const noexcept {
		CVector3d cp(m_center, p);
		cp.normalize();
		double test = cp.dotProduct(m_normal);
		return test;
	}

	/**
	 * @brief Returns the position of a point relative to the plane.
	 * @param p Point to test
	 * @return 1 if in front, -1 if behind, 0 if on the plane
	 */
	int testPointPosition(CPoint3d p) const noexcept {
		CVector3d cp(m_center, p);
		cp.normalize();
		double test = cp.dotProduct(m_normal);
		if (test > 0) return 1;
		if (test < 0) return -1;
		return 0;
	}

	/**
	 * @brief Equality operator for planes.
	 * @param other Plane to compare
	 * @return true if planes have the same normal and D
	 */
	bool operator==(const CPlane& other) const noexcept {
    	return m_normal == other.m_normal && m_d == other.m_d;
	}

	/**
	 * @brief Offset the plane along its normal by a given distance.
	 * @param dist Distance to offset
	 */
	void offset(double dist) {
    	m_center += m_normal * dist;
    	m_d = recalc_D();
	}

	/**
	 * @brief Get the plane equation coefficients as an array [A, B, C, D].
	 * @return Array of coefficients
	 */
	std::array<double, 4> getEquation() const noexcept {
    	return {m_normal.X(), m_normal.Y(), m_normal.Z(), m_d};
	}

	/**
	 * @brief Check if this plane is parallel to another plane.
	 * @param other Plane to compare
	 * @return true if planes are parallel
	 */
	bool isParallelTo(const CPlane& other) const noexcept {
    	return m_normal.crossProduct(other.m_normal).squaredLength() < 1e-12;
	}

	/**
	 * @brief Check if this plane is coincident with another plane.
	 * @param other Plane to compare
	 * @return true if planes are coincident
	 */
	bool isCoincidentWith(const CPlane& other) const noexcept {
    	return isParallelTo(other) && fabs(m_d - other.m_d) < 1e-12;
	}

	/**
	 * @brief Signed distance from a point to the plane.
	 * @param p Point to test
	 * @return Signed distance
	 */
	double signedDistanceTo(const CPoint3d& p) const noexcept {
    	return m_normal.dotProduct(CVector3d(m_center, p)) + m_d;
	}

	/**
	 * @brief Project a vector onto the plane.
	 * @param v Vector to project
	 * @return Projected vector
	 */
	CVector3d projectVector(const CVector3d& v) const noexcept {
    	return v - m_normal * v.dotProduct(m_normal) / m_normal.squaredLength();
	}


	/**
	 * @brief Generate a mesh representation of the plane.
	 * @param size Size of the mesh
	 * @param divX Number of divisions along X
	 * @param divY Number of divisions along Y
	 * @return Shared pointer to the mesh
	 */
	std::shared_ptr<CMesh> getMesh( double size, int divX=1, int divY=1 );

	/**
	 * @brief Convert the plane to a transformation (method 1).
	 * @return Transformation object
	 */
	CTransform toTransform1();

	/**
	 * @brief Convert the plane to an Eigen 4x4 matrix.
	 * @return Eigen 4x4 matrix
	 */
	Eigen::Matrix4d toEigenMatrix4d() const;

	/**
	 * @brief Convert the plane to a transformation.
	 * @return Transformation object
	 */
	CTransform toTransform();

	/**
	 * @brief Get a transformed copy of the plane by a 4x4 matrix.
	 * @param M Transformation matrix
	 * @return Transformed plane
	 */
	CPlane get_transformed(Eigen::Matrix4d M);


private:
	// Helper for regression: computes centroid and normal from a range of points
	template<typename PointIterator, typename PointAccessor>
	void regressionFromPointsImpl(PointIterator begin, PointIterator end, PointAccessor getPoint) {
		const size_t pts_cnt = std::distance(begin, end);
		if (pts_cnt < 3) return;

		// Compute centroid
		CPoint3d centroid(0.0, 0.0, 0.0);
		for (auto it = begin; it != end; ++it) {
			const auto& pt = getPoint(*it);
			centroid += pt;
		}
		centroid /= pts_cnt;

		// Compute covariance matrix
		double xx = 0.0, xy = 0.0, xz = 0.0;
		double yy = 0.0, yz = 0.0, zz = 0.0;
		for (auto it = begin; it != end; ++it) {
			CVector3d r = getPoint(*it) - centroid;
			xx += r.X() * r.X();
			xy += r.X() * r.Y();
			xz += r.X() * r.Z();
			yy += r.Y() * r.Y();
			yz += r.Y() * r.Z();
			zz += r.Z() * r.Z();
		}
		xx /= pts_cnt; xy /= pts_cnt; xz /= pts_cnt;
		yy /= pts_cnt; yz /= pts_cnt; zz /= pts_cnt;

		CVector3d weighted_dir(0.0, 0.0, 0.0);

		double det_x = yy * zz - yz * yz;
		CVector3d axis_dir_x(det_x, xz * yz - xy * zz, xy * yz - xz * yy);
		double weight = det_x * det_x;
		if (weighted_dir.dotProduct(axis_dir_x) < 0.0) weight = -weight;
		weighted_dir += axis_dir_x * weight;

		double det_y = xx * zz - xz * xz;
		CVector3d axis_dir_y(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
		weight = det_y * det_y;
		if (weighted_dir.dotProduct(axis_dir_y) < 0.0) weight = -weight;
		weighted_dir += axis_dir_y * weight;

		double det_z = xx * yy - xy * xy;
		CVector3d axis_dir_z(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
		weight = det_z * det_z;
		if (weighted_dir.dotProduct(axis_dir_z) < 0.0) weight = -weight;
		weighted_dir += axis_dir_z * weight;

		m_center = centroid;
		m_normal = weighted_dir;
		m_normal.normalize();
		m_d = recalc_D();
	}	

public:
	/**
	 * @brief Fits a plane to a container of 3D points using least squares regression.
	 *
	 * This method computes the best-fit plane (in the least squares sense) for a set of 3D points.
	 * The plane's center and normal are updated to match the regression result.
	 *
	 * @tparam PointContainer Container type holding 3D points (e.g., CVertex, CPoint3f, CPoint3d, etc.)
	 * @param pts Container of 3D points
	 */
	template<typename PointContainer>
	void regresionFromPoints(const PointContainer& pts) {
		regressionFromPointsImpl(pts.begin(), pts.end(), [](const auto& p) -> const CPoint3d& { return p; });
	}

	/**
	 * @brief Fits a plane to a subset of points, specified by indices into a container.
	 *
	 * This method computes the best-fit plane (in the least squares sense) for a subset of 3D points,
	 * where the subset is specified by a container of indices into a point container.
	 * The plane's center and normal are updated to match the regression result.
	 *
	 * @tparam IndexContainer Container type holding indices (e.g., std::set<size_t>)
	 * @tparam PointContainer Container type holding 3D points
	 * @param idxs Container of indices into the point container
	 * @param pts Container of 3D points
	 */
	template<typename IndexContainer, typename PointContainer>
	void regresionFromPoints(const IndexContainer& idxs, const PointContainer& pts) {
		regressionFromPointsImpl(idxs.begin(), idxs.end(), [&pts](auto idx) -> const CPoint3d& { return pts[idx]; });
	}
};
