#include "Plane.h"
#include "Mesh.h"


/**
 * @brief Generates a triangle mesh by dividing each square cell into two triangles.
 * @param topLeft Top-left corner of the quad
 * @param topRight Top-right corner of the quad
 * @param bottomLeft Bottom-left corner of the quad
 * @param bottomRight Bottom-right corner of the quad
 * @param divisions Number of divisions along each edge (must be > 0)
 * @return Shared pointer to the generated mesh
 */
std::shared_ptr<CMesh> generateTriangleMesh(const CPoint3d& topLeft, const CPoint3d& topRight,
	const CPoint3d& bottomLeft, const CPoint3d& bottomRight, int divisions)
{
	assert(divisions > 0 && "divisions must be > 0");
	auto mesh = std::make_shared<CMesh>();

	double step = 1.0 / divisions;
	// Generate vertices
	for (int i = 0; i <= divisions; ++i) {
		for (int j = 0; j <= divisions; ++j) {
			CPoint3d vertex = topLeft + (topRight - topLeft) * step * j + (bottomLeft - topLeft) * step * i;
			mesh->addVertex(vertex);
		}
	}

	// Generate faces (indices)
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			int topLeftIndex = i * (divisions + 1) + j;
			int topRightIndex = topLeftIndex + 1;
			int bottomLeftIndex = (i + 1) * (divisions + 1) + j;
			int bottomRightIndex = bottomLeftIndex + 1;

			// First triangle
			mesh->addFace(CFace(topLeftIndex, bottomLeftIndex, topRightIndex));
			// Second triangle
			mesh->addFace(CFace(topRightIndex, bottomLeftIndex, bottomRightIndex));
		}
	}

	return mesh;
}

/**
 * @brief Generates a triangle mesh by dividing each square cell into four triangles (with a center vertex).
 * @param topLeft Top-left corner of the quad
 * @param topRight Top-right corner of the quad
 * @param bottomLeft Bottom-left corner of the quad
 * @param bottomRight Bottom-right corner of the quad
 * @param divisions Number of divisions along each edge (must be > 0)
 * @return Shared pointer to the generated mesh
 */
std::shared_ptr<CMesh> generateTriangleMesh2(const CPoint3d& topLeft, const CPoint3d& topRight,
	const CPoint3d& bottomLeft, const CPoint3d& bottomRight, int divisions )
{
	assert(divisions > 0 && "divisions must be > 0");
	auto mesh = std::make_shared<CMesh>();

	double step = 1.0 / divisions;

	// Generate grid vertices
	for (int i = 0; i <= divisions; ++i) {
		for (int j = 0; j <= divisions; ++j) {
			CPoint3d vertex = topLeft + (topRight - topLeft) * step * j + (bottomLeft - topLeft) * step * i;
			mesh->addVertex(vertex);
		}
	}

	// Add center vertices for each cell
	int offset = static_cast<int>(mesh->vertices().size());
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			CPoint3d centerVertex = (mesh->vertices()[i * (divisions + 1) + j] +
				mesh->vertices()[i * (divisions + 1) + j + 1] +
				mesh->vertices()[(i + 1) * (divisions + 1) + j] +
				mesh->vertices()[(i + 1) * (divisions + 1) + j + 1]) * 0.25;
			mesh->addVertex(centerVertex);
		}
	}

	// Generate faces (indices)
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			int topLeftIndex = i * (divisions + 1) + j;
			int topRightIndex = topLeftIndex + 1;
			int bottomLeftIndex = (i + 1) * (divisions + 1) + j;
			int bottomRightIndex = bottomLeftIndex + 1;
			int centerIndex = offset + i * divisions + j;

			// Divide the cell into four triangles
			mesh->addFace(CFace(topLeftIndex, topRightIndex, centerIndex));
			mesh->addFace(CFace(topRightIndex, bottomRightIndex, centerIndex));
			mesh->addFace(CFace(bottomRightIndex, bottomLeftIndex, centerIndex));
			mesh->addFace(CFace(bottomLeftIndex, topLeftIndex, centerIndex));
		}
	}

	return mesh;
}

/**
 * @brief Generates a mesh representation of the plane as a large square centered at m_center and oriented by m_normal.
 * 
 * The mesh is created as a square of given size, subdivided into triangles for rendering or geometric processing.
 * The orientation is determined by the plane's normal vector. The mesh material is set for both front and back faces.
 * 
 * @param size The length of the side of the square mesh to generate.
 * @param divX Number of subdivisions along the X axis (currently ignored, uses fixed value for high resolution).
 * @param divY Number of subdivisions along the Y axis (currently ignored, uses fixed value for high resolution).
 * @return Shared pointer to the generated mesh.
 */
std::shared_ptr<CMesh> CPlane::getMesh(double size, int divX, int divY)
{
	double sideLength = size;
	CPoint3d point = m_center;
	CVector3d normal = m_normal;

	// Choose an arbitrary vector not parallel to the normal
	CVector3d arbitraryVector(1, 0, 0);
	if (normal.x == 1 && normal.y == 0 && normal.z == 0)
		arbitraryVector = CVector3d(0, 1, 0);

	// Create two orthogonal vectors in the plane
	CVector3d vec1 = normal.crossProduct(arbitraryVector).getNormalized() * sideLength;
	CVector3d vec2 = normal.crossProduct(vec1).getNormalized() * sideLength;

	// Generate the four corners of the square in the plane
	CPoint3d v1 = point + vec1 + vec2;
	CPoint3d v2 = point + vec1 - vec2;
	CPoint3d v3 = point - vec1 - vec2;
	CPoint3d v4 = point - vec1 + vec2;

	// Generate the mesh (currently uses high fixed subdivision for smoothness)
	std::shared_ptr<CMesh> plane = std::shared_ptr<CMesh>( generateTriangleMesh2(v3, v4, v2, v1, 1000) );

	// Set material properties for front and back faces
	plane->getMaterial().FrontColor.ambient.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);
	plane->getMaterial().FrontColor.diffuse.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);

	plane->getMaterial().BackColor.ambient.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.diffuse.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.specular.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.emission.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.shininess = 1.0f;

	return plane;
}

Eigen::Matrix3d calculateRotationMatrix(const Eigen::Vector3d& newNormal) {
    Eigen::Vector3d defaultNormal(0, 0, 1);
    Eigen::Quaterniond q = Eigen::Quaterniond::FromTwoVectors(defaultNormal, newNormal.normalized());
    return q.toRotationMatrix();
}

Eigen::Matrix4d CPlane::toEigenMatrix4d() const
{
    Eigen::Matrix4d mat4d = Eigen::Matrix4d::Identity();
    mat4d.block<3, 3>(0, 0) = calculateRotationMatrix({ m_normal.x, m_normal.y, m_normal.z });
    mat4d.block<3, 1>(0, 3) = Eigen::Vector3d(m_center.x, m_center.y, m_center.z);
    return mat4d;
}

CTransform CPlane::toTransform() { return CTransform(toEigenMatrix4d()); }


CPlane CPlane::get_transformed(Eigen::Matrix4d M)
{
	CPoint3d p0 = this->m_center;
	CPoint3d p1 = this->m_center + this->m_normal;

	p0 = p0.transformByMatrix(M);
	p1 = p1.transformByMatrix(M);

	CPlane plane(p0, (p1 - p0).getNormalized());
	
	return plane;
}