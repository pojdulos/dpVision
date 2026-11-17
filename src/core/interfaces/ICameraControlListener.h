#pragma once

#include <memory>

#include "../Point3.h"

class QString;
class CModel3D;

class ICameraControlListener {
public:
    virtual ~ICameraControlListener() = default;

    virtual void move(float mx, float my, float mz) = 0;
    virtual void rotate(float ax, float ay, float az) = 0;
    virtual void setFloating(bool floating) = 0;
    virtual bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) = 0;
    virtual bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) = 0;
    virtual bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) = 0;
    virtual CPoint3d camPos() = 0;
    virtual void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) = 0;
    virtual void screenshot(const QString& path) = 0;
};
