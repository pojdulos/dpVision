#include "IMovementRenderer.h"
#include <qopengl.h>
#include "Movement.h"
#include "../core/Annotation.h"
#include "../core/Transform.h"
#include "../core/Utilities.h"
#include "../core/dpLog.h"

void IMovementRenderer::renderKids(const CBaseObject* _obj)
{
	CMovement* obj = (CMovement*)_obj;
	renderFrame(obj);
}

void IMovementRenderer::renderFrame(CMovement* obj) const
{
	if ((obj == nullptr) || (obj->size() <= 0))
	{
		return;
	}

	auto& frame = obj->currentFrame();

	if (obj->currentKey() > 0)
	{
		renderRotationAxe(frame.t, obj->frame(obj->currentKey() - 1).t);
	}
	else
	{
		renderRotationAxe(frame.t, CTransform());
	}

	glPushMatrix();
	frame.t.render();

	for (const auto& child : obj->children())
	{
		switch (child.second->type())
		{
		case CObject::MODEL:
			child.second->render();
			break;
		default:
			glLoadName(obj->id());
			glPushName(obj->id());
			child.second->render();
			glPopName();
			glLoadName(0);
			break;
		}
	}

	for (const auto& annotation : obj->annotations())
	{
		annotation.second->render();
	}

	glPopMatrix();
}

void IMovementRenderer::renderRotationAxe(CTransform curr, CTransform prev) const
{
	CQuaternion prevQ = prev.rotation();
	CQuaternion currQ = curr.rotation();

	CQuaternion relQ = prevQ.inverse() * currQ;

	CVector3d prevT = prev.translation();
	CVector3d currT = curr.translation();
	CVector3d relT = currT - prevT;

	Eigen::AngleAxisd absAA(currQ.toEigen().toRotationMatrix());
	Eigen::Vector3d absAxis = absAA.axis();
	double absAngle = absAA.angle();
	double absLen = 200.0;

	Eigen::AngleAxisd relAA(relQ.toEigen().toRotationMatrix());
	Eigen::Vector3d relAxis = relAA.axis();
	double relAngle = relAA.angle();
	double relLen = 200.0;

	dpInfo() << "-------------------------------------------------------------" << Qt::endl;
	dpInfo() << "ABS angle: " << rad2deg(absAngle) << "Â° translation: " << currT.length() << "mm" << Qt::endl;
	dpInfo() << "REL angle: " << rad2deg(relAngle) << "Â° translation: " << relT.length() << "mm" << Qt::endl;

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glColor3f(1.0f, 0.0f, 1.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(absAxis.x() * -absLen, absAxis.y() * -absLen, absAxis.z() * -absLen);
	glVertex3f(absAxis.x() * absLen, absAxis.y() * absLen, absAxis.z() * absLen);
	glEnd();

	glColor3f(1.0f, 0.7f, 0.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(relAxis.x() * -relLen, relAxis.y() * -relLen, relAxis.z() * -relLen);
	glVertex3f(relAxis.x() * relLen, relAxis.y() * relLen, relAxis.z() * relLen);
	glEnd();

	glPopAttrib();
	glPopMatrix();
}

