#pragma once

#include "../../core/Point3.h"
#include "../../core/dll_global.h"
#include <memory>

class CModel3D;
class CTransform;
class GLViewer;
class QString;

class DPVISION_EXPORT CameraHostAccess {
public:
    static GLViewer* currentViewer();
    static void move(float mx, float my, float mz);
    static void rotate(float ax, float ay, float az);
    static void setFloating(bool floating);
    static bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords);
    static bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords);
    static bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1);
    static CPoint3d camPos();
    static CTransform* transform();
    static void setView(int dir, std::shared_ptr<CModel3D> obj);
    static void screenshot(const QString& path, void* viewer = nullptr);
};
