#include "IAnnotationCameraRenderer.h"
#include <qopengl.h>
#include "AnnotationCamera.h"
#include "Model3D.h"

void IAnnotationCameraRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationCamera* obj = (CAnnotationCamera*)_obj;

	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	//glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	

	glMultMatrixd(obj->matrix());

	std::shared_ptr<CBaseObject> parent = obj->getParentPtr();

	if ( parent->isChecked() || obj->isChecked() )
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	double scale = 1.0;
	if (parent)
	{
		while (auto grnd = parent->getParentPtr())
		{
			scale *= std::dynamic_pointer_cast<CAnnotation>(parent)->getTransform().scale().x;
			parent = grnd;
		}
		scale *= std::dynamic_pointer_cast<CModel3D>(parent)->getTransform().scale().x;
	}

	double aspect = obj->sensor().realWidth / obj->sensor().realHeight;
	double sX = 0.40 / scale;
	double sY = sX / aspect;
	//double f = displayScale * m_sensor.mmFocalLength;

	glLineWidth(3);

	glBegin( GL_QUADS );
	/*glVertex3d(-sX, -sY, f);
	glVertex3d(sX, -sY, f);
	glVertex3d(sX, sY, f);
	glVertex3d(-sX, sY, f);
	*/glVertex3d(-sX, -sY, 0);
	glVertex3d(sX, -sY, 0);
	glVertex3d(sX, sY, 0);
	glVertex3d(-sX, sY, 0);
	glEnd();


	glColor4ub(0, 0, 0, 255);
	glBegin(GL_LINES);
	glVertex3d(-sX, -sY, 0); glVertex3d(sX, -sY, 0);
	glVertex3d(sX, -sY, 0); glVertex3d(sX, sY, 0);
	glVertex3d(sX, sY, 0); glVertex3d(-sX, sY, 0);
	glVertex3d(-sX, sY, 0); glVertex3d(-sX, -sY, 0);
	glEnd();


	//glColor4ub(255, 255, 0, 255);

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);


	//glBegin(GL_TRIANGLES);
	//glVertex3d(0.0, 0.0, 0.0);
	//glVertex3d(-sX, sY, f);
	//glVertex3d(sX, sY, f);

	//glVertex3d(0.0, 0.0, 0.0);
	//glVertex3d(sX, -sY, f);
	//glVertex3d(-sX, -sY, f);
	//glEnd();

	//glBegin(GL_LINES);
	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(-sX, sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(sX, sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(sX, -sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(-sX, -sY, f);
	//glEnd();

	glPopAttrib();
	glPopMatrix();
}
