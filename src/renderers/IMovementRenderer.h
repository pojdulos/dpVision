#include "IObjectRenderer.h"

class CBaseObject;
class CMovement;

class IMovementRenderer : public IObjectRenderer {
public:
    virtual void renderKids(const CBaseObject* _obj) override;
};
