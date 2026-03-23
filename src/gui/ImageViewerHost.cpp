#include "ImageViewerHost.h"

#include "ImageViewerState.h"
#include "MainWindow.h"
#include "MdiChild.h"
#include "PicViewer.h"
#include "../core/Image.h"
#include "../core/Workspace.h"

namespace {
QMdiArea* mdiArea()
{
    if (auto win = CMainWindow::instance()) {
        return win->ui.mdiArea;
    }
    return nullptr;
}
}

QMdiSubWindow* ImageViewerHost::instance(int imageId)
{
    QMdiArea* area = mdiArea();
    if (area == nullptr) {
        return nullptr;
    }

    const auto windows = area->subWindowList();
    for (QMdiSubWindow* window : windows) {
        MdiChild* child = qobject_cast<MdiChild*>(window->widget());
        if (child != nullptr && child->hasType(MdiChild::Type::Pic)) {
            PicViewer* viewer = qobject_cast<PicViewer*>(child->m_widget);
            if (viewer != nullptr && viewer->id() == imageId) {
                return window;
            }
        }
    }

    return nullptr;
}

bool ImageViewerHost::hasOpenViewer(int imageId)
{
    return instance(imageId) != nullptr;
}

void ImageViewerHost::activateOrOpen(int imageId)
{
    if (QMdiSubWindow* window = instance(imageId)) {
        if (QMdiArea* area = mdiArea()) {
            area->setActiveSubWindow(window);
        }
        return;
    }

    std::shared_ptr<CBaseObject> object = CWorkspace::instance()->getSomethingWithId(imageId);
    std::shared_ptr<CImage> image = std::dynamic_pointer_cast<CImage>(object);
    if (image != nullptr) {
        open(image.get());
        if (QMdiArea* area = mdiArea()) {
            area->setActiveSubWindow(area->currentSubWindow());
        }
    }
}

void ImageViewerHost::open(CImage* image)
{
    if (image == nullptr) {
        return;
    }

    if (QMdiArea* area = mdiArea()) {
        MdiChild::create(image, area)->show();
    }
}

void ImageViewerHost::closeAll(int imageId)
{
    QMdiArea* area = mdiArea();
    if (area == nullptr) {
        return;
    }

    const auto windows = area->subWindowList();
    for (QMdiSubWindow* window : windows) {
        MdiChild* child = qobject_cast<MdiChild*>(window->widget());
        if (child != nullptr && child->hasType(MdiChild::Type::Pic)) {
            PicViewer* viewer = qobject_cast<PicViewer*>(child->m_widget);
            if (viewer != nullptr && viewer->id() == imageId) {
                window->close();
            }
        }
    }
}

void ImageViewerHost::reload(int imageId)
{
    if (QMdiSubWindow* window = instance(imageId)) {
        MdiChild* child = qobject_cast<MdiChild*>(window->widget());
        if (child != nullptr) {
            PicViewer* viewer = qobject_cast<PicViewer*>(child->m_widget);
            if (viewer != nullptr) {
                viewer->reloadImage();
            }
        }
    }
}

bool ImageViewerHost::fitToWindow(int imageId)
{
    return ImageViewerState::fitToWindow(imageId);
}

void ImageViewerHost::setFitToWindow(int imageId, bool fit)
{
    ImageViewerState::setFitToWindow(imageId, fit);
    reload(imageId);
}
