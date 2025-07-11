#pragma once

// api/QtWorkspaceEvents.h
#include <QObject>
#include "../core/events/IWorkspaceEvents.h"

class QtWorkspaceEvents : public QObject, public IWorkspaceEvents {
    Q_OBJECT
public:
    explicit QtWorkspaceEvents(QObject* parent = nullptr) : QObject(parent) {}

    void modelAdded(int modelId) override { emit modelAddedSignal(modelId); }
    void modelRemoved(int modelId) override { emit modelRemovedSignal(modelId); }

signals:
    void modelAddedSignal(int modelId);
    void modelRemovedSignal(int modelId);
};
