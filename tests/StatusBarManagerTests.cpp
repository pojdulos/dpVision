#include "TestFramework.h"

#include "../src/core/StatusBarManager.h"

#include <QString>
#include <string>
#include <vector>

namespace {

class RecordingStatusListener : public IStatusListener {
public:
    std::vector<std::string> messages;
    int clearCalls = 0;

    void setText(const std::string& text) override
    {
        messages.push_back(text);
    }

    void clear() override
    {
        ++clearCalls;
    }
};

struct StatusListenerScope {
    RecordingStatusListener listener;

    StatusListenerScope()
    {
        StatusBarManager::setListener(&listener);
    }

    ~StatusListenerScope()
    {
        StatusBarManager::setListener(nullptr);
    }
};

} // namespace

TEST_CASE(StatusBarManagerSetTextWritesToListener)
{
    StatusListenerScope scope;

    StatusBarManager::setText(QString::fromUtf8("hello"));

    REQUIRE_EQ(scope.listener.messages.size(), static_cast<size_t>(1));
    REQUIRE_EQ(scope.listener.messages.back(), std::string("hello"));
}

TEST_CASE(StatusBarManagerPrintfFormatsUtf8Message)
{
    StatusListenerScope scope;

    StatusBarManager::printf("value=%d", 42);

    REQUIRE_EQ(scope.listener.messages.size(), static_cast<size_t>(1));
    REQUIRE_EQ(scope.listener.messages.back(), std::string("value=42"));
}

TEST_CASE(StatusBarManagerPrintfFormatsWideMessage)
{
    StatusListenerScope scope;

    StatusBarManager::printf(L"wide=%d", 7);

    REQUIRE_EQ(scope.listener.messages.size(), static_cast<size_t>(1));
    REQUIRE_EQ(scope.listener.messages.back(), std::string("wide=7"));
}
