#pragma once

#include "dll_global.h"
#include "interfaces/ICameraControlListener.h"

class DPVISION_EXPORT CameraControlManager {
public:
    static void setListener(ICameraControlListener* listener) { listenerRef() = listener; }
    static ICameraControlListener* listener() { return listenerRef(); }

    static void move(float mx, float my, float mz) {
        if (listenerRef()) {
            listenerRef()->move(mx, my, mz);
        }
    }

    static void rotate(float ax, float ay, float az) {
        if (listenerRef()) {
            listenerRef()->rotate(ax, ay, az);
        }
    }

    static void setFloating(bool floating) {
        if (listenerRef()) {
            listenerRef()->setFloating(floating);
        }
    }

    static bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) {
        return listenerRef() ? listenerRef()->convertWinToWorld(winCoords, worldCoords) : false;
    }

    static bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) {
        return listenerRef() ? listenerRef()->convertWorldToWin(worldCoords, winCoords) : false;
    }

    static bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) {
        return listenerRef() ? listenerRef()->convertCoords(winX, winY, pkt0, pkt1) : false;
    }

    static CPoint3d camPos() {
        return listenerRef() ? listenerRef()->camPos() : CPoint3d(0, 0, 0);
    }

    static void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) {
        if (listenerRef()) {
            listenerRef()->setView(dir, std::move(obj));
        }
    }

    static void screenshot(const QString& path) {
        if (listenerRef()) {
            listenerRef()->screenshot(path);
        }
    }

private:
    static ICameraControlListener*& listenerRef() {
        static ICameraControlListener* listener = nullptr;
        return listener;
    }
};
