#include "../core/StatusBarManager.h"
#include "../core/MessageBoxManager.h"
#include "../core/interfaces/IProgressListener.h"
#include "../core/PointCloud.h"
#include "../core/interfaces/IStatusListener.h"
#include "../core/interfaces/IMessageListener.h"

IStatusListener* StatusBarManager::listener_ = nullptr;
IMessageListener* MessageBoxManager::listener_ = nullptr;
std::shared_ptr<IProgressListener> IProgressListener::defaultListener_ = nullptr;
int CPointCloud::m_pointSize = 1;
