#include "IObjectRenderer.h"

class CBaseObject;
class CMovement;
class CTransform;

class IMovementRenderer : public IObjectRenderer {
public:
    virtual void renderKids(const CBaseObject* _obj) override;

private:
    void renderFrame(CMovement* obj) const;
    void renderRotationAxe(CTransform curr, CTransform prev) const;
};
