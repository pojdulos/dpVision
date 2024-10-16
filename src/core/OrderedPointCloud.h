#pragma once
#include "PointCloud.h"

class DPVISION_EXPORT COrderedPointCloud : public CPointCloud
{
public:
	typedef std::vector<std::pair<float,float>> Angles;
	typedef std::vector<float> Distances;
	typedef std::vector<unsigned char> Reflections;
	size_t m_cols;
	size_t m_rows;

	COrderedPointCloud::Angles m_angles;
	COrderedPointCloud::Distances m_dist;
	COrderedPointCloud::Reflections m_refl;

	COrderedPointCloud(CBaseObject *p= nullptr);
	COrderedPointCloud(COrderedPointCloud&);
	~COrderedPointCloud();

	virtual COrderedPointCloud* getCopy() override { return new COrderedPointCloud(*this); };

	inline int type() override { return CObject::Type::ORDEREDCLOUD; };
	std::wstring infoRow() override;

	virtual void renderSelf() override;
private:

	void renderAsMesh();
};

typedef COrderedPointCloud* PtrOrderedPointCloud;
typedef COrderedPointCloud& RefOrderedPointCloud;
