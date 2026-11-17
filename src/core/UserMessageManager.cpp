#include "UserMessageManager.h"

IMessageListener*& UserMessageManager::listenerRef()
{
    static IMessageListener* listener = nullptr;
    return listener;
}
