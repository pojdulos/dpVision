#pragma once

#include "../../core/CameraControlManager.h"
#include "../interfaces/ICameraControlAPI.h"

class GuiCameraAPIAdapter : public ICameraControlAPI {
public:
    void move(float mx, float my, float mz) override {
        CameraControlManager::move(mx, my, mz);
    }

    void rotate(float ax, float ay, float az) override {
        CameraControlManager::rotate(ax, ay, az);
    }

    void setFloating(bool floating) override {
        CameraControlManager::setFloating(floating);
    }

    bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) override {
        return CameraControlManager::convertWinToWorld(winCoords, worldCoords);
    }

    bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) override {
        return CameraControlManager::convertWorldToWin(worldCoords, winCoords);
    }

    bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) override {
        return CameraControlManager::convertCoords(winX, winY, pkt0, pkt1);
    }

    CPoint3d camPos() override {
        return CameraControlManager::camPos();
    }

    void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) override {
        CameraControlManager::setView(dir, std::move(obj));
    }

    void screenshot(const QString& path) override {
        CameraControlManager::screenshot(path);
    }
};
