#pragma once

#include "../interfaces/ICameraAPI.h"
#include "../../api/AP.h"
#include "../../core/Point3.h"
#include "../../core/Wektor3D.h"
#include "../../gui/MainWindow.h"
#include "../../gui/GLViewer.h"
#include <QMdiArea>
#include <QMdiSubWindow>

class CameraAPIAdapter : public ICameraAPI {
    static GLViewer* currentMdiViewer()
    {
        if (auto win = AP::mainWinPtr()) {
            QMdiArea* mdiArea = win->ui.mdiArea;
            if (mdiArea != nullptr) {
                const QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
                if (!windows.empty()) {
                    return qobject_cast<GLViewer*>(windows[0]->widget());
                }
            }
        }
        return nullptr;
    }

public:
    void move(float mx, float my, float mz) override {
        if (auto viewer = currentMdiViewer()) {
            viewer->transform().translate(CVector3f(mx, -my, mz));
            viewer->update();
        }
    }

    void rotate(float ax, float ay, float az) override {
        if (auto viewer = currentMdiViewer()) {
            Q_UNUSED(ax);
            Q_UNUSED(ay);
            Q_UNUSED(az);
            viewer->update();
        }
    }

    void setFloating(bool floating) override {
        if (auto viewer = currentMdiViewer()) {
            viewer->setCameraFloating(floating);
        }
    }

    bool convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords) override {
        if (auto viewer = currentViewer()) {
            return viewer->convertWinToWorld(winCoords, worldCoords);
        }
        return false;
    }

    bool convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords) override {
        if (auto viewer = currentViewer()) {
            return viewer->convertWorldToWin(worldCoords, winCoords);
        }
        return false;
    }

    bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1) override {
        if (auto viewer = currentViewer()) {
            return viewer->convertCoords(winX, winY, pkt0, pkt1);
        }
        return false;
    }

    CPoint3d camPos() override {
        if (auto viewer = currentViewer()) {
            return viewer->camPos();
        }
        return CPoint3d(0, 0, 0);
    }

    CTransform* transform() override {
        if (auto viewer = currentViewer()) {
            return &viewer->transform();
        }
        return nullptr;
    }

    void setView(int dir, std::shared_ptr<CModel3D> obj = nullptr) override {
        AP::mainWin().actionLookDir(dir, obj);
    }

    GLViewer* currentViewer() override {
        if (auto win = AP::mainWinPtr()) {
            return win->currentViewer();
        }
        return nullptr;
    }

    void screenshot(const QString& path, void* viewer = nullptr) override {
        if (viewer != nullptr) {
            static_cast<GLViewer*>(viewer)->screenshot(path);
            return;
        }

        if (auto activeViewer = currentViewer()) {
            activeViewer->screenshot(path);
        }
    }
};
