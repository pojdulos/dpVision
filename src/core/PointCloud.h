#pragma once
#include "Texture.h"
#include "Global.h"
#include "Material.h"

#include "Object.h"

#include "Vertex.h"
#include "KDTree.h"
//#include "UI.h"

class CPlane;

class DPVISION_EXPORT CPointCloud : public CObject
{
public:
	typedef std::vector<CVertex> Vertices;
	typedef std::vector<CRGBA> Colors;
	typedef std::vector<CVector3f> Normals;
	typedef std::vector<CMaterial*> Materials;

	class DPVISION_EXPORT KDtree : private trimesh::KDtree
	{
		CPointCloud::Vertices &m_vertices;

	public:
		enum {
			PRESERVE = false,
			REBUILD = true
		};

		KDtree(CPointCloud::Vertices &v) : trimesh::KDtree(v), m_vertices(v) {};
		~KDtree() {};

		inline int closest_to_pt(CPoint3f p, float maxdist = 0.0f) const
		{
			const CVertex *v = (const CVertex *)trimesh::KDtree::closest_to_pt(p.toVector(), maxdist*maxdist, NULL);
			
			return (v == NULL) ? -1 : (v - &m_vertices[0]);
		};


		inline int closest_to_ray(CPoint3f p, CVector3f dir, float maxdist = 0.0f) const
		{
			const CVertex *v = (const CVertex *)trimesh::KDtree::closest_to_ray(p.toVector(), dir.toVector(), maxdist*maxdist, NULL);

			return (v == NULL) ? -1 : (v - &m_vertices[0]);
		};

		std::vector<INDEX_TYPE> find_k_closest_to_pt(int k, CPoint3f p, float maxdist = 0.0f) const;
		std::vector<INDEX_TYPE> find_k_closest_to_ray(int k, CPoint3f p, CVector3f dir, float maxdist = 0.0f) const;
		bool is_any_in_distance_to_pt(float distance, CPoint3f p) const;
		std::vector<INDEX_TYPE> find_all_in_distance_to_pt(float distance, CPoint3f p) const;
		std::vector<INDEX_TYPE> find_all_in_distance_to_ray(float distance, CPoint3f p, CVector3f dir ) const;
		std::vector<std::pair<INDEX_TYPE, float>> find_all_in_distance_to_ray_2(float distance, CPoint3f p, CVector3f dir) const;

		void clear();
		void rebuild();
	};


protected:
	Vertices m_vertices;
	Colors m_vcolors;
	Normals m_vnormals;

	Materials m_materials;

	CPointCloud::KDtree *m_kdtree;

	//static IRenderer* renderer_;


public:
	static int m_pointSize;

	CPointCloud(std::shared_ptr<CBaseObject> p= nullptr);
	CPointCloud(CPointCloud&);
	~CPointCloud();

	inline int type() override { return CObject::Type::CLOUD; };
	std::wstring infoRow() override;

	virtual std::shared_ptr<CBaseObject> getCopy() override {
		auto obj = std::make_shared<CPointCloud>(*this); 
		updateChildrenParentPointers(obj);
		return obj;
	};

	//inline CVertex &vertex(size_t i) { return m_vertices.at(i); };
	//inline CRGBA &vertexColor(size_t i) { return m_vcolors.at(i); };
	//inline CVector3f &vertexNormal(size_t i) { return m_vnormals.at(i); };

	inline Vertices &vertices() { return m_vertices; };
	inline Colors &vcolors() { return m_vcolors; };
	inline Normals &vnormals() { return m_vnormals; };

	// Fast, return reference. But to access the vector out of index will result in undefined behavior
	inline CVertex& vertex(size_t idx)
	{
		return m_vertices[idx];
	};

	// Slower than vertex(), return reference too. Signals if the requested position is out of range by throwing an out_of_range exception.
	inline CVertex& vertexAt(size_t idx)
	{
		return m_vertices.at(idx);
	};

	// Returns a copy of the vertex as opposed to vertex() and vertexAt().
	// You can check if the index was correct using the 'status' variable.
	// Remember that if the index was invalid, the vertex returned is (0.0,0.0,0.0)
	inline CVertex getVertex(size_t idx, bool *status = nullptr)
	{
		if (m_vertices.size() > idx)
		{
			if (status != nullptr) *status = true;
			return m_vertices[idx];
		}
		if (status != nullptr) *status = false;
		return CVertex();
	};


	inline size_t addVertex(CVertex v) {
		m_vertices.push_back(v);
		expandBoundingBox(v);
		m_modified = true;
		return m_vertices.size() - 1;
	};

	inline size_t addVertex(float x, float y, float z) { return addVertex(CVertex(x, y, z)); };

	size_t addVertex(CVertex v, CRGBA c);
	size_t addVertex(CVertex v, CVector3d n);
	size_t addVertex(CVertex v, CRGBA c, CVector3d n);

	bool setVertex(size_t idx, CVertex v);
	inline bool setVertex(size_t idx, float x, float y, float z) { return setVertex(idx, CVertex(x, y, z)); };

	bool setVertexColor(size_t idx, CRGBA vColor);
	inline bool setVertexColor(size_t idx, unsigned char r, unsigned char g, unsigned char b, unsigned char a=255U) { return setVertexColor(idx, CRGBA(r, g, b, a)); };
	inline bool setVertexColor(size_t idx, float r, float g, float b, float a=1.0f) { return setVertexColor(idx, CRGBA(r, g, b, a)); };

	bool setVertexNormal(size_t idx, CVector3f vNormal);
	inline bool setVertexNormal(size_t idx, float x, float y, float z) { return setVertexNormal(idx, CVector3f(x, y, z)); };

	bool removeVertex(size_t idx);



	inline Materials &materials() { return m_materials; }
	inline CMaterial& getMaterial(INDEX_TYPE idx = 0) { return *m_materials[idx]; };

	//static int getPointSize() { return m_pointSize; };
	//static void setPointSize(int s) { m_pointSize = s; };

	CVertex & operator[](INDEX_TYPE index) { return m_vertices[index]; }
	const CVertex & operator[](INDEX_TYPE index) const { return m_vertices[index]; }

	inline size_t size() { return m_vertices.size(); };

	inline bool hasVertexColors() { return !m_vcolors.empty() && (m_vcolors.size() >= m_vertices.size()); };
	inline bool hasVertexNormals() { return !m_vnormals.empty() && (m_vnormals.size() >= m_vertices.size()); };

	//void renderBACKUP();

	//virtual void renderSelf() override;
	//virtual void render() override;



	void rebuildBoundingBox();

	void createKDtree();
	void removeKDtree();
	CPointCloud::KDtree & getKDtree(bool rebuild);

	CVertex getClosestVertex(CPoint3f pkt, CVector3f vec);

	CPoint3f getCenterOfWeight();

	virtual void applyTransformation(CTransform& from, CTransform& to) override;

	void transformByMatrixF(float matrix[16]);
	void transformByMatrixD(double matrix[16]);
	void cutPlane(CPlane &m_cutPlane, std::shared_ptr<CPointCloud> &rest);
	static std::shared_ptr<CPointCloud> findNClosest(std::shared_ptr<CPointCloud> ruchoma, std::shared_ptr<CPointCloud> nieruchoma, int limit);
};

typedef CPointCloud* PtrPointCloud;
typedef CPointCloud& RefPointCloud;
