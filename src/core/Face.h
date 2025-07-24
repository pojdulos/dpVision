#pragma once

#include "Global.h"

#include "Utilities.h"

#include "Vector3.h"
#include "Point3.h"
#include "Vertex.h"

/**
 * @brief Represents a triangle face in a mesh, defined by three vertex indices.
 *
 * Provides accessors, mutators, and geometric utilities for working with mesh faces.
 * Supports comparison for use as a key in associative containers.
 */
class DPVISION_EXPORT CFace
{
	INDEX_TYPE x, y, z;

public:
	/**
	 * @brief Default constructor. Initializes all indices to zero.
	 */
	CFace() : x(0), y(0), z(0) {}

	/**
	 * @brief Constructor from three vertex indices.
	 * @param ax Index of the first vertex
	 * @param ay Index of the second vertex
	 * @param az Index of the third vertex
	 */
	CFace(INDEX_TYPE ax, INDEX_TYPE ay, INDEX_TYPE az) : x(ax), y(ay), z(az) {}

	/// @name Accessors and Mutators
	/// @{

	/// Get the index of the first vertex.
	inline INDEX_TYPE A() const { return x; }
	/// Set the index of the first vertex.
	inline INDEX_TYPE A(const INDEX_TYPE _a) { return x = _a; }

	/// Get the index of the second vertex.
	inline INDEX_TYPE B() const { return y; }
	/// Set the index of the second vertex.
	inline INDEX_TYPE B(const INDEX_TYPE _b) { return y = _b; }

	/// Get the index of the third vertex.
	inline INDEX_TYPE C() const { return z; }
	/// Set the index of the third vertex.
	inline INDEX_TYPE C(const INDEX_TYPE _c) { return z = _c; }

	/**
	 * @brief Set all three vertex indices at once.
	 * @param ax Index of the first vertex
	 * @param ay Index of the second vertex
	 * @param az Index of the third vertex
	 */
	inline void Set(INDEX_TYPE ax, INDEX_TYPE ay, INDEX_TYPE az)
	{
		x = ax;
		y = ay;
		z = az;
	}

	/**
	 * @brief Get the index at the specified position (0, 1, or 2).
	 * @param n Position (0 for A, 1 for B, 2 for C)
	 * @return Vertex index at the given position, or -1 if out of range
	 */
	INDEX_TYPE at(int n) const;

	/**
	 * @brief Set the index at the specified position (0, 1, or 2).
	 * @param n Position (0 for A, 1 for B, 2 for C)
	 * @param v_id Vertex index to set
	 */
	void setAt(int n, const INDEX_TYPE v_id);

	/// @}

	/**
	 * @brief Inverts the order of the second and third vertex indices.
	 */
	void invert();

	/**
	 * @brief Computes the normal vector of the face using the provided vertex list.
	 * @param vertices List of mesh vertices
	 * @return Normalized normal vector of the face
	 */
	CVector3d getNormal(const std::vector<CVertex>& vertices);

	/**
	 * @brief Computes the normal vector of a triangle given its three points.
	 * @param A First point
	 * @param B Second point
	 * @param C Third point
	 * @return Normalized normal vector of the triangle
	 */
	static CVector3d triangleNormal(CPoint3d A, CPoint3d B, CPoint3d C);

	/**
	 * @brief Computes the area of a triangle given its three points.
	 * @param n0 First point
	 * @param n1 Second point
	 * @param n2 Third point
	 * @return Area of the triangle
	 */
	static double triangleArea(CPoint3d n0, CPoint3d n1, CPoint3d n2);

	/**
	 * @brief Checks if the face contains the given vertex index.
	 * @param v_id Vertex index to check
	 * @return 0 if at A, 1 if at B, 2 if at C, -1 if not present
	 */
	int hasVertex(const INDEX_TYPE v_id);

	/**
	 * @brief Lexicographical comparison operator for use in associative containers.
	 * @param c Face to compare
	 * @return true if this face is less than c
	 */
	bool operator<(const CFace& c) const {
		if (x < c.x) return true;
		else if (x > c.x) return false;
		else if (y < c.y) return true;
		else if (y > c.y) return false;
		return (z < c.z);
	}
};
