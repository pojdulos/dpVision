#pragma once
#include "IObjectRenderer.h"

class CBaseObject;
class Volumetric;

class QOpenGLFunctions_3_2_Core;
//class QOpenGLShaderProgram;

#define QOpenGLFunctionsType QOpenGLFunctions_3_2_Core

class DPVISION_EXPORT IVolumetricRenderer : public IObjectRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
	void remove_shader_program();
private:
	bool m_renderBoxes = false;
	unsigned int vbo = 0;
	//QOpenGLVertexArrayObject vao;
	//QOpenGLShaderProgram* shader_program = nullptr;
	unsigned int shader_program = 0;

	//GLuint compile_shader(QOpenGLFunctions* functions, QString source, GLenum shader_type);
	unsigned int compile_shader(QOpenGLFunctionsType* f, const char* source, unsigned int shader_type);
	bool create_program(Volumetric* obj, QOpenGLFunctionsType* f);
};
