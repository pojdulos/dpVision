#pragma once
#include <memory>

#include "BaseObject.h"

enum class WorkspaceEventType {
    ObjectActivated,
    ObjectStateChanged,
    ObjectAdded,
    ObjectRemoved,
    StructureChanged,
    // kolejne typy
};

struct WorkspaceEvent {
    WorkspaceEventType type;
    int objectId = -1;           // -1 jeœli nie dotyczy konkretnego obiektu
    CBaseObject::Type objectType = CBaseObject::Type::GENERIC;
};

// core/IWorkspaceEvents.h
class IWorkspaceEvents {
public:
    virtual void emitWorkspaceEvent(const WorkspaceEvent& event) = 0;
    virtual ~IWorkspaceEvents() = default;
};
