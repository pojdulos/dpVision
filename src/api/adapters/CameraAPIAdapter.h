#pragma once

#include "../interfaces/ICameraAPI.h"
#include "../UI.h"
#include "../../core/Point3.h"

class CameraAPIAdapter : public ICameraAPI {
public:
    void move(float mx, float my, float mz) override {
        UI::CAMERA::move(mx, my, mz);
    }

    void rotate(float ax, float ay, float az) override {
        UI::CAMERA::rotate(ax, ay, az);
    }

    void setFloating(bool floating) override {
        UI::CAMERA::setFloating(floating);
    }

    bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) override {
        return UI::CAMERA::convertWinToWorld(winCoords, worldCoords);
    }

    bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) override {
        return UI::CAMERA::convertWorldToWin(worldCoords, winCoords);
    }

    bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) override {
        return UI::CAMERA::convertCoords(winX, winY, pkt0, pkt1);
    }

    CPoint3d camPos() override {
        return UI::CAMERA::camPos();
    }

    CTransform* transform() override {
        return UI::CAMERA::transform();
    }

    void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) override {
        UI::CAMERA::setView(dir, obj);
    }

    GLViewer* currentViewer() override {
        return UI::CAMERA::currentViewer();
    }

    void screenshot(const QString& path, void* viewer = nullptr) override {
        UI::CAMERA::screenshot(path, viewer);
    }
};
