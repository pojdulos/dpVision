#pragma once

#include <memory>
#include "../../core/Point3.h"

class QString;
class CTransform;
class CModel3D;
class GLViewer;

// Legacy compatibility contract. New plugin code should prefer
// ICameraControlAPI for safe camera operations and use IGuiInternalsAPI for
// raw viewer/transform access.
class ICameraAPI {
public:
    virtual ~ICameraAPI() = default;
    virtual void move(float mx, float my, float mz) = 0;
    virtual void rotate(float ax, float ay, float az) = 0;
    virtual void setFloating(bool floating) = 0;
    virtual bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) = 0;
    virtual bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) = 0;
    virtual bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) = 0;
    virtual CPoint3d camPos() = 0;
    [[deprecated("Legacy raw GUI escape hatch. Prefer ICameraControlAPI and only use IGuiInternalsAPI for privileged transform access.")]]
    virtual CTransform* transform() = 0;
    virtual void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) = 0;
    [[deprecated("Legacy raw GUI escape hatch. Prefer ICameraControlAPI and only use IGuiInternalsAPI for privileged viewer access.")]]
    virtual GLViewer* currentViewer() = 0;
    virtual void screenshot(const QString& path, void* viewer = nullptr) = 0;
};
