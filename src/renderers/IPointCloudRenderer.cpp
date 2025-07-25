#include "IPointCloudRenderer.h"
#include "PointCloud.h"

#define PACKET_SIZE (size_t)1000000

void IPointCloudRenderer::renderSelf(const CBaseObject* _obj)
{
	CPointCloud* obj = (CPointCloud*)_obj;

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (obj->hasVertexColors() && !obj->materials()[0]->m_force)
	{
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh.getMaterial().FrontColor.emission.fV(buf));

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	}
	else
	{
		GLfloat kol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, obj->materials()[0]->FrontColor.ambient.fV(kol));
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, obj->materials()[0]->FrontColor.diffuse.fV(kol));
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, obj->materials()[0]->FrontColor.specular.fV(kol));
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, obj->materials()[0]->FrontColor.emission.fV(kol));
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, obj->materials()[0]->FrontColor.shininess);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(CPointCloud::m_pointSize);

	size_t idx = 0;

	while (idx < obj->vertices().size()) {
		glVertexPointer(3, GL_FLOAT, 0, &(obj->vertices()[idx]));


		if (obj->hasVertexColors())
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, &(obj->vcolors()[idx]));
		}

		if (obj->hasVertexNormals())
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &(obj->vnormals()[idx]));
		}

		glDrawArrays(GL_POINTS, 0, std::min(PACKET_SIZE, obj->vertices().size() - idx));

		if (obj->hasVertexNormals())
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		if (obj->hasVertexColors())
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}

		idx += PACKET_SIZE;
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
	glPopMatrix();

}

