#include "stdafx.h"

#include <Windows.h>
#include <GL\GL.h>

#include "Volumetric.h"
#include "Volumetric_shaders.h"


std::wstring Volumetric::infoRow()
{
	std::wstring info = L"Volumetric TK\n";
	info += L"data: " + std::to_wstring(this->m_minVal) + L" - " + std::to_wstring(this->m_maxVal) + L"\n";
	info += L"window: " + std::to_wstring(this->m_minDisplWin) + L" - " + std::to_wstring(this->m_maxDisplWin) + L"\n";

	info += L"range:\ncols[" + std::to_wstring(this->m_minColumn) + L"-" + std::to_wstring(this->m_maxColumn) + L"]\nrows[" + std::to_wstring(this->m_minRow) + L"-" + std::to_wstring(this->m_maxRow) + L"]\nslices[" + std::to_wstring(this->m_minSlice) + L"-" + std::to_wstring(this->m_maxSlice) + L"]\n\n";

	//info += L"voxelSize: " + std::to_wstring(kostka.m_voxelSize.x) + L", " + std::to_wstring(kostka.m_voxelSize.y) + L", " + std::to_wstring(kostka.m_voxelSize.z) + L"\n";
	//info += L"gantra: " + std::to_wstring(m_alpha) + L"\n";

	return info;
}


Volumetric* Volumetric::getCopy()
{	
	int new_Layers = 1 + this->m_maxSlice - this->m_minSlice;
	int new_Rows = 1 + this->m_maxRow - this->m_minRow;
	int new_Columns = 1 + this->m_maxColumn - this->m_minColumn;
	
	Volumetric* volum = create(new_Layers, new_Rows, new_Columns );

	if (volum) {
		for (int l = 0; l < new_Layers; l++)
		{
			for (int r = 0; r < new_Rows; r++)
				for (int c = 0; c < new_Columns; c++)
				{
					Volumetric::VoxelType value = this->m_volume[l + m_minSlice][(r + m_minRow) * m_shape[2] + c + m_minColumn];
					volum->m_volume[l][r * new_Columns + c] = ((value >= m_minDisplWin) && (value <= m_maxDisplWin)) ? value : 0;
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

Volumetric::SliceDataType clip2D(const Volumetric::SliceDataType& layer, int rows, int cols, int startRow, int endRow, int startCol, int endCol) {
	int newRows = 1 + endRow - startRow;
	int newCols = 1 + endCol - startCol;
	
	Volumetric::SliceDataType result(newRows * newCols);

	for (int i = 0; i < newRows; ++i) {
		int orginalRowOffset = (startRow + i) * cols;
		int newRowOffset = i * newCols;
		for (int j = 0; j < newCols; ++j) {
			result[newRowOffset + j] = layer[orginalRowOffset + (startCol + j)];
		}
	}

	return result;
}

Volumetric::VolumeDataType clip3D(const Volumetric::VolumeDataType& volume, int layers, int rows, int cols, int startLayer, int endLayer, int startRow, int endRow, int startCol, int endCol) {
	int newLayers = endLayer - startLayer;
	int newRows = endRow - startRow;
	int newCols = endCol - startCol;

	Volumetric::VolumeDataType result(newLayers, Volumetric::SliceDataType(newRows * newCols));

	for (int k = 0; k < newLayers; ++k) {
		result[k] = clip2D(volume[startLayer + k], rows, cols, startRow, endRow, startCol, endCol);
	}

	return result;
}



// Funkcja przeskalowuj�ca jednowymiarowy wektor float�w reprezentuj�cy obraz 2D z okre�lonymi granicami
Volumetric::SliceDataType downsample2D(const Volumetric::SliceDataType& layer, int rows, int cols, int factor, int startRow, int endRow, int startCol, int endCol) {
	int newRows = (endRow - startRow) / factor;
	int newCols = (endCol - startCol) / factor;
	
	Volumetric::SliceDataType result(newRows * newCols);

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
Volumetric::VolumeDataType downsample3D(const Volumetric::VolumeDataType& volume, int layers, int rows, int cols, int factor, int startLayer, int endLayer, int startRow, int endRow, int startCol, int endCol) {
	int newLayers = (endLayer - startLayer) / factor;
	int newRows = (endRow - startRow) / factor;
	int newCols = (endCol - startCol) / factor;

	Volumetric::VolumeDataType result(newLayers, Volumetric::SliceDataType(newRows * newCols));

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
	f->glUniform1f(minColor_loc, this->m_minDisplWin);

	GLint maxColor_loc = f->glGetUniformLocation(this->shader_program, "maxColor");
	f->glUniform1f(maxColor_loc, this->m_maxDisplWin);

	GLint f_loc = f->glGetUniformLocation(this->shader_program, "f");
	f->glUniform3fv(f_loc, 7, (float*)this->m_filters);

	GLint fcolors_loc = f->glGetUniformLocation(this->shader_program, "fcolors");
	f->glUniform3fv(fcolors_loc, 7, (float*)this->m_fcolors);

	int factor = 1;
	if (this->m_fastDraw)
		factor = 4;


	GLint factor_loc = f->glGetUniformLocation(this->shader_program, "factor");
	f->glUniform1i(factor_loc, factor);

	int first_slice = factor * int(this->m_minSlice / factor);
	int first_row = factor * int(this->m_minRow / factor);
	int first_column = factor * int(this->m_minColumn / factor);

	int last_slice = 1 + factor * int(this->m_maxSlice / factor);
	int last_row = 1 + factor * int(this->m_maxRow / factor);
	int last_column = 1 + factor * int(this->m_maxColumn / factor);

	int small_shape[3];
	small_shape[0] = (last_slice - first_slice) / factor;
	small_shape[1] = (last_row - first_row) / factor;
	small_shape[2] = (last_column - first_column) / factor;

	VolumeDataType subvolume;

	if (factor > 1) {
		subvolume = downsample3D(this->m_volume, this->m_shape[0], this->m_shape[1], this->m_shape[2], factor, first_slice, last_slice, first_row, last_row, first_column, last_column);
	}
	else {
		subvolume = clip3D(this->m_volume, this->m_shape[0], this->m_shape[1], this->m_shape[2], first_slice, last_slice, first_row, last_row, first_column, last_column);
	}

	GLint sizeX_loc = f->glGetUniformLocation(this->shader_program, "sizeX");
	f->glUniform1i(sizeX_loc, small_shape[2]);

	GLint sizeY_loc = f->glGetUniformLocation(this->shader_program, "sizeY");
	f->glUniform1i(sizeY_loc, small_shape[1]);

	int maxSubLayer = std::min(this->m_maxSlice / factor, small_shape[0]);

	for (int idx_in_subvolume = 0; idx_in_subvolume < small_shape[0]; idx_in_subvolume++) {
		int true_index_of_slice = first_slice + idx_in_subvolume * factor;

		std::vector<float> colors = subvolume[idx_in_subvolume];
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
			volum->m_volume.resize(layers);

			for (int i = 0; i < layers; i++) {
				volum->m_volume[i].resize(rows * columns, 0.0f); // 100.0f * i / 10);
			}

		} catch (...) {
			qInfo() << "\nBRAK PAMI�CI !!!" << Qt::endl;

			//CZYSZCZENIE 

			return nullptr;
		}

		volum->m_shape[0] = layers;
		volum->m_shape[1] = rows;
		volum->m_shape[2] = columns;

		for (int i = 0; i < layers; i++) {
			SliceMetadata mdata;
			mdata.image_position_patient[2] = float(i);
			volum->metadata.push_back(mdata);
		}

		volum->m_minVal = 0.0f;
		volum->m_maxVal = 0.0f; // 100.0f * layers / 10;
		
		volum->adjustMinMax(false);

		for (auto& filter : volum->m_filters) {
			filter[1] = std::max(filter[1], volum->m_minDisplWin);
			filter[2] = std::min(filter[2], volum->m_maxDisplWin);
		}

		return volum;
	}

	return nullptr;
}




void Volumetric::adjustMinMax(bool calc_color)
{
	if (calc_color && !this->m_volume.empty()) {
		float min = *std::min_element(this->m_volume[0].begin(), this->m_volume[0].end());
		float max = *std::max_element(this->m_volume[0].begin(), this->m_volume[0].end());

		for (int i = 0; i < this->m_volume.size(); i++) {
			min = std::min(min, *std::min_element(this->m_volume[i].begin(), this->m_volume[i].end()));
			max = std::max(max, *std::max_element(this->m_volume[i].begin(), this->m_volume[i].end()));
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
	this->m_maxSlice = this->m_shape[0] - 1;
	this->m_minRow = 0;
	this->m_maxRow = this->m_shape[1] - 1;
	this->m_minColumn = 0;
	this->m_maxColumn = this->m_shape[2] - 1;
}

void Volumetric::adjustMinMaxColor(VoxelType color) {
	this->m_minVal = std::min(this->m_minVal, color);
	this->m_maxVal = std::max(this->m_maxVal, color);
	this->m_minDisplWin = this->m_minVal;
	this->m_maxDisplWin = this->m_maxVal;
}

void Volumetric::drawBox(CPoint3i origin = { 0, 0, 0 }, CPoint3i size = { 10, 10, 10 }, VoxelType color = 1000.0f)
{
	int layers = this->m_shape[0];
	int rows = this->m_shape[1];
	int cols = this->m_shape[2];

	//int minX = std::max(0, origin[0] - size[0]);
	//int minY = std::max(0, origin[1] - size[1]);
	//int minZ = std::max(0, origin[2] - size[2]);

	int minX = std::max(0, origin[0]);
	int minY = std::max(0, origin[1]);
	int minZ = std::max(0, origin[2]);

	int maxX = std::min(layers, origin[0] + size[0] + 1);
	int maxY = std::min(rows, origin[1] + size[1] + 1);
	int maxZ = std::min(cols, origin[2] + size[2] + 1 );

	for (int z=minZ; z<maxZ; z++)
		for (int y=minY; y<maxY; y++)
			for (int x = minX; x < maxX; x++)
				this->m_volume[z][y * cols + x] = color;

	this->adjustMinMaxColor(color);
}

//								def drawSphere(self, origin = [0, 0, 0], radius = 1, color = 1000.) :
//								origin_z, origin_y, origin_x = origin
//								z_min = max(0, origin_z - radius)
//								z_max = min(self.shape[0], origin_z + radius + 1)
//								y_min = max(0, origin_y - radius)
//								y_max = min(self.shape[1], origin_y + radius + 1)
//								x_min = max(0, origin_x - radius)
//								x_max = min(self.shape[2], origin_x + radius + 1)
//
//								# Przygotowanie danych wej�ciowych
//								radius_squared = radius * *2
//
//								# Tworzenie siatki wsp�rz�dnych
//								z = np.arange(z_min, z_max)
//								y = np.arange(y_min, y_max)
//								x = np.arange(x_min, x_max)
//								zv, yv, xv = np.meshgrid(z, y, x, indexing = 'ij')
//
//								# Obliczanie maski dla sfer
//								mask = (xv - origin_x) * *2 + (yv - origin_y) * *2 + (zv - origin_z) * *2 <= radius_squared
//
//								# Rysowanie sfery
//								for z_idx in tqdm(range(z_min, z_max), desc = f"drawing volumetric sphere: origin = {origin}, radius = {radius}...") :
//									slice_mask = mask[z_idx - z_min, :, : ]
//
//									shifted_mask = np.zeros_like(self.m_volume[z_idx], dtype = bool)
//									shifted_mask[y_min:y_min + slice_mask.shape[0], x_min : x_min + slice_mask.shape[1]] = slice_mask
//
//									self.m_volume[z_idx][shifted_mask] = color
//									self.adjustMinMaxColor(color)
//
//
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
//									def drawCylinder(self, origin = [0, 0, 0], radius = 1, height = 1, axis = 'z', color = 1000.) :
//									origin_z, origin_y, origin_x = origin
//									#radius = float(radius)
//									height = int(height)
//
//									if axis == 'z':
//			for z in range(origin_z - height // 2, origin_z + height // 2 + 1):
//				for y in range(origin_y - radius, origin_y + radius + 1) :
//					for x in range(origin_x - radius, origin_x + radius + 1) :
//						if ((y - origin_y) * *2 + (x - origin_x) * *2 <= radius * *2) :
//							self.m_volume[z][y, x] = color
//							elif axis == 'y' :
//							for z in range(origin_z - radius, origin_z + radius + 1) :
//								for y in range(origin_y - height // 2, origin_y + height // 2 + 1):
//									for x in range(origin_x - radius, origin_x + radius + 1) :
//										if ((z - origin_z) * *2 + (x - origin_x) * *2 <= radius * *2) :
//											self.m_volume[z][y, x] = color
//											elif axis == 'x' :
//											for z in range(origin_z - radius, origin_z + radius + 1) :
//												for y in range(origin_y - radius, origin_y + radius + 1) :
//													for x in range(origin_x - height // 2, origin_x + height // 2 + 1):
//														if ((z - origin_z) * *2 + (y - origin_y) * *2 <= radius * *2) :
//															self.m_volume[z][y, x] = color
//														else:
//			raise ValueError("Axis must be one of 'x', 'y', or 'z'.")
//				# self.m_volume.flush()
//				self.adjustMinMaxColor(color)
//
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

		auto image = clip2D(this->m_volume[idx_of_slice], this->m_shape[1], this->m_shape[2], this->m_minRow, this->m_maxRow, this->m_minColumn, this->m_maxColumn);

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
		+ "\nvolume size: " + QString::number(volTK->m_shape[2]) + " x " + QString::number(volTK->m_shape[1]) + " x " + QString::number(volTK->m_shape[0])
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
		+ "\nvolume size: " + QString::number(volTK->m_shape[2]) + " x " + QString::number(volTK->m_shape[1]) + " x " + QString::number(volTK->m_shape[0])
		+ "\nintensity range: " + QString::number(volTK->m_minDisplWin) + " - " + QString::number(volTK->m_maxDisplWin)
		+ "\n"
		+ "\nmethod: marching tetrahedron\nlower treshold: " + QString::number(pMin)
		+ "\nupper treshold: " + QString::number(pMax)
		+ "\nthinning factor: " + QString::number(div) + "\n"
	);

	return mesh;
}

bool Volumetric::getSlice(int nr, Volumetric::Layer layer, Volumetric::SliceDataType* slice, int* cols, int* rows)
{
	int _columns = this->m_shape[2];
	int _rows = this->m_shape[1];
	int _layers = this->m_shape[0];

	switch (layer)
	{
	case YZ:
		slice->resize(_layers * _rows);
		for (int row = 0; row < _rows; row++)
			for (int col = 0; col < _layers; col++)
				(*slice)[row * _layers + col] = this->m_volume[col][row * _columns + nr];

		*cols = _layers;
		*rows = _rows;
		return true;
		break;
	case ZX:
		slice->resize(_layers * _columns);
		for (int row = 0; row < _layers; row++)
			for (int col = 0; col < _columns; col++)
				(*slice)[row * _columns + col] = this->m_volume[row][nr * _columns + col];

		*cols = _columns;
		*rows = _layers;
		return true;
		break;
	case XY:
	default:
		slice->resize(_columns * _rows);
		std::copy(this->m_volume[nr].begin(), this->m_volume[nr].end(), slice->begin());
		*cols = _columns;
		*rows = _rows;
		return true;
		break;
	}

	return false;
}

Volumetric* Volumetric::getRotatedVol(Volumetric::Layer dir)
{
	int old_L = m_shape[0];
	int old_R = m_shape[1];
	int old_C = m_shape[2];

	if (dir == XY)
	{
		Volumetric* new_volume = Volumetric::create(old_L, old_R, old_C );
		for (int l = 0; l < old_L; l++)
			std::copy(m_volume[l].begin(), m_volume[l].end(), new_volume->m_volume[l].begin());
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
					float value = m_volume[l][r * old_C + c];

					new_volume->m_volume[r][c * new_C + l] = value;
				}
		return new_volume;
	}
	//else { // YZ
	//	int new_L = old_R;
	//	int new_R = old_C;
	//	int new_C = old_L;
	//}
}

QImage Volumetric::getLayerAsImage(int nr, Volumetric::Layer layer)
{
	Volumetric::SliceDataType slice;
	int cols, rows;

	if (getSlice(nr, layer, &slice, &cols, &rows))
	{
		QImage::Format imgFormat = QImage::Format::Format_Grayscale16;
		WORD maxDisplVal = 0xffff;

		QImage image = QImage(cols, rows, imgFormat);

		for (qint32 y = 0; y < rows; y++)
		{
			uint16_t* line = (uint16_t*) image.scanLine(y);
			for (qint32 x = 0; x < cols; x++)
			{
				line[x] = ((slice[y * cols + x] - this->m_minDisplWin) / (this->m_maxDisplWin - this->m_minDisplWin)) * maxDisplVal;
			}
		}
		return image;
	}

	return QImage();
}



Volumetric* Volumetric::scal1(Volumetric* src1, Volumetric* src2)
{
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

