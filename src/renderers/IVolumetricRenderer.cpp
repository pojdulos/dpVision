#include "IVolumetricRenderer.h"
#include <QtOpenGL>
#include <QtGui/QOpenGLFunctions_3_2_Core>
#include <QtGui/QOpenGLFunctions>
//#include <QOpenGLBuffer>
//#include <QOpenGLVertexArrayObject>

#include "Volumetric.h"
#include "IVolumetricShaders.h"

extern bool mouse_key_pressed;

void IVolumetricRenderer::renderSelf(const CBaseObject* _obj)
{
    Volumetric* obj = (Volumetric*)_obj;

	if (this->m_renderBoxes != obj->m_renderBoxes) {
		this->m_renderBoxes = obj->m_renderBoxes;
		this->remove_shader_program();
		this->shader_program = 0;
	}

	QOpenGLFunctionsType* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctionsType>();

	f->glEnable(GL_PROGRAM_POINT_SIZE);

	if (this->shader_program == 0) {
		this->create_program(obj,f);
	}

	f->glUseProgram(this->shader_program);

	if (this->vbo == 0)
		f->glGenBuffers(1, &this->vbo);

	f->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	// Ustawienie atrybutu aCol w location 0
	f->glEnableVertexAttribArray(0);

	//f->glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
	f->glVertexAttribIPointer(0, 1, GL_INT, 0, nullptr);


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
	f->glUniform1f(minColor_loc, float(obj->m_minDisplWin));

	GLint maxColor_loc = f->glGetUniformLocation(this->shader_program, "maxColor");
	f->glUniform1f(maxColor_loc, float(obj->m_maxDisplWin));

	GLint f_loc = f->glGetUniformLocation(this->shader_program, "f");
	f->glUniform3fv(f_loc, 7, (float*)obj->m_filters);
	//f->glUniform3iv(f_loc, 7, (int32_t*)obj->m_filters);

	GLint fcolors_loc = f->glGetUniformLocation(this->shader_program, "fcolors");
	f->glUniform3fv(fcolors_loc, 7, (float*)obj->m_fcolors);

	int factor = 1;
	if (obj->m_fastDraw || mouse_key_pressed)
		factor = 4;


	GLint factor_loc = f->glGetUniformLocation(this->shader_program, "factor");
	f->glUniform1i(factor_loc, factor);


	int first_slice = obj->m_minSlice;
	int first_row = obj->m_minRow;
	int first_column = obj->m_minColumn;

	int last_slice = obj->m_maxSlice;
	int last_row = obj->m_maxRow;
	int last_column = obj->m_maxColumn;

	int small_shape[3];

	small_shape[0] = 1 + last_slice - first_slice;
	small_shape[1] = 1 + last_row - first_row;
	small_shape[2] = 1 + last_column - first_column;


	if (factor > 1) {
		first_slice = (obj->m_minSlice / factor) * factor;
		first_row = (obj->m_minRow / factor) * factor;
		first_column = (obj->m_minColumn / factor) * factor;

		last_slice = (obj->m_maxSlice / factor) * factor;
		last_row = (obj->m_maxRow / factor) * factor;
		last_column = (obj->m_maxColumn / factor) * factor;

		small_shape[0] = 1 + (last_slice - first_slice) / factor;
		small_shape[1] = 1 + (last_row - first_row) / factor;
		small_shape[2] = 1 + (last_column - first_column) / factor;
	}


	Volumetric::VolumeType subvolume;

	if (factor > 1) {
		subvolume = obj->downsample3D(
			obj->m_data,
			obj->m_layers, obj->m_rows, obj->m_columns,
			factor,
			first_slice, last_slice,
			first_row, last_row,
			first_column, last_column);
	}
	else {
		subvolume = obj->clip3D(
			obj->m_data,
			obj->m_layers, obj->m_rows, obj->m_columns,
			first_slice, last_slice,
			first_row, last_row,
			first_column, last_column);
	}

	GLint sizeX_loc = f->glGetUniformLocation(this->shader_program, "sizeX");
	f->glUniform1i(sizeX_loc, small_shape[2]);

	GLint sizeY_loc = f->glGetUniformLocation(this->shader_program, "sizeY");
	f->glUniform1i(sizeY_loc, small_shape[1]);

	int maxSubLayer = std::min(obj->m_maxSlice / factor, small_shape[0]);

	for (int idx_in_subvolume = 0; idx_in_subvolume < small_shape[0]; idx_in_subvolume++) {
		int true_index_of_slice = first_slice + idx_in_subvolume * factor;

#if defined(_VoxelTypeFLOAT)
		Volumetric::SliceType colors = subvolume[idx_in_subvolume];
#else
		Volumetric::SliceType colors = subvolume[idx_in_subvolume];
		// SliceType subvol = subvolume[idx_in_subvolume];
		// std::vector<float> colors(subvol.size());
		// std::transform(subvol.begin(), subvol.end(), colors.begin(), [](VoxelType value) { return static_cast<float>(value); });
#endif

		f->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(int32_t), colors.data(), GL_STATIC_DRAW);
		//f->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

		SliceMetadata metadata = obj->metadata[true_index_of_slice];

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



void IVolumetricRenderer::remove_shader_program() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glDeleteProgram(this->shader_program);
	this->shader_program = 0;
}


unsigned int IVolumetricRenderer::compile_shader(QOpenGLFunctionsType* f, const char* source, unsigned int shader_type) {
	GLuint shader = f->glCreateShader(shader_type);

	f->glShaderSource(shader, 1, &source, NULL);
	f->glCompileShader(shader);

	// Sprawdzenie, czy kompilacja si� powiod�a
	GLint success;
	f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	//	checkGLError("compile_shaders",source);

	if (!success)
	{
		char info_log[512];
		GLsizei len;
		f->glGetShaderInfoLog(shader, 512, &len, info_log);
		qInfo() << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << Qt::endl;
		f->glDeleteShader(shader);
		//throw std::exception("Shader compilation failed");
		throw "Shader compilation failed";
	}


	return shader;
}





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

void checkGLError(const char* label, const char* txt = "") {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		qInfo() << "OpenGL error: " << err << " after " << label << Qt::endl;
		qInfo() << txt << Qt::endl;
	}
}

//#include <exception>



bool IVolumetricRenderer::create_program(Volumetric* obj, QOpenGLFunctionsType* f) {
	//QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	GLuint vertex_shader = compile_shader(f, vertex_shader_code, GL_VERTEX_SHADER);

	GLuint geometry_shader;
	if (obj->m_renderBoxes)
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

	if (!success)
	{
		qInfo() << "Error linking shaders" << Qt::endl;
	}

	f->glDeleteShader(vertex_shader);
	f->glDeleteShader(geometry_shader);
	f->glDeleteShader(fragment_shader);

	return true;
}
