#pragma once

// api/QtWorkspaceEvents.h
#include <QObject>
#include "../core/events/IWorkspaceEvents.h"

class QtWorkspaceEvents : public QObject, public IWorkspaceEvents {
    Q_OBJECT
public:
    explicit QtWorkspaceEvents(QObject* parent = nullptr) : QObject(parent) {}

    void emitWorkspaceEvent(const WorkspaceEvent& event) override;

signals:
    void objectActivatedSignal(int objectId);
    void objectStateChangedSignal(int objectId);
    void objectAddedSignal(int objectId);
    void objectRemovedSignal(int objectId);
    void structureChangedSignal();
};

