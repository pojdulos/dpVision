#include "IPointCloudRenderer.h"

class CBaseObject;
class CPolygonGroup;

class IMeshRenderer : public IPointCloudRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
private:
    void renderPoly(CPolygonGroup &poly);

	// std::map<const QOpenGLContext*, QOpenGLShaderProgram*> programs;
	// GLint inPositionLoc;
	// GLint inVColorsLoc;
	// GLint inTexCoordLoc;
	// GLint textureUniform;
	// GLint useVNormalsLoc;
	// GLint inNormalsLoc;
	// GLuint EBO;
};
