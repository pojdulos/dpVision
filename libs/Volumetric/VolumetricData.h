#pragma once

#include "dll_global.h"

#include "Object.h"
#include "RGBA.h"

#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/Impl/Utility.h"

#include "PolyVoxCore/Log.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/RawVolume.h"

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

const int32_t g_uVolumeSideLength = 128;

struct OpenGLSurfaceMesh
{
	unsigned long noOfIndices;
	unsigned int indexBuffer;
	unsigned int vertexBuffer;
	const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* sourceMesh;
};

class DPVISION_EXPORT2 CVolumetricData : public CObject
{
public:
	typedef PolyVox::MaterialDensityPair44 VoxelType;
	typedef PolyVox::LargeVolume< CVolumetricData::VoxelType > VolumeType;

	CVolumetricData(void);
	CVolumetricData(CBaseObject *p);
	~CVolumetricData(void);

	virtual inline int type() { return CObject::Type::VOLUMETRIC; };

	void render();
	void setVolume( CVolumetricData::VolumeType* volData );

	unsigned char m_type;
	unsigned char m_alpha;

	size_t size()
	{
		return m_volData->getWidth() * m_volData->getHeight() * m_volData->getDepth();
	};

protected:
	CVolumetricData::VolumeType* m_volData;

	unsigned int m_uRegionSideLength;
	unsigned int m_uVolumeWidthInRegions;
	unsigned int m_uVolumeHeightInRegions;
	unsigned int m_uVolumeDepthInRegions;

    bool m_bUseOpenGLVertexBufferObjects;

	//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
	std::map<PolyVox::Vector3DUint8, OpenGLSurfaceMesh> m_mapOpenGLSurfaceMeshes;
	std::map<PolyVox::Vector3DUint8, polyvox_shared_ptr<PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> > > m_mapSurfaceMeshes;

	int create();
	void createSphereInVolume( CVolumetricData::VolumeType& volData, float fRadius, uint8_t uValue);
	void createCubeInVolume( CVolumetricData::VolumeType& volData, PolyVox::Vector3DInt32 lowerCorner, PolyVox::Vector3DInt32 upperCorner, uint8_t uValue);

	CRGBA convertMaterialIDToColour( uint8_t materialID );
	void renderRegionImmediateMode(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, unsigned int uLodLevel );

	OpenGLSurfaceMesh BuildOpenGLSurfaceMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh);
	void renderRegionVertexBufferObject(const OpenGLSurfaceMesh& openGLSurfaceMesh, unsigned int uLodLevel);
};




