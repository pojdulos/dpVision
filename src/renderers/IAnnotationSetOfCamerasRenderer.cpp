#include "IAnnotationSetOfCamerasRenderer.h"
#include <qopengl.h>
#include "AnnotationSetOfCameras.h"

void IAnnotationSetOfCamerasRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSetOfCameras* obj = (CAnnotationSetOfCameras*)_obj;
    obj->renderSelf();
}
