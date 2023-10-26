#include "StdAfx.h"

#include "VolumetricData.h"

#include "PolyVoxCore/Log.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/Impl/Utility.h"

#include "PolyVoxCore/SimpleInterface.h"

#include <QtOpenGL>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGLExtensions/QtOpenGLExtensions>
#include <gl/glu.h>

//using namespace std;
//using namespace PolyVox;
//using namespace std;

CVolumetricData::CVolumetricData():m_volData(0)
{
	m_parent = 0;
	m_uRegionSideLength = 32;
	m_bUseOpenGLVertexBufferObjects = true; //false;
	m_type = 0;
	m_alpha = 255;
}

CVolumetricData::CVolumetricData(CBaseObject *p):m_volData(0)
{
	m_parent = p;
	m_uRegionSideLength = 32;
	m_bUseOpenGLVertexBufferObjects = true; //false;
	m_type = 0;
	m_alpha = 255;
}


CVolumetricData::~CVolumetricData(void)
{
}


void CVolumetricData::setVolume( CVolumetricData::VolumeType* volData )
{
	//First we free anything from the previous volume (if there was one).
	m_mapOpenGLSurfaceMeshes.clear();
	m_mapSurfaceMeshes.clear();
	m_volData = volData;

	//If we have any volume data then generate the new surface patches.
	if(m_volData != 0)
	{
		m_uVolumeWidthInRegions = volData->getWidth() / m_uRegionSideLength;
		m_uVolumeHeightInRegions = volData->getHeight() / m_uRegionSideLength;
		m_uVolumeDepthInRegions = volData->getDepth() / m_uRegionSideLength;

		//Our volume is broken down into cuboid regions, and we create one mesh for each region.
		//This three-level for loop iterates over each region.
		for(uint16_t uRegionZ = 0; uRegionZ < m_uVolumeDepthInRegions; ++uRegionZ)
		{
			//std::cout << "uRegionZ = " << uRegionZ << " of " << m_uVolumeDepthInRegions << std::endl;
			for(uint16_t uRegionY = 0; uRegionY < m_uVolumeHeightInRegions; ++uRegionY)
			{
				for(uint16_t uRegionX = 0; uRegionX < m_uVolumeWidthInRegions; ++uRegionX)
				{
					//Compute the extents of the current region
					//FIXME - This is a little complex? PolyVox could
					//provide more functions for dealing with regions?
					int32_t regionStartX = uRegionX * m_uRegionSideLength;
					int32_t regionStartY = uRegionY * m_uRegionSideLength;
					int32_t regionStartZ = uRegionZ * m_uRegionSideLength;

					int32_t regionEndX = regionStartX + m_uRegionSideLength;
					int32_t regionEndY = regionStartY + m_uRegionSideLength;
					int32_t regionEndZ = regionStartZ + m_uRegionSideLength;

					PolyVox::Vector3DInt32 regLowerCorner(regionStartX, regionStartY, regionStartZ);
					PolyVox::Vector3DInt32 regUpperCorner(regionEndX, regionEndY, regionEndZ);

					//Extract the surface for this region
					//extractSurface(m_volData, 0, PolyVox::Region(regLowerCorner, regUpperCorner), meshCurrent);

					polyvox_shared_ptr< PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> > mesh(new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>);

					PolyVox::MarchingCubesSurfaceExtractor< CVolumetricData::VolumeType > surfaceExtractor(volData, PolyVox::Region(regLowerCorner, regUpperCorner), mesh.get());

					surfaceExtractor.execute();

					//decimatedMesh->generateAveragedFaceNormals(true);

					//computeNormalsForVertices(m_volData, *(decimatedMesh.get()), SOBEL_SMOOTHED);
					//*meshCurrent = getSmoothedSurface(*meshCurrent);
					//mesh->smooth(0.3f);
					//meshCurrent->generateAveragedFaceNormals(true);

					if(mesh->m_vecTriangleIndices.size() > 0)
					{


						PolyVox::Vector3DUint8 v3dRegPos(uRegionX,uRegionY,uRegionZ);
						if(m_bUseOpenGLVertexBufferObjects)
						{
							OpenGLSurfaceMesh openGLSurfaceMesh = BuildOpenGLSurfaceMesh(*(mesh.get()));					
							m_mapOpenGLSurfaceMeshes.insert(std::make_pair(v3dRegPos, openGLSurfaceMesh));
						}
						//else
						//{
							m_mapSurfaceMeshes.insert(std::make_pair(v3dRegPos, mesh));
						//}
						//delete meshCurrent;
					}
				}
			}
		}

		//Projection matrix is dependant on volume size, so we need to set it up again.
		//setupProjectionMatrix();
	}
}


void CVolumetricData::render()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	if(m_volData)
	{
		glEnable ( GL_COLOR_MATERIAL );
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//GLfloat kol[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, kol );// FrontColor.specular.w );
		//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, kol );// FrontColor.emission.w );
		//glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0f );// FrontColor.shininess);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glShadeModel(GL_SMOOTH);

		for(uint16_t uRegionZ = 0; uRegionZ < m_uVolumeDepthInRegions; ++uRegionZ)
		{
			for(uint16_t uRegionY = 0; uRegionY < m_uVolumeHeightInRegions; ++uRegionY)
			{
				for(uint16_t uRegionX = 0; uRegionX < m_uVolumeWidthInRegions; ++uRegionX)
				{
					PolyVox::Vector3DUint8 v3dRegPos(uRegionX,uRegionY,uRegionZ);
					if(m_mapSurfaceMeshes.find(v3dRegPos) != m_mapSurfaceMeshes.end())
					{
						polyvox_shared_ptr< PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> > meshCurrent = m_mapSurfaceMeshes[v3dRegPos];
						unsigned int uLodLevel = 0; //meshCurrent->m_vecLodRecords.size() - 1;
						if(m_bUseOpenGLVertexBufferObjects)
						{													
							renderRegionVertexBufferObject( m_mapOpenGLSurfaceMeshes[v3dRegPos], uLodLevel );
						}
						else
						{						
							renderRegionImmediateMode( *meshCurrent, uLodLevel );
						}
					}
				}
			}
		}

		GLenum errCode;
		const GLubyte *errString;

		if ((errCode = glGetError()) != GL_NO_ERROR)
		{
			errString = gluErrorString(errCode);
			std::cout << "OpenGL Error: " << errString << std::endl;
		}
	}
	glPopAttrib();
}

int CVolumetricData::create()
{
	VolumeType volData(PolyVox::Region(PolyVox::Vector3DInt32(0,0,0), PolyVox::Vector3DInt32(g_uVolumeSideLength-1, g_uVolumeSideLength-1, g_uVolumeSideLength-1)));

	//Make our volume contain a sphere in the center.
	int32_t minPos = 0;
	int32_t midPos = g_uVolumeSideLength / 2;
	int32_t maxPos = g_uVolumeSideLength - 1;

	//createSphereInVolume(volData, 60.0f, 5);

	uint8_t uDensity = CVolumetricData::VoxelType::getMaxDensity();
    VoxelType voxel = volData.getVoxelAt( 5, 5, 5 );
    voxel.setDensity(uDensity);
	//voxel.setMaterial( voxel.getMaterial() & 0x03 );
	volData.setVoxelAt( 5, 5, 5, voxel );

	//createSphereInVolume(volData, 50.0f, 4);
	//createSphereInVolume(volData, 40.0f, 3);
	//createSphereInVolume(volData, 30.0f, 2);
	//createSphereInVolume(volData, 20.0f, 1);

	//createCubeInVolume(volData, Vector3DInt32(minPos, minPos, minPos), Vector3DInt32(midPos-1, midPos-1, midPos-1), 0);
	//createCubeInVolume(volData, Vector3DInt32(midPos+1, midPos+1, minPos), Vector3DInt32(maxPos, maxPos, midPos-1), 0);
	//createCubeInVolume(volData, Vector3DInt32(midPos+1, minPos, midPos+1), Vector3DInt32(maxPos, midPos-1, maxPos), 0);
	//createCubeInVolume(volData, Vector3DInt32(minPos, midPos+1, midPos+1), Vector3DInt32(midPos-1, maxPos, maxPos), 0);

	//createCubeInVolume(volData, Vector3DInt32(1, midPos-10, midPos-10), Vector3DInt32(maxPos-1, midPos+10, midPos+10), MaterialDensityPair44::getMaxDensity());
	//createCubeInVolume(volData, Vector3DInt32(midPos-10, 1, midPos-10), Vector3DInt32(midPos+10, maxPos-1, midPos+10), MaterialDensityPair44::getMaxDensity());
	//createCubeInVolume(volData, Vector3DInt32(midPos-10, midPos-10 ,1), Vector3DInt32(midPos+10, midPos+10, maxPos-1), MaterialDensityPair44::getMaxDensity());

	//I've removed this smoothing because it doesn't really make sense to apply a low pass filter to a volume with material values. 
	//I could implement the mathematical operators for MaterialDensityPair in such a way that they ignores the materials but this 
	//seems to be setting a bad example. Users can add this operators in their own classes if they want smoothing.
	//RawVolume<MaterialDensityPair44> tempVolume(PolyVox::Region(0,0,0,128, 128, 128));
	//LowPassFilter< LargeVolume<MaterialDensityPair44>, RawVolume<MaterialDensityPair44> > pass1(&volData, PolyVox::Region(Vector3DInt32(62, 62, 62), Vector3DInt32(126, 126, 126)), &tempVolume, PolyVox::Region(Vector3DInt32(62, 62, 62), Vector3DInt32(126, 126, 126)), 3);
	//pass1.executeSAT();
	//LowPassFilter< RawVolume<MaterialDensityPair44>, LargeVolume<MaterialDensityPair44> > pass2(&tempVolume, PolyVox::Region(Vector3DInt32(62, 62, 62), Vector3DInt32(126, 126, 126)), &volData, PolyVox::Region(Vector3DInt32(62, 62, 62), Vector3DInt32(126, 126, 126)), 3);
	//pass2.executeSAT();

	int32_t ttt = volData.getWidth();

	setVolume(&volData);

	return 1;
} 

CRGBA CVolumetricData::convertMaterialIDToColour( uint8_t materialID )
{
	CRGBA colour;

	if ( m_type == 0 ) // RW3
	{
		colour.R( materialID*16 );
		colour.G( materialID*16 );
		colour.B( materialID*16 );
		colour.A( m_alpha );
	}
	else if ( m_type == 1 ) // DFF
	{
		if ( materialID < 8 )
		{
			colour.R( 0 );
			colour.G( (15 - materialID)*16 );
			colour.B( 0 );
			colour.A( m_alpha );
		}
		else if ( materialID > 8 )
		{
			colour.R( materialID*16 );
			colour.G( 0 );
			colour.B( 0 );
			colour.A( m_alpha );
		}
	}
	return colour;
}

void CVolumetricData::renderRegionImmediateMode(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, unsigned int uLodLevel )
{
	const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = mesh.getVertices();
	const std::vector<uint32_t>& vecIndices = mesh.getIndices();

	int beginIndex = mesh.m_vecLodRecords[uLodLevel].beginIndex;
	int endIndex = mesh.m_vecLodRecords[uLodLevel].endIndex;

	glBegin(GL_TRIANGLES);
	//for(vector<PolyVox::uint32_t>::const_iterator iterIndex = vecIndices.begin(); iterIndex != vecIndices.end(); ++iterIndex)
	for(int index = beginIndex; index < endIndex; ++index)
	{
		const PolyVox::PositionMaterialNormal& vertex = vecVertices[vecIndices[index]];
		const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(mesh.m_Region.getLowerCorner());

		uint8_t material = static_cast<uint8_t>(vertex.getMaterial() + 0.5);

		CRGBA colour = convertMaterialIDToColour( material );

		glColor3ub(colour.R(), colour.G(), colour.B());
		//glColor4b( colour.red, colour.green, colour.blue, colour.alpha );
		glNormal3f(vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());
		glVertex3f(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());
	}
	glEnd();
}

#include <QOpenGLFunctions_1_5>

OpenGLSurfaceMesh CVolumetricData::BuildOpenGLSurfaceMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh)
{
	//Represents our filled in OpenGL vertex and index buffer objects.
	OpenGLSurfaceMesh result;
    
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    QOpenGLFunctions_1_5 *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_1_5>();
    
	//The source
	result.sourceMesh = &mesh;

	//Convienient access to the vertices and indices
	const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = mesh.getVertices();
	const std::vector<uint32_t>& vecIndices = mesh.getIndices();

	//If we have any indices...
	if(!vecIndices.empty())
	{
		//Create an OpenGL index buffer
		f->glGenBuffers(1, &result.indexBuffer);
		f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.indexBuffer);

		//Get a pointer to the first index
		GLvoid* pIndices = (GLvoid*)(&(vecIndices[0]));		

		//Fill the OpenGL index buffer with our data. 
		f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), pIndices, GL_STATIC_DRAW);
	}

	result.noOfIndices = vecIndices.size();

	f->glGenBuffers(1, &result.vertexBuffer);
	f->glBindBuffer(GL_ARRAY_BUFFER, result.vertexBuffer);
	f->glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(GLfloat) * 9, 0, GL_STATIC_DRAW);
	GLfloat* ptr = (GLfloat*)f->glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	for(std::vector<PolyVox::PositionMaterialNormal>::const_iterator iterVertex = vecVertices.begin(); iterVertex != vecVertices.end(); ++iterVertex)
	{
		const PolyVox::PositionMaterialNormal& vertex = *iterVertex;
		const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(mesh.m_Region.getLowerCorner());

		*ptr = v3dFinalVertexPos.getX();
		ptr++;
		*ptr = v3dFinalVertexPos.getY();
		ptr++;
		*ptr = v3dFinalVertexPos.getZ();
		ptr++;

		*ptr = vertex.getNormal().getX();
		ptr++;
		*ptr = vertex.getNormal().getY();
		ptr++;
		*ptr = vertex.getNormal().getZ();
		ptr++;

		uint8_t material = static_cast<uint8_t>(vertex.getMaterial() + 0.5);

		CRGBA colour = convertMaterialIDToColour(material);

		*ptr = colour.fR();
		ptr++;
		*ptr = colour.fG();
		ptr++;
		*ptr = colour.fB();
		ptr++;
	}

	f->glUnmapBuffer(GL_ARRAY_BUFFER);

	return result;
}

void CVolumetricData::renderRegionVertexBufferObject(const OpenGLSurfaceMesh& openGLSurfaceMesh, unsigned int uLodLevel)
{
	int beginIndex = openGLSurfaceMesh.sourceMesh->m_vecLodRecords[uLodLevel].beginIndex;
	int endIndex = openGLSurfaceMesh.sourceMesh->m_vecLodRecords[uLodLevel].endIndex;
    
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    QOpenGLFunctions_1_5 *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_1_5>();
    
	f->glBindBuffer(GL_ARRAY_BUFFER, openGLSurfaceMesh.vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 36, 0);
	glNormalPointer(GL_FLOAT, 36, (GLvoid*)12);
	glColorPointer(3, GL_FLOAT, 36, (GLvoid*)24);

	f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLSurfaceMesh.indexBuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//glDrawElements(GL_TRIANGLES, openGLSurfaceMesh.noOfIndices, GL_UNSIGNED_INT, 0);
	f->glDrawRangeElements(GL_TRIANGLES, beginIndex, endIndex-1, endIndex - beginIndex,/* openGLSurfaceMesh.noOfIndices,*/ GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CVolumetricData::createSphereInVolume( CVolumetricData::VolumeType& volData, float fRadius, uint8_t uValue )
{
	//This vector hold the position of the center of the volume
	PolyVox::Vector3DInt32 v3dVolCenter = (volData.getEnclosingRegion().getUpperCorner() - volData.getEnclosingRegion().getLowerCorner()) / static_cast<int32_t>(2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				PolyVox::Vector3DInt32 v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				double fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center
				//then we make it solid, otherwise we make it empty space.
				if(fDistToCenter <= fRadius)
				{
					volData.setVoxelAt(x,y,z, CVolumetricData::VoxelType(uValue, uValue > 0 ? CVolumetricData::VoxelType::getMaxDensity() : CVolumetricData::VoxelType::getMinDensity()));
				}
			}
		}
	}
}

void CVolumetricData::createCubeInVolume( CVolumetricData::VolumeType& volData, PolyVox::Vector3DInt32 lowerCorner, PolyVox::Vector3DInt32 upperCorner, uint8_t uValue)
{
	uint8_t maxDen = CVolumetricData::VoxelType::getMaxDensity();
	uint8_t minDen = CVolumetricData::VoxelType::getMinDensity();

	//This three-level for loop iterates over every voxel between the specified corners
	for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
	{
		for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
		{
			for (int x = lowerCorner.getX() ; x <= upperCorner.getX(); x++)
			{
				volData.setVoxelAt(x,y,z, CVolumetricData::VoxelType(uValue, uValue > 0 ? maxDen : minDen));
			}
		}
	}
}