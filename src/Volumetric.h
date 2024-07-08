#pragma once
#include "Object.h"
#include <QtCore/QString>
#include <vector>

struct SliceMetadata
{
	float image_position_patient[3] = { 0.0f, 0.0f, 0.0f };
	float slice_location = 0.0f;
	float pixel_spacing[2] = { 1.0f, 1.0f };
	float slice_thickness = 1.0f;
	float slice_distance = 1.0f;
	float gantry_detector_tilt = 0.0f;
	float rescale_slope = 1.0f;
	float rescale_intercept = 0.0f;

	//QString toString() {
	//	txt = f"SliceMetadata"
	//		txt += f"( gantry_detector_tilt = {self.gantry_detector_tilt}"
	//		txt += f", slice_thickness = {self.slice_thickness}"
	//		txt += f", slice_location = {self.slice_location}"
	//		txt += f", pixel_spacing = {self.pixel_spacing}"
	//		txt += f", image_position_patient = {self.image_position_patient} )"
	//		return txt
	//}
};

class QOpenGLShaderProgram;
class QOpenGLFunctions;

class CPointCloud;
class CMesh;

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class DPVISION_EXPORT Volumetric : public CObject
{
public:
	typedef float VoxelType;
	typedef std::vector<VoxelType> SliceDataType;
	typedef std::vector<SliceDataType> VolumeDataType;
	typedef SliceMetadata SliceMetadata;
	typedef enum { XY, YZ, ZX } Layer;

	VolumeDataType m_volume;
	int i = 0;
	GLuint vbo = 0;
	//QOpenGLVertexArrayObject vao;
	//QOpenGLShaderProgram* shader_program = nullptr;
	GLuint shader_program = 0;
	VoxelType m_minVal = 0.0f;
	VoxelType m_maxVal = 1.0f;
	VoxelType m_minDisplWin = 0.0f;
	VoxelType m_maxDisplWin = 1.0f;
	bool m_fastDraw = true;
	bool m_renderBoxes = false;
	std::vector<SliceMetadata> metadata;
	int m_minSlice = 0;
	int m_maxSlice = 0;
	int m_minRow = 0;
	int m_maxRow = 0;
	int m_minColumn = 0;
	int m_maxColumn = 0;
	float m_filters[7][3] = {
		{0, -100, 799},
		{0, -9999, 99999},
		{0, -9999, 99999},
		{0, -9999, 4000},
		{0, -9999, 99999},
		{0, -9999, 99999},
		{0, 800, 4095} };

	float m_fcolors[7][3] = {
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
		{1.0, 1.0, 0.0},
		{0.0, 1.0, 1.0},
		{1.0, 0.0, 1.0},
		{1.0, 1.0, 1.0} };

	int m_shape[3] = { 0, 0, 0 };

	Volumetric(CBaseObject* p = nullptr):CObject(p) {};
	~Volumetric() {};

	virtual inline int type() override { return CObject::Type::VOLUMETRIC_NEW; };
	virtual std::wstring infoRow() override;
	virtual Volumetric* getCopy() override;

	//GLuint compile_shader(QOpenGLFunctions* functions, QString source, GLenum shader_type);

	void remove_shader_program();

	bool create_program(QOpenGLFunctions* f);
	
	virtual void renderSelf() override;

	CPointCloud* sift_cloud( int nfeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6, int factor = 1 );
	CMesh* marching_cube(int factor = 1);
	CMesh* marching_tetrahedron(int factor = 1);

	bool getSlice(int nr, Volumetric::Layer layer, Volumetric::SliceDataType* slice, int* cols, int* rows);

	Volumetric* getRotatedVol(Volumetric::Layer dir);

	QImage getLayerAsImage(int nr, Volumetric::Layer layer);

	CPoint3d getVoxelSize();

	CPoint3d getOrigin();

	static Volumetric* create(int layers = 256, int rows = 256, int columns = 256);
	void adjustMinMax(bool calc_color = true);
	void adjustMinMaxColor(VoxelType color);
	void drawBox(CPoint3i origin, CPoint3i size, VoxelType color);


	static Volumetric* scal1(Volumetric*, Volumetric*);
};

