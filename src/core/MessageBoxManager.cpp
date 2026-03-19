#include "MessageBoxManager.h"

IMessageListener*& MessageBoxManager::listenerRef()
{
    static IMessageListener* listener = nullptr;
    return listener;
}
