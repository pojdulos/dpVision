#include "CameraHostAccess.h"

#include "../core/Wektor3D.h"
#include "GLViewer.h"
#include "MainWindow.h"
#include <QMdiArea>
#include <QMdiSubWindow>

namespace {
GLViewer* currentMdiViewer()
{
    if (auto win = CMainWindow::instance()) {
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
}

GLViewer* CameraHostAccess::currentViewer()
{
    if (auto win = CMainWindow::instance()) {
        return win->currentViewer();
    }
    return nullptr;
}

void CameraHostAccess::move(float mx, float my, float mz)
{
    if (auto viewer = currentMdiViewer()) {
        viewer->transform().translate(CVector3f(mx, -my, mz));
        viewer->update();
    }
}

void CameraHostAccess::rotate(float ax, float ay, float az)
{
    if (auto viewer = currentMdiViewer()) {
        Q_UNUSED(ax);
        Q_UNUSED(ay);
        Q_UNUSED(az);
        viewer->update();
    }
}

void CameraHostAccess::setFloating(bool floating)
{
    if (auto viewer = currentMdiViewer()) {
        viewer->setCameraFloating(floating);
    }
}

bool CameraHostAccess::convertWinToWorld(CPoint3d winCoords, CPoint3d& worldCoords)
{
    if (auto viewer = currentViewer()) {
        return viewer->convertWinToWorld(winCoords, worldCoords);
    }
    return false;
}

bool CameraHostAccess::convertWorldToWin(CPoint3d worldCoords, CPoint3d& winCoords)
{
    if (auto viewer = currentViewer()) {
        return viewer->convertWorldToWin(worldCoords, winCoords);
    }
    return false;
}

bool CameraHostAccess::convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1)
{
    if (auto viewer = currentViewer()) {
        return viewer->convertCoords(winX, winY, pkt0, pkt1);
    }
    return false;
}

CPoint3d CameraHostAccess::camPos()
{
    if (auto viewer = currentViewer()) {
        return viewer->camPos();
    }
    return CPoint3d(0, 0, 0);
}

CTransform* CameraHostAccess::transform()
{
    if (auto viewer = currentViewer()) {
        return &viewer->transform();
    }
    return nullptr;
}

void CameraHostAccess::setView(int dir, std::shared_ptr<CModel3D> obj)
{
    if (auto win = CMainWindow::instance()) {
        win->actionLookDir(dir, std::move(obj));
    }
}

void CameraHostAccess::screenshot(const QString& path, void* viewer)
{
    if (viewer != nullptr) {
        static_cast<GLViewer*>(viewer)->screenshot(path);
        return;
    }

    if (auto activeViewer = currentViewer()) {
        activeViewer->screenshot(path);
    }
}
