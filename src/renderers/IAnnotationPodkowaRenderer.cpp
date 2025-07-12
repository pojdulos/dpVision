#include "IAnnotationPodkowaRenderer.h"
#include <qopengl.h>
#include "AnnotationPodkowa.h"

void IAnnotationPodkowaRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPodkowa* obj = (CAnnotationPodkowa*)_obj;
    obj->renderSelf();
}
