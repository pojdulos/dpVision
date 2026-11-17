#include "LegacyObjectTransferService.h"

#include "Annotation.h"
#include "AppStateManager.h"
#include "BaseObject.h"
#include "Model3D.h"
#include "Transform.h"
#include "UserMessageManager.h"
#include "Workspace.h"

namespace {

bool validateTransferTarget(
    const std::shared_ptr<CBaseObject>& object,
    const std::shared_ptr<CBaseObject>& newParent,
    const char* errorMessage)
{
    if (object == nullptr) {
        return false;
    }

    if ((newParent != nullptr)
        && newParent->hasCategory(CBaseObject::Category::ANNOTATION)
        && object->hasCategory(CBaseObject::Category::OBJECT)) {
        UserMessageManager::error(errorMessage);
        return false;
    }

    return true;
}

CTransform parentGlobalTransform(const std::shared_ptr<CBaseObject>& object)
{
    if (object != nullptr && object->getParent() != nullptr) {
        return CTransform(CBaseObject::getGlobalTransformationMatrix(object->getParentPtr()));
    }

    return CTransform();
}

std::shared_ptr<CModel3D> buildTransferWrapper(
    const std::shared_ptr<CBaseObject>& object,
    const char* description)
{
    std::shared_ptr<CModel3D> wrapper = std::make_shared<CModel3D>();
    wrapper->setLabel("<=>");
    wrapper->setDescr(description);

    if (object->hasCategory(CBaseObject::OBJECT)) {
        wrapper->addChild(wrapper, object);
        wrapper->importChildrenGeometry();
    } else if (object->hasCategory(CBaseObject::ANNOTATION)) {
        wrapper->addAnnotation(wrapper, std::dynamic_pointer_cast<CAnnotation>(object));
    }

    return wrapper;
}

void attachAsRootOrChild(
    CWorkspace* workspace,
    const std::shared_ptr<CBaseObject>& object,
    const std::shared_ptr<CBaseObject>& newParent)
{
    if (newParent == nullptr) {
        workspace->_objectAdd(object);
    } else {
        workspace->_objectAdd(object, newParent);
    }
}

void detachFromCurrentParent(CWorkspace* workspace, const std::shared_ptr<CBaseObject>& object)
{
    const std::shared_ptr<CBaseObject> oldParent = object->getParentPtr();
    if (oldParent != nullptr) {
        workspace->_objectRemove(object);
    } else {
        workspace->_objectRemove(object->id());
    }
}

} // namespace

void LegacyObjectTransferService::moveTo(
    const std::shared_ptr<CBaseObject>& object,
    const std::shared_ptr<CBaseObject>& newParent,
    bool keepPosition)
{
    if (!validateTransferTarget(
            object,
            newParent,
            "regular object cannot be moved as a descendant of annotation")) {
        return;
    }

    CWorkspace* workspace = CWorkspace::instance();
    const std::shared_ptr<CBaseObject> oldParent = object->getParentPtr();
    const CTransform sourceParentTransform = oldParent != nullptr
        ? CTransform(CBaseObject::getGlobalTransformationMatrix(oldParent))
        : CTransform();

    detachFromCurrentParent(workspace, object);

    if (keepPosition) {
        std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
            object,
            "Macierz dopasowania, wygenerowana podczas przenoszenia obiektu");

        if (newParent == nullptr) {
            wrapper->setTransform(sourceParentTransform);
            workspace->_objectAdd(wrapper);
        } else {
            const CTransform targetParentTransform(CBaseObject::getGlobalTransformationMatrix(newParent));
            const CTransform finalTransform = CTransform::fromTo(sourceParentTransform, targetParentTransform);
            wrapper->setTransform(finalTransform);
            workspace->_objectAdd(wrapper, newParent);
        }
    } else {
        attachAsRootOrChild(workspace, object, newParent);
    }

    AppStateManager::updateAllViews();
}

void LegacyObjectTransferService::copyTo(
    const std::shared_ptr<CBaseObject>& object,
    const std::shared_ptr<CBaseObject>& newParent,
    bool keepPosition)
{
    if (!validateTransferTarget(
            object,
            newParent,
            "regular object cannot be copied as a descendant of annotation")) {
        return;
    }

    std::shared_ptr<CBaseObject> copy = object != nullptr ? object->getCopy() : nullptr;
    if (copy == nullptr) {
        return;
    }

    CWorkspace* workspace = CWorkspace::instance();
    if (keepPosition) {
        const CTransform sourceParentTransform = parentGlobalTransform(object);

        if (newParent == nullptr) {
            std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
                copy,
                "Macierz dopasowania, wygenerowana podczas kopiowania obiektu");
            wrapper->setTransform(sourceParentTransform);
            workspace->_objectAdd(wrapper);
        } else {
            const CTransform targetParentTransform(CBaseObject::getGlobalTransformationMatrix(newParent));
            const CTransform finalTransform = CTransform::fromTo(sourceParentTransform, targetParentTransform);
            CTransform finalTransformForCheck(finalTransform);

            if (finalTransformForCheck.toQMatrix4x4().isIdentity()) {
                workspace->_objectAdd(copy, newParent);
            } else {
                std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
                    copy,
                    "Macierz dopasowania, wygenerowana podczas kopiowania obiektu");
                wrapper->setTransform(finalTransform);
                workspace->_objectAdd(wrapper, newParent);
            }
        }
    } else {
        attachAsRootOrChild(workspace, copy, newParent);
    }

    AppStateManager::updateAllViews();
}
