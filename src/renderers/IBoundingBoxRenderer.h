#pragma once

#include "dll_global.h"

#include "../core/BoundingBox.h"
#include <qopengl.h>

class DPVISION_EXPORT IBoundingBoxRenderer
{
public:
	virtual ~IBoundingBoxRenderer() = default;
	virtual void render(const CBoundingBox& box, CBoundingBox::Style style, bool checked) const
	{
		if (box.isInvalid() || (style == CBoundingBox::Style::DontRender))
		{
			return;
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLineWidth(1.0f);

		if (style == CBoundingBox::Style::NotSelected)
		{
			glColor3f(0.2f, 0.2f, 0.2f);
		}
		else if (style == CBoundingBox::Style::Locked)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		else
		{
			glColor3f(0.0f, 0.5f, 0.0f);
		}

		renderBox(box, false);

		if (checked)
		{
			glLineWidth(3.0f);
			glColor4f(0.5f, 0.5f, 0.0f, 0.5f);
			renderBox(box, true);
		}

		glDisable(GL_COLOR_MATERIAL);
		glPopAttrib();
	}

private:
	void renderBox(const CBoundingBox& box, bool dashed) const
	{
		if (dashed)
		{
			glLineStipple(3, 0x00FF);
			glEnable(GL_LINE_STIPPLE);
		}

		const CPoint3d& min = box.getMin();
		const CPoint3d& max = box.getMax();

		glPushAttrib(GL_ENABLE_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_QUAD_STRIP);
		glVertex3d(max.X(), min.Y(), min.Z());
		glVertex3d(max.X(), min.Y(), max.Z());

		glVertex3d(min.X(), min.Y(), min.Z());
		glVertex3d(min.X(), min.Y(), max.Z());

		glVertex3d(min.X(), max.Y(), min.Z());
		glVertex3d(min.X(), max.Y(), max.Z());

		glVertex3d(max.X(), max.Y(), min.Z());
		glVertex3d(max.X(), max.Y(), max.Z());

		glVertex3d(max.X(), min.Y(), min.Z());
		glVertex3d(max.X(), min.Y(), max.Z());
		glEnd();

		glPopAttrib();
	}
};
