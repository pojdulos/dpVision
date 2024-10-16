#include "stdafx.h"

#include <Windows.h>
#include <GL\GL.h>

#include "Volumetric.h"
#include "Volumetric_shaders.h"

#include <omp.h>

extern bool mouse_key_pressed;

template <typename T> T Volumetric::clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}


std::wstring Volumetric::infoRow()
{
	float vsizeX = metadata[1].pixel_spacing[0];
	float vsizeY = metadata[1].pixel_spacing[1];
	float vsizeZ = metadata[1].slice_distance;

	QString info("Volumetric TK\n");

	info += QString("Volume size:\n%1 x %2 x %3").arg(m_columns).arg(m_rows).arg(m_layers);
	info += QString("\n(%1mm x %2mm x %3mm)").arg(vsizeX * m_columns).arg(vsizeY * m_rows).arg(vsizeZ * m_layers);

	info += QString("\nCurently displayed:\ncols[%1-%2]\nrows[%3-%4]\nslices[%5-%6]\n\n").arg(m_minColumn).arg(m_maxColumn).arg(m_minRow).arg(m_maxRow).arg(m_minSlice).arg(m_maxSlice);

	info += QString("Value range: %1 - %2\n").arg(this->m_minVal).arg(this->m_maxVal);
	info += QString("Displayed values: %1 - %2\n").arg(this->m_minDisplWin).arg(this->m_maxDisplWin);



	//info += L"voxelSize: " + std::to_wstring(kostka.m_voxelSize.x) + L", " + std::to_wstring(kostka.m_voxelSize.y) + L", " + std::to_wstring(kostka.m_voxelSize.z) + L"\n";
	//info += L"gantra: " + std::to_wstring(m_alpha) + L"\n";

	return info.toStdWString();
}


Volumetric* Volumetric::getCopy()
{	
	int new_Layers = 1 + this->m_maxSlice - this->m_minSlice;
	int new_Rows = 1 + this->m_maxRow - this->m_minRow;
	int new_Columns = 1 + this->m_maxColumn - this->m_minColumn;
	
	Volumetric* volum = create(new_Layers, new_Rows, new_Columns );

	if (volum) {
#pragma omp parallel for
		for (int l = 0; l < new_Layers; l++)
		{
			for (int r = 0; r < new_Rows; r++)
				for (int c = 0; c < new_Columns; c++)
				{
					VoxelType value = m_data[l + m_minSlice][(r + m_minRow) * m_columns + c + m_minColumn];
					volum->m_data[l][r * new_Columns + c] = ((value >= m_minDisplWin) && (value <= m_maxDisplWin)) ? value : 0;
				}
			
			volum->metadata[l] = this->metadata[l+m_minSlice];
			volum->metadata[l].image_position_patient[0] += volum->metadata[l].pixel_spacing[0] * m_minColumn;
			volum->metadata[l].image_position_patient[1] += volum->metadata[l].pixel_spacing[1] * m_minRow;
		}

		volum->adjustMinMax();
	}

	return volum;
}


#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>

/*
bool Volumetric::create_program() {
	// Tworzenie programu shader�w
	this->shader_program = new QOpenGLShaderProgram;

	// Inicjalizacja i konfiguracja shader�w

	QOpenGLShader* vertex_shader = new QOpenGLShader(QOpenGLShader::Vertex);
	if (vertex_shader->compileSourceCode(vertex_shader_code)) {
		this->shader_program->addShader(vertex_shader);
	}
	else {
		qInfo() << "BLAD KOMPILACJI SHADERA WIERZCHO�K�W" << Qt::endl;
	}


	QOpenGLShader* geometry_shader = new QOpenGLShader(QOpenGLShader::Geometry);
	bool success = false;
	if (this->m_renderBoxes)
		success = geometry_shader->compileSourceCode(geometry_shader_code_boxes);
	else
		success = geometry_shader->compileSourceCode(geometry_shader_code);

	if (success) {
		this->shader_program->addShader(geometry_shader);
	}
	else {
		qInfo() << "BLAD KOMPILACJI SHADERA GEOMETRII" << Qt::endl;
	}

	QOpenGLShader* fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment);
	if (fragment_shader->compileSourceCode(fragment_shader_code)) {
		this->shader_program->addShader(fragment_shader);
	}
	else {
		qInfo() << "BLAD KOMPILACJI SHADERA FRAGMENT�W" << Qt::endl;
	}

	// Linkowanie programu shader�w po do��czeniu wszystkich shader�w
	return (this->shader_program->link());
}
*/

void checkGLError(const char *label, const char *txt="") {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		qInfo() << "OpenGL error: " << err << " after " << label << Qt::endl;
		qInfo() << txt << Qt::endl;
	}
}

#include <exception>

GLuint compile_shader(QOpenGLFunctions* f, const char* source, GLenum shader_type) {
	GLuint shader = f->glCreateShader(shader_type);

	f->glShaderSource(shader, 1, &source, NULL);
	f->glCompileShader(shader);

	// Sprawdzenie, czy kompilacja si� powiod�a
	GLint success;
	f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

//	checkGLError("compile_shaders",source);

	if (! success)
	{
		char info_log[512];
		GLsizei len;
		f->glGetShaderInfoLog(shader, 512, &len, info_log);
		qInfo() << "ERROR::SHADER::COMPILATION_FAILED\n" <<info_log << Qt::endl;
		f->glDeleteShader(shader);
		//throw std::exception("Shader compilation failed");
		throw "Shader compilation failed";
	}


	return shader;
}

void Volumetric::remove_shader_program() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glDeleteProgram(this->shader_program);
	this->shader_program = 0;
}

bool Volumetric::create_program(QOpenGLFunctions* f) {
	//QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	GLuint vertex_shader = compile_shader(f, vertex_shader_code, GL_VERTEX_SHADER);

	GLuint geometry_shader;
	if (this->m_renderBoxes)
		geometry_shader = compile_shader(f, geometry_shader_code_boxes, GL_GEOMETRY_SHADER);
	else
		geometry_shader = compile_shader(f, geometry_shader_code, GL_GEOMETRY_SHADER);

	GLuint fragment_shader = compile_shader(f, fragment_shader_code, GL_FRAGMENT_SHADER);

	this->shader_program = f->glCreateProgram();

	f->glAttachShader(this->shader_program, vertex_shader);
	f->glAttachShader(this->shader_program, geometry_shader);
	f->glAttachShader(this->shader_program, fragment_shader);

	f->glLinkProgram(this->shader_program);

	// Sprawdzanie, czy program zosta� powi�zany poprawnie
	GLint success;
	f->glGetProgramiv(this->shader_program, GL_LINK_STATUS, &success);
	
	if (! success)
	{
		qInfo() << "Error linking shaders" << Qt::endl;
	}

	f->glDeleteShader(vertex_shader);
	f->glDeleteShader(geometry_shader);
	f->glDeleteShader(fragment_shader);

	return true;
}


/*
void Volumetric::renderSelf() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	//f->glEnable(GL_PROGRAM_POINT_SIZE);

	if (this->shader_program == nullptr)
		if (!this->create_program())
			qInfo() << "BLAD LINKOWANIA PROGRAMU SHADER�W" << Qt::endl;


	// U�ywanie programu shader�w
	if (! this->shader_program->bind() )
		qInfo() << "BLAD BINDOWANIA PROGRAMU SHADER�W" << Qt::endl;
	

	// Konfiguracja atrybut�w wierzcho�ka
	if (!this->vao.isCreated())
		this->vao.create();

	this->vao.bind();

	if (!this->vbo.isCreated())
		this->vbo.create();

	this->vbo.bind();

	// Pobieranie macierzy modelview i przekazywanie jej do shadera
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	QMatrix4x4 mvMatrix(modelview,4,4);
	this->shader_program->setUniformValue("modelviewMatrix", mvMatrix);


	float projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	QMatrix4x4 projMatrix(projection, 4, 4);
	this->shader_program->setUniformValue("projectionMatrix", projMatrix);


	this->shader_program->setUniformValue("minColor", this->m_minDisplWin);
	this->shader_program->setUniformValue("maxColor", this->m_maxDisplWin);
	this->shader_program->setUniformValueArray("f", (float*)this->m_filters, 7, 3);
	this->shader_program->setUniformValueArray("fcolors", (float*)this->m_fcolors, 7, 3);

	int factor = 1;
	//if (this->m_fastDraw) // || AP.mouse_key_pressed )
	//{
	//	if (this->m_volume.size() < 1536)
	//		factor = 4;
	//	else
	//		factor = 8;
	//}

	this->shader_program->setUniformValue("factor", factor);
	
	int first_slice = factor * int(this->m_minSlice / factor);
	int first_row = factor * int(this->m_minRow / factor);
	int first_column = factor * int(this->m_minColumn / factor);

	int small_shape[3] = { this->m_shape[0] / factor, this->m_shape[1] / factor, this->m_shape[2] / factor };

	//std::vector<std::vector<float>> subvolume;
	//for (auto layer = m_volume.begin(); layer < m_volume.end(); layer += factor) {
	//	std::vector<float> slice;

	//	for (auto valptr = layer->begin(); valptr < layer->end(); valptr += factor) {
	//			slice.push_back(*valptr);
	//	}
	//	
	//	subvolume.push_back(slice);
	//}
	auto& subvolume = this->m_volume;

	this->shader_program->setUniformValue("sizeX", small_shape[2]);
	this->shader_program->setUniformValue("sizeY", small_shape[1]);

	this->vbo.release();
	this->vao.release();

	for (int idx_in_subvolume = 0; idx_in_subvolume < small_shape[0]; idx_in_subvolume++)
	{
		this->vbo.bind();
		this->vao.bind();

		//qInfo() << idx_in_subvolume << Qt::endl;
		int true_index_of_slice = first_slice + idx_in_subvolume * factor;

		auto& metadata = this->metadata[true_index_of_slice];

		float imagePosition[3] = {
			metadata.image_position_patient[0] + metadata.pixel_spacing[0] * float(first_column),
			metadata.image_position_patient[1] + metadata.pixel_spacing[1] * float(first_row),
			metadata.image_position_patient[2]
		};

		float voxel_size[3] = {
			metadata.pixel_spacing[0],
			metadata.pixel_spacing[1],
			metadata.slice_thickness
		};

		this->shader_program->setUniformValueArray("imagePosition", (float*)imagePosition, 3, 1);
		this->shader_program->setUniformValueArray("voxelSize", (float*)voxel_size, 3, 1);

		auto &colors = subvolume[idx_in_subvolume];

		this->vbo.bind();

		this->vbo.allocate(colors.data(), colors.size() * sizeof(float));

		int color_location = shader_program->attributeLocation("vertexColor");
		shader_program->enableAttributeArray(color_location);
		shader_program->setAttributeBuffer(color_location, GL_FLOAT, 0, 1);

		this->vbo.release();
		this->vao.release();

		//=====================================================

		this->vao.bind();
		glDrawArrays(GL_POINTS, 0, colors.size());
		this->vao.release();
	}

	this->shader_program->release();
}
*/

Volumetric::SliceType clip2D(const Volumetric::SliceType& layer, int rows, int cols, int startRow, int endRow, int startCol, int endCol) {
	int newRows = 1 + endRow - startRow;
	int newCols = 1 + endCol - startCol;
	
	Volumetric::SliceType result(newRows, newCols);

#pragma omp parallel for
	for (int i = 0; i < newRows; ++i) {
		int orginalRowOffset = (startRow + i) * cols;
		int newRowOffset = i * newCols;
		for (int j = 0; j < newCols; ++j) {
			result[newRowOffset + j] = layer[orginalRowOffset + (startCol + j)];
		}
	}

	return result;
}

Volumetric::VolumeType clip3D(const Volumetric::VolumeType& volume, int layers, int rows, int cols, int startLayer, int endLayer, int startRow, int endRow, int startCol, int endCol) {
	int newLayers = 1 + endLayer - startLayer;
	int newRows = 1 + endRow - startRow;
	int newCols = 1 + endCol - startCol;

	Volumetric::VolumeType result(newLayers, Volumetric::SliceType(newRows, newCols));

	for (int k = 0; k < newLayers; ++k) {
		result[k] = clip2D(volume[startLayer + k], rows, cols, startRow, endRow, startCol, endCol);
	}

	return result;
}



// Funkcja przeskalowuj�ca jednowymiarowy wektor float�w reprezentuj�cy obraz 2D z okre�lonymi granicami
Volumetric::SliceType downsample2D(const Volumetric::SliceType& layer, int rows, int cols, int factor, int startRow, int endRow, int startCol, int endCol) {
	int newRows = 1 + (endRow - startRow) / factor;
	int newCols = 1 + (endCol - startCol) / factor;
	
	Volumetric::SliceType result(newRows, newCols);

#pragma omp parallel for
	for (int i = 0; i < newRows; ++i) {
		int orginalRowOffset = (startRow + i * factor) * cols;
		int newRowOffset = i * newCols;

		for (int j = 0; j < newCols; ++j) {
			result[newRowOffset + j] = layer[orginalRowOffset + (startCol + j * factor)];
		}
	}

	return result;
}

// Funkcja przeskalowuj�ca tr�jwymiarow� struktur� danych wolumetrycznych z okre�lonymi granicami
Volumetric::VolumeType downsample3D(const Volumetric::VolumeType& volume, int layers, int rows, int cols, int factor, int startLayer, int endLayer, int startRow, int endRow, int startCol, int endCol) {
	int newLayers = 1 + (endLayer - startLayer) / factor;
	int newRows = 1 + (endRow - startRow) / factor;
	int newCols = 1 + (endCol - startCol) / factor;

	Volumetric::VolumeType result(newLayers, Volumetric::SliceType(newRows, newCols));

	for (int k = 0; k < newLayers; ++k) {
		result[k] = downsample2D(volume[startLayer + k * factor], rows, cols, factor, startRow, endRow, startCol, endCol);
	}

	return result;
}



void Volumetric::renderSelf() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	f->glEnable(GL_PROGRAM_POINT_SIZE);

	if (this->shader_program == 0) {
		this->create_program(f);
	}

	f->glUseProgram(this->shader_program);

	if (this->vbo == 0)
		f->glGenBuffers(1, &this->vbo);

	f->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	// Ustawienie atrybutu aCol w location 0
	f->glEnableVertexAttribArray(0);
	f->glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Pobieranie macierzy modelview i projection
	float modelview[16];
	f->glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	GLint modelview_loc = f->glGetUniformLocation(this->shader_program, "modelviewMatrix");
	f->glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, modelview);

	float projection[16];
	f->glGetFloatv(GL_PROJECTION_MATRIX, projection);
	GLint projection_loc = f->glGetUniformLocation(this->shader_program, "projectionMatrix");
	f->glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection);

	// Ustawianie uniform�w
	GLint minColor_loc = f->glGetUniformLocation(this->shader_program, "minColor");
	f->glUniform1f(minColor_loc, float(this->m_minDisplWin));

	GLint maxColor_loc = f->glGetUniformLocation(this->shader_program, "maxColor");
	f->glUniform1f(maxColor_loc, float(this->m_maxDisplWin));

	GLint f_loc = f->glGetUniformLocation(this->shader_program, "f");
	f->glUniform3fv(f_loc, 7, (float*)this->m_filters);

	GLint fcolors_loc = f->glGetUniformLocation(this->shader_program, "fcolors");
	f->glUniform3fv(fcolors_loc, 7, (float*)this->m_fcolors);

	int factor = 1;
	if (this->m_fastDraw || mouse_key_pressed)
		factor = 4;


	GLint factor_loc = f->glGetUniformLocation(this->shader_program, "factor");
	f->glUniform1i(factor_loc, factor);


	int first_slice = this->m_minSlice;
	int first_row = this->m_minRow;
	int first_column = this->m_minColumn;

	int last_slice = this->m_maxSlice;
	int last_row = this->m_maxRow;
	int last_column = this->m_maxColumn;

	int small_shape[3];

	small_shape[0] = 1 + last_slice - first_slice;
	small_shape[1] = 1 + last_row - first_row;
	small_shape[2] = 1 + last_column - first_column;


	if (factor > 1) {
		first_slice = (this->m_minSlice / factor) * factor;
		first_row = (this->m_minRow / factor) * factor;
		first_column = (this->m_minColumn / factor) * factor;

		last_slice = (this->m_maxSlice / factor) * factor;
		last_row = (this->m_maxRow / factor) * factor;
		last_column = (this->m_maxColumn / factor) * factor;

		small_shape[0] = 1 + (last_slice - first_slice) / factor;
		small_shape[1] = 1 + (last_row - first_row) / factor;
		small_shape[2] = 1 + (last_column - first_column) / factor;
	}


	VolumeType subvolume;

	if (factor > 1) {
		subvolume = downsample3D(
			m_data,
			m_layers, m_rows, m_columns,
			factor,
			first_slice, last_slice,
			first_row, last_row,
			first_column, last_column);
	}
	else {
		subvolume = clip3D(
			m_data,
			m_layers, m_rows, m_columns,
			first_slice, last_slice,
			first_row, last_row,
			first_column, last_column);
	}

	GLint sizeX_loc = f->glGetUniformLocation(this->shader_program, "sizeX");
	f->glUniform1i(sizeX_loc, small_shape[2]);

	GLint sizeY_loc = f->glGetUniformLocation(this->shader_program, "sizeY");
	f->glUniform1i(sizeY_loc, small_shape[1]);

	int maxSubLayer = std::min(this->m_maxSlice / factor, small_shape[0]);

	for (int idx_in_subvolume = 0; idx_in_subvolume < small_shape[0]; idx_in_subvolume++) {
		int true_index_of_slice = first_slice + idx_in_subvolume * factor;

#if defined(_VoxelTypeFLOAT)
		SliceType colors = subvolume[idx_in_subvolume];
#else
		SliceType subvol = subvolume[idx_in_subvolume];
		std::vector<float> colors(subvol.size());
		std::transform(subvol.begin(), subvol.end(), colors.begin(), [](VoxelType value) { return static_cast<float>(value); });
#endif

		f->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

		SliceMetadata metadata = this->metadata[true_index_of_slice];

		float imagePosition[3] = {
			metadata.image_position_patient[0] + metadata.pixel_spacing[0] * float(first_column),
			metadata.image_position_patient[1] + metadata.pixel_spacing[1] * float(first_row),
			metadata.image_position_patient[2]
		};

		float voxel_size[3] = {
			metadata.pixel_spacing[0],
			metadata.pixel_spacing[1],
			metadata.slice_thickness
		};

		GLint imagePosition_loc = f->glGetUniformLocation(this->shader_program, "imagePosition");
		f->glUniform3fv(imagePosition_loc, 1, imagePosition);

		GLint voxelSize_loc = f->glGetUniformLocation(this->shader_program, "voxelSize");
		f->glUniform3fv(voxelSize_loc, 1, voxel_size);

		f->glDrawArrays(GL_POINTS, 0, colors.size());
	}

	f->glBindBuffer(GL_ARRAY_BUFFER, 0);
	f->glUseProgram(0);
	f->glDisable(GL_PROGRAM_POINT_SIZE);
}

CPoint3d Volumetric::getVoxelSize() {
	return CPoint3d(this->metadata[0].pixel_spacing[0], this->metadata[0].pixel_spacing[1], this->metadata[0].slice_distance);
}

CPoint3d Volumetric::getOrigin() {
	return CPoint3d(this->metadata[0].image_position_patient[0], this->metadata[0].image_position_patient[1], this->metadata[0].image_position_patient[2]);
}


Volumetric* Volumetric::create(int layers, int rows, int columns)
{
	Volumetric* volum = new Volumetric();

	if (volum) {
		try {
			volum->m_data.resize(layers);

			for (int i = 0; i < layers; i++) {
				volum->m_data[i].resize(rows * columns, 0.0f); // 100.0f * i / 10);
			}

		}
		catch (...) {
			qInfo() << "\nBRAK PAMIĘCI !!!" << Qt::endl;

			//CZYSZCZENIE 

			return nullptr;
		}

		volum->m_layers = layers;
		volum->m_rows = rows;
		volum->m_columns = columns;


		for (int i = 0; i < layers; i++) {
			SliceMetadata mdata;
			mdata.pixel_spacing[0] = mdata.pixel_spacing[1] = 1.0f;
			mdata.slice_thickness = mdata.slice_distance = 1.0f;
			mdata.image_position_patient[0] = mdata.image_position_patient[1] = 0.0f;
			mdata.image_position_patient[2] = float(i);

			volum->metadata.push_back(mdata);
		}

		volum->m_minVal = 0.0f;
		volum->m_maxVal = 0.0f; // 100.0f * layers / 10;
		
		volum->adjustMinMax(false);

		for (auto& filter : volum->m_filters) {
			filter[1] = std::max(filter[1], (float)volum->m_minDisplWin);
			filter[2] = std::min(filter[2], (float)volum->m_maxDisplWin);
		}

		return volum;
	}

	return nullptr;
}




void Volumetric::adjustMinMax(bool calc_color)
{
	if (calc_color && !this->empty()) {
		VoxelType min = *std::min_element(m_data[0].begin(), m_data[0].end());
		VoxelType max = *std::max_element(m_data[0].begin(), m_data[0].end());

		for (int i = 0; i < this->size(); i++) {
			min = std::min(min, *std::min_element(m_data[i].begin(), m_data[i].end()));
			max = std::max(max, *std::max_element(m_data[i].begin(), m_data[i].end()));
		}

		this->m_minVal = min;
		if (max > min)
			this->m_maxVal = max;
		else
			this->m_maxVal = min+1;
	}

	this->m_minDisplWin = this->m_minVal;
	this->m_maxDisplWin = this->m_maxVal;
	this->m_minSlice = 0;
	this->m_maxSlice = m_layers - 1;
	this->m_minRow = 0;
	this->m_maxRow = m_rows - 1;
	this->m_minColumn = 0;
	this->m_maxColumn = m_columns - 1;
}

void Volumetric::adjustMinMaxColor(VoxelType color) {
	this->m_minVal = std::min(this->m_minVal, color);
	this->m_maxVal = std::max(this->m_maxVal, color);
	this->m_minDisplWin = this->m_minVal;
	this->m_maxDisplWin = this->m_maxVal;
}

void Volumetric::drawBox(CPoint3i origin = { 0, 0, 0 }, CPoint3i size = { 10, 10, 10 }, VoxelType color = 1000.0f)
{
	int layers = this->m_layers;
	int rows = this->m_rows;
	int cols = this->m_columns;

	//int minX = std::max(0, origin[0] - size[0]);
	//int minY = std::max(0, origin[1] - size[1]);
	//int minZ = std::max(0, origin[2] - size[2]);

	int minX = std::max(0, origin[0]);
	int minY = std::max(0, origin[1]);
	int minZ = std::max(0, origin[2]);

	int maxX = std::min(layers, origin[0] + size[0] + 1);
	int maxY = std::min(rows, origin[1] + size[1] + 1);
	int maxZ = std::min(cols, origin[2] + size[2] + 1 );

#pragma omp parallel for
	for (int z=minZ; z<maxZ; z++)
		for (int y=minY; y<maxY; y++)
			for (int x = minX; x < maxX; x++)
				m_data[z][y * cols + x] = color;

	this->adjustMinMaxColor(color);
}

void Volumetric::drawSphere(CPoint3i origin = { 0, 0, 0 }, int radius = 1, VoxelType color = 1000.0f)
{
	int layers = m_layers;
	int rows = m_rows;
	int cols = m_columns;

	int z_min = std::max(0, origin.z - radius);
	int z_max = std::min(layers, origin.z + radius + 1);
	int y_min = std::max(0, origin.y - radius);
	int y_max = std::min(rows, origin.y + radius + 1);
	int x_min = std::max(0, origin.x - radius);
	int x_max = std::min(cols, origin.x + radius + 1);

	// Przygotowanie danych wejściowych
	int radius_squared = radius * radius;

#pragma omp parallel for
	for (int z = z_min; z < z_max; z++)
		for (int y = y_min; y < y_max; y++)
			for (int x = x_min; x < x_max; x++)
				if ((x - origin.x) * (x - origin.x) + (y - origin.y) * (y - origin.y) + (z - origin.z) * (z - origin.z) <= radius_squared)
					m_data[z][y * cols + x] = color;

	this->adjustMinMaxColor(color);
}



//									# def drawEllipsoid(self, origin = [0, 0, 0], radii = [1, 1, 1], color = 1000.) :
//									# 	origin_z, origin_y, origin_x = origin
//									# 	radius_z, radius_y, radius_x = radii
//									# 	for z in range(self.shape[0]) :
//									# 		for y in range(self.shape[1]) :
//									# 			for x in range(self.shape[2]) :
//# 				if ((x - origin_x) / radius_x) ** 2 + ((y - origin_y) / radius_y) ** 2 + ((z - origin_z) / radius_z) ** 2 <= 1:
//									# 					self.m_volume[z, y, x] = color
//# 	# self.m_volume.flush()
//									# 	self.adjustMinMax()
//


void Volumetric::drawCylinder(CPoint3i origin = { 0, 0, 0 }, int radius = 1, int height = 1, char axis = 'z', VoxelType color = 1000.0f)
{
	if (axis == 'z')
	{
#pragma omp parallel for
		for (int z = origin.z - height / 2; z < (origin.z + height / 2 + 1); z++)
			for (int y = origin.y - radius; y < (origin.y + radius + 1); y++)
				for (int x = origin.x - radius; x<(origin.x + radius + 1); x++)
					if ((y - origin.y) * (y - origin.y) + (x - origin.x) * (x - origin.x) <= radius * radius)
						m_data[z][y * m_columns + x] = color;
	}
	else if (axis == 'y') 
	{
#pragma omp parallel for
		for (int z = origin.z - radius; z < (origin.z + radius + 1); z++)
			for (int y = origin.y - height / 2; y < (origin.y + height / 2 + 1); y++)
				for (int x = origin.x - radius; x < (origin.x + radius + 1); x++)
					if ((z - origin.z) * (z - origin.z) + (x - origin.x) * (x - origin.x) <= radius * radius)
						m_data[z][y * m_columns + x] = color;
	}
	else if (axis == 'x')
	{
#pragma omp parallel for
		for (int z = origin.z - radius; z < (origin.z + radius + 1); z++)
			for (int y = origin.y - radius; y < (origin.y + radius + 1); y++)
				for (int x = origin.x - height / 2; x < (origin.x + height / 2 + 1); x++)
					if ((z - origin.z) * (z - origin.z) + (y - origin.y) * (y - origin.y) <= radius * radius)
						m_data[z][y * m_columns + x] = color;
	}
	else {
		;
		//			raise ValueError("Axis must be one of 'x', 'y', or 'z'.")
	}

	this->adjustMinMaxColor(color);
}


//void Volumetric::drawCylinder(CPoint3f p0, CPoint3f p1, float radius = 1, VoxelType color = 1000.0f)
//{
//	// Wektor różnicy
//	CVector3f d = p1 - p0;
//
//	// Długość cylindra (odległość między punktami)
//	float length = d.length();
//
//	// Normalizacja wektora
//	CVector3f n = d.getNormalized();
//
//	// Baza ortogonalna (potrzebna do transformacji do układu cylindra)
//	CVector3f up = (fabs(n.x) > 0.9f) ? CVector3f(0, 1, 0) : CVector3f(1, 0, 0);
//	CVector3f u = up.crossProduct(n).getNormalized();
//	CVector3f v = n.crossProduct(u).getNormalized();
//
//	// Rysowanie cylindra
//	for (int t = 0; t <= length; ++t) {
//		CPoint3f c = p0 + n * t;
//
//		for (int i = -radius; i <= radius; ++i) {
//			for (int j = -radius; j <= radius; ++j) {
//				if (i * i + j * j <= radius * radius) {
//					CPoint3f offset = u * i + v * j;
//					int px = static_cast<int>(c.x + offset.x);
//					int py = static_cast<int>(c.y + offset.y);
//					int pz = static_cast<int>(c.z + offset.z);
//
//					if (px >= 0 && px < m_columns && py >= 0 && py < m_rows && pz >= 0 && pz < m_layers)
//						m_data[pz][py * m_columns + px] = color;
//				}
//			}
//		}
//	}
//
//	this->adjustMinMaxColor(color);
//}


void Volumetric::drawCylinder(CPoint3f p0, CPoint3f p1, float radius = 1, VoxelType color = 1000.0f)
{
	// Wektor różnicy
	CVector3f d = p1 - p0;

	// Długość cylindra (odległość między punktami)
	float length = d.length();

	// Normalizacja wektora
	CVector3f n = d.getNormalized();

	// Iteracja przez wszystkie woksele w przestrzeni 3D
#pragma omp parallel for
	for (int z = 0; z < m_layers; ++z) {
		for (int y = 0; y < m_rows; ++y) {
			for (int x = 0; x < m_columns; ++x) {
				// Punkt (x, y, z) jako wektor
				CPoint3f p(x, y, z);

				// Wektor od punktu początkowego do aktualnego punktu
				CVector3f dP = p - p0;

				// Rzutowanie wektora dP na oś cylindra
				float proj = dP.dotProduct(n);

				// Sprawdzenie, czy punkt leży w zakresie wysokości cylindra
				if (proj >= 0 && proj <= length) {
					// Obliczenie najbliższego punktu na osi cylindra
					CPoint3f closestPoint = p0 + n * proj;

					// Odległość od osi cylindra
					float dist = (p - closestPoint).length();

					// Sprawdzenie, czy punkt leży wewnątrz promienia cylindra
					if (dist <= radius) {
						m_data[z][y * m_columns + x] = color;
					}
				}
			}
		}
	}

	this->adjustMinMaxColor(color);
}


void Volumetric::drawCylinder(CPoint3i origin, CVector3i vector, int radius = 1, VoxelType color = 1000.0f)
{
	CPoint3i end = { origin.x + vector.x, origin.y + vector.y, origin.z + vector.z };
	drawCylinder(origin, end, radius, color);
}


//				def set_pixel_size(self, image_x = 1.0, image_y = 1.0, slice_thickness = 1.0) :
//				for n, mdata in enumerate(self.metadata) :
//					mdata.pixel_spacing = [image_x, image_y]
//					mdata.slice_thickness = slice_thickness
//					mdata.slice_distance = slice_thickness
//					mdata.slice_location = slice_thickness * n
//					mdata.image_position_patient[2] = slice_thickness * n
//
//					def set_position(self, x = 0.0, y = 0.0, z = 0.0) :
//					for n, mdata in enumerate(self.metadata) :
//						mdata.image_position_patient[0] = x
//						mdata.image_position_patient[1] = y
//						mdata.image_position_patient[2] = z + mdata.slice_distance * n
//						mdata.slice_location = mdata.image_position_patient[2]
//
//						# def export(self, dir = "v:/test/", file_base = "image_", ext = ".png") :
//						# 	from PIL import Image
//# 	import os
//						# 	for i, layer in enumerate(self.m_volume) :
//# 		# stw�rz obraz
//						# 		img = Image.fromarray(layer.astype(np.uint8))
//
//# 		# Utw�rz nazw� pliku zgodnie z formatem "file_base_numer.ext"
//						# 		filename = os.path.join(dir, f"{file_base}{i:03}{ext}")
//
//# 		# Zapisz obraz na dysku
//						# 		img.save(filename)
//
//						def export(self, dir = "v:/test/", file_base = "image_", ext = ".dcm") :
//						if not os.path.exists(dir) :
//							os.makedirs(dir)
//
//							for i, layer in enumerate(self.m_volume) :
//								# Tworzenie obiektu DICOM
//								ds = pydicom.FileDataset(os.path.join(dir, f"{file_base}{i:03}{ext}"), {}, file_meta = None, preamble = b"\0" * 128)
//
//								# Ustawienie podstawowych atrybut�w DICOM
//								ds.PatientName = "Anonymous"
//								ds.PatientID = "123456"
//								ds.Modality = "MR"
//								ds.SeriesDescription = "generated with pyDpVision software"
//								ds.Rows, ds.Columns = layer.shape
//								ds.PixelSpacing = self.metadata[i].pixel_spacing
//								ds.BitsAllocated = 16  # Ustawiamy na 16 bit�w
//								ds.BitsStored = 16
//								ds.HighBit = 15
//								ds.PixelRepresentation = 1  # Ustawiamy na warto�� ze znakiem(signed)
//
//								# Przyk�adowe dodatkowe atrybuty DICOM
//								ds.ImagePositionPatient = self.metadata[i].image_position_patient
//								ds.RescaleIntercept = -1000.0  # Je�li potrzebne, ustawienie interceptu
//								ds.RescaleSlope = 1.0  # Je�li potrzebne, ustawienie nachylenia
//								ds.SliceThickness = self.metadata[i].slice_thickness
//								ds.SliceLocation = self.metadata[i].slice_location
//
//								ds.SamplesPerPixel = 1
//								ds.PhotometricInterpretation = 'MONOCHROME2'
//
//								ds.file_meta.TransferSyntaxUID = pydicom.uid.ImplicitVRLittleEndian
//
//								# Konwersja warstwy na format DICOM
//								# Zak�adaj�c, �e warstwa jest typu int lub float, mo�esz j� rzutowa� na int16
//								image = layer.astype(np.int16)
//								ds.PixelData = image.tobytes()
//
//								# Zapisanie pliku DICOM
//								ds.save_as(os.path.join(dir, f"{file_base}{i:03}{ext}"))




#include "Mesh.h"
#include "MeshMaker.h"
#include "AP.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
//#include <opencv2/core/types_c.h>
//#include "opencv2/core/utility.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

CPointCloud* Volumetric::sift_cloud(int nfeatures, int nOctaveLayers, double contrastThreshold, double edgeThreshold, double sigma, int factor) {
	//UI::MESSAGEBOX::information("NOT IMPLEMENTED YET");
	//return;

	CPointCloud* cloud = new CPointCloud();

	UI::PROGRESSBAR::init(this->m_minSlice, this->m_maxSlice, this->m_minSlice);
	UI::PROGRESSBAR::setText("Sift Cloud:");

	// Tworzenie obiektu SIFT z ustawionymi parametrami
	cv::Ptr<cv::SIFT> sift = cv::SIFT::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

	for (int idx_of_slice = this->m_minSlice; idx_of_slice <= this->m_maxSlice; idx_of_slice++)
	{
		UI::PROGRESSBAR::setValue(idx_of_slice);

		auto image = clip2D(m_data[idx_of_slice], m_rows, m_columns, this->m_minRow, this->m_maxRow, this->m_minColumn, this->m_maxColumn);

		float *pixel_spacing = this->metadata[idx_of_slice].pixel_spacing;

		float position[] = {
			this->metadata[idx_of_slice].image_position_patient[0] + pixel_spacing[0] * float(this->m_minColumn),
			this->metadata[idx_of_slice].image_position_patient[1] + pixel_spacing[1] * float(this->m_minRow),
			this->metadata[idx_of_slice].image_position_patient[2]
		};


		//qInfo() << "slice " << idx_of_slice << ": position = " << position << Qt::endl;

		for (auto& px : image)
		{
			px = std::min(std::max(this->m_minDisplWin, px), this->m_maxDisplWin);
		}

		cv::Mat mat(1 + this->m_maxRow - this->m_minRow, 1 + this->m_maxColumn - this->m_minColumn, CV_32FC1, image.data());

		// Normalizuj dane obrazu do zakresu 0 - 255
		cv::Mat obraz;
		cv::normalize(mat, obraz, 0, 255, cv::NORM_MINMAX, CV_8UC1);

		// Domy�lne ustawienie parametr�w dla algorytmu SIFT

		// Liczba kluczowych punkt�w do zachowania.Domy�lnie 0, co oznacza, �e nie ma limitu.
		//int nfeatures = 0;

		// Liczba warstw w ka�dej oktawie.Domy�lnie 3
		//int nOctaveLayers = 5;

		// Pr�g eliminacji kluczowych punkt�w o niskim kontra�cie.Domy�lnie 0.04
		//double contrastThreshold = 0.01;

		// Pr�g eliminacji kluczowych punkt�w na kraw�dziach.Domy�lnie 10
		//int edgeThreshold = 5;

		// Pocz�tkowa sigma dla Gaussowskiego rozmycia.Domy�lnie 1.6
		//double sigma = 1.6;

		// Tworzenie obiektu SIFT z ustawionymi parametrami
		//cv::Ptr<cv::SIFT> sift = cv::SIFT::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

		std::vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;

		try {
			sift->detectAndCompute(obraz, cv::noArray(), keypoints, descriptors);
		}
		catch (cv::Exception ex) {
			qInfo() << ex.msg.c_str() << Qt::endl;
		}
		// Wy�wietlanie kluczowych punkt�w na obrazie
		//cv::Mat outputImg;
		//cv::drawKeypoints(obraz, keypoints, outputImg);
		//cv::imshow("SIFT Keypoints", outputImg);
		//cv::waitKey(0);


		for (auto key : keypoints)
		{
			CVertex point(
				float(position[0]) + pixel_spacing[0] * key.pt.x,
				float(position[1]) + pixel_spacing[1] * key.pt.y,
				float(position[2]) );
			
			cloud->addVertex(point, CRGBA(1.0f, 1.0f, 0.0f, 1.0f) );
		}
	}
	
	UI::PROGRESSBAR::hide();

	return cloud;
}

CMesh* Volumetric::marching_cube(int factor) {
	Volumetric* volTK = this;

	int pMin = volTK->m_minDisplWin;
	int pMax = volTK->m_maxDisplWin;

	double div = factor;

	CMesh* mesh = new CMesh;

	//MeshMaker m(volTK, mesh, pMin + ((progi) ? 32768 : 0), pMax + ((progi) ? 32768 : 0));
	MeshMaker m(volTK, mesh, pMin, pMax);
	m.MarchingCube(div);

	std::map<CVertex, INDEX_TYPE> indexMap;

	CFace f;

	for (const auto& t : m.triangles)
	{
		for (int i = 0; i < 3; i++) {
			CVertex v(t.vertex[i]);

			std::map<CVertex, INDEX_TYPE>::iterator it = indexMap.find(v);

			if (it == indexMap.end())
			{
				f[i] = mesh->addVertex(v);
				indexMap[v] = f[i];
			}
			else
			{
				f[i] = it->second;
			}
		}

		mesh->faces().push_back(f);
	}

	indexMap.clear();

	CPoint3d scale = volTK->getVoxelSize();
	CPoint3d origin = volTK->getOrigin();

	for (auto& v : mesh->vertices())
	{
		v.scaleByVector(scale);
		v = origin + v;
	}

	QString label("c_" + QString::number(pMin) + "_" + QString::number(pMax) + "_" + QString::number(div));
	mesh->setLabel(label);
	mesh->setDescr("source data: " + volTK->getParent()->getLabel()
		+ "\nvolume size: " + QString::number(volTK->m_columns) + " x " + QString::number(volTK->m_rows) + " x " + QString::number(volTK->m_layers)
		+ "\nintensity range: " + QString::number(volTK->m_minDisplWin) + " - " + QString::number(volTK->m_maxDisplWin)
		+ "\n"
		+ "\nmethod: marching cube\nlower treshold: " + QString::number(pMin)
		+ "\nupper treshold: " + QString::number(pMax)
		+ "\nthinning factor: " + QString::number(div) + "\n"
	);

	return mesh;
}

CMesh* Volumetric::marching_tetrahedron(int factor) {
	Volumetric* volTK = this;

	int pMin = volTK->m_minDisplWin;
	int pMax = volTK->m_maxDisplWin;

	double div = factor;

	CMesh* mesh = new CMesh;

	//MeshMaker m(volTK, mesh, pMin + ((progi) ? 32768 : 0), pMax + ((progi) ? 32768 : 0));
	MeshMaker m(volTK, mesh, pMin, pMax);
	m.MarchingTetrahedron(div);

	std::map<CVertex, INDEX_TYPE> indexMap;

	CFace f;

	for (const auto& t : m.triangles)
	{
		for (int i = 0; i < 3; i++) {
			CVertex v(t.vertex[i]);

			std::map<CVertex, INDEX_TYPE>::iterator it = indexMap.find(v);

			if (it == indexMap.end())
			{
				f[i] = mesh->addVertex(v);
				indexMap[v] = f[i];
			}
			else
			{
				f[i] = it->second;
			}
		}

		mesh->faces().push_back(f);
	}

	indexMap.clear();

	CPoint3d scale = volTK->getVoxelSize();
	CPoint3d origin = volTK->getOrigin();

	for (auto& v : mesh->vertices())
	{
		v.scaleByVector(scale);
		v = origin + v;
	}

	QString label("t_" + QString::number(pMin) + "_" + QString::number(pMax) + "_" + QString::number(div));
	mesh->setLabel(label);
	mesh->setDescr("source data: " + volTK->getParent()->getLabel()
		+ "\nvolume size: " + QString::number(volTK->m_columns) + " x " + QString::number(volTK->m_rows) + " x " + QString::number(volTK->m_layers)
		+ "\nintensity range: " + QString::number(volTK->m_minDisplWin) + " - " + QString::number(volTK->m_maxDisplWin)
		+ "\n"
		+ "\nmethod: marching tetrahedron\nlower treshold: " + QString::number(pMin)
		+ "\nupper treshold: " + QString::number(pMax)
		+ "\nthinning factor: " + QString::number(div) + "\n"
	);

	return mesh;
}

bool Volumetric::getSlice(int nr, Volumetric::LayerPlane layer, Volumetric::SliceType* slice)
{
	switch (layer)
	{
	case YZ:
		slice->setSize(m_rows, m_layers);
#pragma omp parallel for
		for (int row = 0; row < m_rows; row++)
			for (int col = 0; col < m_layers; col++)
				(*slice)[row * m_layers + col] = m_data[col][row * m_columns + nr];
		return true;
		break;
	case ZX:
		slice->setSize(m_layers, m_columns);
#pragma omp parallel for
		for (int row = 0; row < m_layers; row++)
			for (int col = 0; col < m_columns; col++)
				(*slice)[row * m_columns + col] = m_data[row][nr * m_columns + col];

		return true;
		break;
	case XY:
	default:
		slice->setSize(m_rows, m_columns);
		std::copy(m_data[nr].begin(), m_data[nr].end(), slice->begin());
		return true;
		break;
	}

	return false;
}


Volumetric* Volumetric::getRotatedVol(Volumetric::LayerPlane dir)
{
	int old_L = m_layers;
	int old_R = m_rows;
	int old_C = m_columns;

	if (dir == XY)
	{
		Volumetric* new_volume = Volumetric::create(old_L, old_R, old_C );
		for (int l = 0; l < old_L; l++)
			std::copy(m_data[l].begin(), m_data[l].end(), (*new_volume)[l].begin());
		return new_volume;
	} 
	else //if (dir == ZX)
	{
		int new_L = old_R;
		int new_R = old_C;
		int new_C = old_L;

		Volumetric* new_volume = Volumetric::create(new_L, new_R, new_C);

		for (int l = 0; l < old_L; l++)
			for (int r = 0; r < old_R; r++)
				for (int c = 0; c < old_C; c++)
				{
					VoxelType value = m_data[l][r * old_C + c];

					(*new_volume)[r][c * new_C + l] = value;
				}
		return new_volume;
	}
	//else { // YZ
	//	int new_L = old_R;
	//	int new_R = old_C;
	//	int new_C = old_L;
	//}
}

QImage Volumetric::getLayerAsImage(int nr, Volumetric::LayerPlane layer, bool tresh)
{
	Volumetric::SliceType slice;

	if ( getSlice(nr, layer, &slice) )
	{
		unsigned int rows = slice.rows();
		unsigned int cols = slice.columns();

		QImage::Format imgFormat = QImage::Format::Format_Grayscale16;
		WORD maxDisplVal = 0xffff;

		QImage image = QImage(cols, rows, imgFormat);

		VoxelType _min = tresh ? m_minDisplWin : m_minVal;
		VoxelType _max = tresh ? m_maxDisplWin : m_maxVal;

		for (qint32 y = 0; y < rows; y++)
		{
			uint16_t* line = (uint16_t*) image.scanLine(y);
			for (qint32 x = 0; x < cols; x++)
			{
				VoxelType val = slice.at(y, x);
				if (tresh)
				{
					if (val < m_minDisplWin) val = m_minDisplWin;
					if (val > m_maxDisplWin) val = m_maxDisplWin;
				}

				WORD piksel = static_cast<WORD>((double(val - _min) / double(_max - _min)) * double(maxDisplVal));
				line[x] = piksel;
			}
		}
		return image;
	}

	return QImage();
}

QImage Volumetric::getLayerAsArgbImage(int nr, Volumetric::LayerPlane layer, bool tresh)
{
	Volumetric::SliceType slice;

	if (getSlice(nr, layer, &slice))
	{
		unsigned int rows = slice.rows();
		unsigned int cols = slice.columns();

		QImage::Format imgFormat = QImage::Format::Format_ARGB32;
		//QRgb maxDisplVal = qRgba(255, 255, 255, 255);

		QImage image = QImage(cols, rows, imgFormat);

		VoxelType _min = tresh ? m_minDisplWin : m_minVal;
		VoxelType _max = tresh ? m_maxDisplWin : m_maxVal;

		for (qint32 y = 0; y < rows; y++)
		{
			QRgb* line = (QRgb*)image.scanLine(y);
			for (qint32 x = 0; x < cols; x++)
			{
				VoxelType val = slice.at(y, x);
				if (tresh)
				{
					if (val < m_minDisplWin) val = m_minDisplWin;
					if (val > m_maxDisplWin) val = m_maxDisplWin;
				}

				//QRgb piksel = static_cast<QRgb>((double(val - _min) / double(_max - _min)) * double(maxDisplVal));

				int b = (double(val - _min) / double(_max - _min)) * 255;
				QRgb piksel = qRgb(b, b, b);
				line[x] = piksel;
			}
		}
		return image;
	}

	return QImage();
}


CPoint3d Volumetric::realXYZ(CPoint3d pt)
{
	float* pos = metadata[pt.z].image_position_patient;
	float* vSxy = metadata[pt.z].pixel_spacing;
	float vSz = metadata[pt.z].slice_distance;

	float x = pos[0] + pt.x * vSxy[0];
	float y = pos[1] + pt.y * vSxy[1];
	float z = pos[2] + pt.z * vSz;

	return CPoint3d(x, y, z);
}

CPoint3d Volumetric::realXYZ(int x, int y, int z)
{
	float* pos = metadata[z].image_position_patient;
	float* vSxy = metadata[z].pixel_spacing;
	float vSz = metadata[z].slice_distance;

	float xx = pos[0] + vSxy[0] * x;
	float yy = pos[1] + vSxy[1] * y;
	float zz = pos[2];

	return CPoint3d(xx, yy, zz);
}

Volumetric* Volumetric::scal1(Volumetric* src1, Volumetric* src2)
{
	// TO JEST NIEGOTOWE !!!!

	bool scalenie_jest_mozliwe = true;

	Volumetric::SliceMetadata md1 = src1->metadata[1];
	Volumetric::SliceMetadata md2 = src2->metadata[1];

	scalenie_jest_mozliwe = scalenie_jest_mozliwe && (md1.pixel_spacing[0] == md2.pixel_spacing[0]);
	scalenie_jest_mozliwe = scalenie_jest_mozliwe && (md1.pixel_spacing[1] == md2.pixel_spacing[1]);
	scalenie_jest_mozliwe = scalenie_jest_mozliwe && (md1.slice_distance == md2.slice_distance);

	//if (scalenie_jest_mozliwe)
	//{
	//	Volumetric* volum = new Volumetric();

	//	if (volum) {

	//	}

	//	return volum;
	//}
		
	return nullptr;
}





#include <QtGui/QImage>
#include <vector>
#include <cmath>
#include  <algorithm>

// Funkcja obliczająca odległość punktu od płaszczyzny
float Volumetric::pointToPlaneDistance(const CPoint3f& point, const CPoint3f& normal, const CPoint3f& planePoint) {
	return (point.x - planePoint.x) * normal.x +
		(point.y - planePoint.y) * normal.y +
		(point.z - planePoint.z) * normal.z;
}

// Funkcja interpolująca wartość voxel
float Volumetric::interpolateVoxel(const Volumetric::VolumeType& volume, const CPoint3f& point) {
	// Przyjmujemy, że point.x, point.y, point.z mieszczą się w zakresie indeksów volume
	int x = static_cast<int>(std::floor(point.x));
	int y = static_cast<int>(std::floor(point.y));
	int z = static_cast<int>(std::floor(point.z));

	// Prosta interpolacja nearest neighbor, można zastosować bardziej zaawansowaną interpolację
	return volume[z].at(y, x);
}


// Funkcja generująca przekrój
QImage Volumetric::generateFreeViewSliceImage(const CVector3f& normal, const CPoint3f& planePoint, const CVector3f& free_up, int width, int height, float scale, bool tresh)
{
	QImage::Format imgFormat = QImage::Format::Format_Grayscale16;
	WORD maxDisplVal = 0xffff;

	QImage image(width, height, imgFormat);

	VoxelType _min = tresh ? m_minDisplWin : m_minVal;
	VoxelType _max = tresh ? m_maxDisplWin : m_maxVal;

	// Oblicz wektor "right" jako iloczyn wektorowy wektora "free_up" i "normal"
	CVector3f right = free_up.crossProduct(normal).getNormalized();

	for (int y = 0; y < height; ++y)
	{
		uint16_t* line = (uint16_t*)image.scanLine(y);

		for (int x = 0; x < width; ++x)
		{
			// Przekształcanie współrzędnych pikseli obrazu na współrzędne przestrzenne
			CPoint3f point = planePoint;

			// Przesunięcie wzdłuż "right" i "free_up" o odpowiednie skale
			point.x += (x - width / 2.0f) * scale * right.x + (y - height / 2.0f) * scale * free_up.x;
			point.y += (x - width / 2.0f) * scale * right.y + (y - height / 2.0f) * scale * free_up.y;
			point.z += (x - width / 2.0f) * scale * right.z + (y - height / 2.0f) * scale * free_up.z;

			// Rzutowanie punktu na płaszczyznę przekroju
			float distance = pointToPlaneDistance(point, normal, planePoint);
			point.x -= distance * normal.x;
			point.y -= distance * normal.y;
			point.z -= distance * normal.z;

			if (point.x >= 0 && point.x < m_columns && point.y >= 0 && point.y < m_rows && point.z >= 0 && point.z < m_layers)
			{
				VoxelType val = interpolateVoxel(m_data, point);

				if (tresh)
				{
					if (val < m_minDisplWin) val = m_minDisplWin;
					if (val > m_maxDisplWin) val = m_maxDisplWin;
				}

				WORD piksel = static_cast<WORD>((double(val - _min) / double(_max - _min)) * double(maxDisplVal));
				line[x] = piksel;
			}
			else
			{
				line[x] = static_cast<WORD>(0xf0ff);
			}
		}
	}

	return image;
}


// Funkcja generująca przekrój
//QImage Volumetric::generateFreeViewSliceImage( const CVector3f& normal, const CPoint3f& planePoint,	int width, int height, float scale, bool tresh)
//{
//	QImage::Format imgFormat = QImage::Format::Format_Grayscale16;
//	WORD maxDisplVal = 0xffff;
//
//	//QImage image(width, height, QImage::Format_Grayscale8);
//	QImage image(width, height, imgFormat);
//
//	VoxelType _min = tresh ? m_minDisplWin : m_minVal;
//	VoxelType _max = tresh ? m_maxDisplWin : m_maxVal;
//
//	for (int y = 0; y < height; ++y)
//	{
//		uint16_t* line = (uint16_t*)image.scanLine(y);
//
//		for (int x = 0; x < width; ++x)
//		{
//			// Przekształcanie współrzędnych pikseli obrazu na współrzędne przestrzenne
//			CPoint3f point;
//			point.x = planePoint.x + (x - width / 2.0) * scale;
//			point.y = planePoint.y + (y - height / 2.0) * scale;
//			point.z = planePoint.z;
//
//			// Rzutowanie punktu na płaszczyznę przekroju
//			float distance = pointToPlaneDistance(point, normal, planePoint);
//			point.x -= distance * normal.x;
//			point.y -= distance * normal.y;
//			point.z -= distance * normal.z;
//
//			if (point.x >= 0 && point.x < m_columns && point.y >= 0 && point.y < m_rows && point.z >= 0 && point.z < m_layers)
//			{
//				VoxelType val = interpolateVoxel(m_data, point);
//				
//				if (tresh)
//				{
//					if (val < m_minDisplWin) val = m_minDisplWin;
//					if (val > m_maxDisplWin) val = m_maxDisplWin;
//				}
//
//				WORD piksel = static_cast<WORD>(((val - _min) / (_max - _min)) * double(maxDisplVal));
//
//				line[x] = piksel;
//			}
//			else
//			{
//				line[x] = static_cast<WORD>(0xf0ff);
//			}
//		}
//	}
//
//	return image;
//}




QImage Volumetric::getRTGasImage(Volumetric::LayerPlane plane, bool tresh)
{
	int count = 0;
	int rows = 0;
	int cols = 0;

	WORD maxDisplVal = 0xffff;
	QImage::Format imgFormat = QImage::Format::Format_Grayscale16;

	if (plane == LayerPlane::XY) {
		count = m_layers;
		rows = m_rows;
		cols = m_columns;
	}
	else if (plane == LayerPlane::YZ) {
		count = m_columns;
		rows = m_rows;
		cols = m_layers;
	}
	else if (plane == LayerPlane::ZX) {
		count = m_rows;
		rows = m_layers;
		cols = m_columns;
	}
	else
	{
		return QImage(cols, rows, imgFormat);
	}

	std::vector<std::vector<double>> accum;
	accum.resize(rows);
	for (auto& row : accum)	row.resize(cols, 0.0);

	Volumetric::SliceType slice;

	for (int i = 0; i < count; i++)
	{
		if (getSlice(i, plane, &slice))
		{
#pragma omp parallel for
			for (int row = 0; row < rows; row++)
				for (int col = 0; col < cols; col++)
				{
					VoxelType val = slice.at(row, col);
					if (tresh)
					{
						if (val < m_minDisplWin) val = m_minVal;
						if (val > m_maxDisplWin) val = m_maxVal;
					}
					accum[row][col] += val;
					//if ( tresh && (val >= m_minDisplWin) )//&& (val <= m_maxDisplWin) )
					//	accum[row][col] += val;
				}
		}
	}

	double _min = DBL_MAX;
	double _max = -DBL_MAX;
	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
		{
			double val = accum[row][col] / count;
			_min = std::min(_min, val);
			_max = std::max(_max, val);
			accum[row][col] = val;
		}

	QImage image = QImage(cols, rows, imgFormat);

	for (int row = 0; row < rows; row++)
	{
		uint16_t* line = (uint16_t*)image.scanLine(row);
#pragma omp parallel for
		for (int col = 0; col < cols; col++)
		{
			double val = accum[row][col];

			val = clamp(val, _min, _max);
			WORD piksel = static_cast<WORD>(((val - _min) / (_max - _min)) * double(maxDisplVal));
			line[col] = piksel;
		}
	}

	return image;
}




QImage Volumetric::generateFreeViewRtgImage(const CVector3f& normal, const CPoint3f& planePoint, int width, int height, float scale, bool tresh)
{
	QImage::Format imgFormat = QImage::Format::Format_Grayscale16;
	WORD maxDisplVal = 0xffff;

	//QImage image(width, height, QImage::Format_Grayscale8);
	QImage image(width, height, imgFormat);

	VoxelType _min = tresh ? m_minDisplWin : m_minVal;
	VoxelType _max = tresh ? m_maxDisplWin : m_maxVal;

	for (int y = 0; y < height; ++y)
	{
		uint16_t* line = (uint16_t*)image.scanLine(y);

		for (int x = 0; x < width; ++x)
		{
			// Przekształcanie współrzędnych pikseli obrazu na współrzędne przestrzenne
			CPoint3f point;
			point.x = planePoint.x + (x - width / 2.0) * scale;
			point.y = planePoint.y + (y - height / 2.0) * scale;
			point.z = planePoint.z;

			// Rzutowanie punktu na płaszczyznę przekroju
			float distance = pointToPlaneDistance(point, normal, planePoint);
			point.x -= distance * normal.x;
			point.y -= distance * normal.y;
			point.z -= distance * normal.z;

			if (point.x >= 0 && point.x < m_columns && point.y >= 0 && point.y < m_rows && point.z >= 0 && point.z < m_layers)
			{
				VoxelType val = interpolateVoxel(m_data, point);

				if (tresh)
				{
					if (val < m_minDisplWin) val = m_minDisplWin;
					if (val > m_maxDisplWin) val = m_maxDisplWin;
				}

				WORD piksel = static_cast<WORD>(((val - _min) / (_max - _min)) * double(maxDisplVal));

				line[x] = piksel;
			}
			else
			{
				line[x] = 0;// static_cast<WORD>(0xf0ff);
			}
		}
	}

	return image;
}
