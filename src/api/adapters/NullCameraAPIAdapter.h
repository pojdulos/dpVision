#pragma once

#include "../interfaces/ICameraAPI.h"

class NullCameraAPIAdapter : public ICameraAPI {
public:
    void move(float mx, float my, float mz) override {
        Q_UNUSED(mx);
        Q_UNUSED(my);
        Q_UNUSED(mz);
    }

    void rotate(float ax, float ay, float az) override {
        Q_UNUSED(ax);
        Q_UNUSED(ay);
        Q_UNUSED(az);
    }

    void setFloating(bool floating) override {
        Q_UNUSED(floating);
    }

    bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) override {
        Q_UNUSED(winCoords);
        Q_UNUSED(worldCoords);
        return false;
    }

    bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) override {
        Q_UNUSED(worldCoords);
        Q_UNUSED(winCoords);
        return false;
    }

    bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) override {
        Q_UNUSED(winX);
        Q_UNUSED(winY);
        Q_UNUSED(pkt0);
        Q_UNUSED(pkt1);
        return false;
    }

    CPoint3d camPos() override {
        return CPoint3d(0, 0, 0);
    }

    CTransform* transform() override {
        return nullptr;
    }

    void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) override {
        Q_UNUSED(dir);
        Q_UNUSED(obj);
    }

    GLViewer* currentViewer() override {
        return nullptr;
    }

    void screenshot(const QString& path, void* viewer = nullptr) override {
        Q_UNUSED(path);
        Q_UNUSED(viewer);
    }
};
