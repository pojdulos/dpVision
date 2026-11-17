#pragma once

#include "../../core/interfaces/ICameraControlListener.h"
#include "../CameraHostAccess.h"

class QtCameraControlAdapter : public ICameraControlListener {
public:
    void move(float mx, float my, float mz) override {
        CameraHostAccess::move(mx, my, mz);
    }

    void rotate(float ax, float ay, float az) override {
        CameraHostAccess::rotate(ax, ay, az);
    }

    void setFloating(bool floating) override {
        CameraHostAccess::setFloating(floating);
    }

    bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) override {
        return CameraHostAccess::convertWinToWorld(winCoords, worldCoords);
    }

    bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) override {
        return CameraHostAccess::convertWorldToWin(worldCoords, winCoords);
    }

    bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) override {
        return CameraHostAccess::convertCoords(winX, winY, pkt0, pkt1);
    }

    CPoint3d camPos() override {
        return CameraHostAccess::camPos();
    }

    void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) override {
        CameraHostAccess::setView(dir, std::move(obj));
    }

    void screenshot(const QString& path) override {
        CameraHostAccess::screenshot(path, nullptr);
    }
};
