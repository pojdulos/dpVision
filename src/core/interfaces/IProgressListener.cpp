#include "IProgressListener.h"

std::shared_ptr<IProgressListener>& IProgressListener::defaultListenerRef()
{
    static std::shared_ptr<IProgressListener> defaultListener = nullptr;
    return defaultListener;
}
