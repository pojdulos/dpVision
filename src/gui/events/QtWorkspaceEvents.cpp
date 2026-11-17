#include "QtWorkspaceEvents.h"

#include "ImageViewerHost.h"
#include "ImageViewerState.h"
#include "MainWindow.h"
#include "DockWidgetHistogram.h"

void QtWorkspaceEvents::emitWorkspaceEvent(const WorkspaceEvent& event) {
    switch (event.type) {
    case WorkspaceEventType::ObjectActivated:
        emit objectActivatedSignal(event.objectId);
        break;
    case WorkspaceEventType::ObjectStateChanged:
        emit objectStateChangedSignal(event.objectId);
        break;
    case WorkspaceEventType::ObjectAdded:
        emit objectAddedSignal(event.objectId);
        break;
    case WorkspaceEventType::ObjectRemoved:
        if (event.objectType == CBaseObject::Type::HISTOGRAM)
        {
            auto dock = CMainWindow::instance()->dockHisto;
            auto p = (CBaseObject*)dock->getHistogram();
            if (p)
                if (p->id() == event.objectId)
                {
                    p->setSelfVisibility(false);
                    dock->setVisible(false);
                    dock->setHistogram(NULL);
                }
        }
        else if (event.objectType == CBaseObject::Type::IMAGE)
        {
            ImageViewerState::clear(event.objectId);
            ImageViewerHost::closeAll(event.objectId);
        }
        emit objectRemovedSignal(event.objectId);
        break;
    case WorkspaceEventType::StructureChanged:
        emit structureChangedSignal();
        break;
    }
}
