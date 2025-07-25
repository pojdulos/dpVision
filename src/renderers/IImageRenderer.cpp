#include "IImageRenderer.h"
#include <qopengl.h>
#include "Image.h"

void IImageRenderer::renderSelf(const CBaseObject* _obj)
{
    CImage* obj = (CImage*)_obj;

	if (obj->getSelfVisibility())
	{
		QOpenGLTexture ogltx(*(QImage*)obj);

		glLoadName(obj->id());
		glPushName(obj->id());

		glPushAttrib(GL_ALL_ATTRIB_BITS);


		glBindTexture(GL_TEXTURE_2D, ogltx.textureId());
		glEnable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glBegin(GL_QUADS);

		auto img3d_half_height = obj->img3d_half_height;
		auto img3d_half_width = obj->img3d_half_width;

		glTexCoord2f(0.0, 0.0); 		glVertex2f(-img3d_half_width, img3d_half_height);
		glTexCoord2f(1.0, 0.0); 		glVertex2f(img3d_half_width, img3d_half_height);
		glTexCoord2f(1.0, 1.0); 		glVertex2f(img3d_half_width, -img3d_half_height);
		glTexCoord2f(0.0, 1.0); 		glVertex2f(-img3d_half_width, -img3d_half_height);

		glEnd();


		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL);

		glPopAttrib();

		glPopName();
		glLoadName(0);

		//if (bDrawBB) renderBoundingBox();
	}
}
