#pragma once

#include <string>

enum class UserMessageSeverity {
    Info,
    Warning,
    Error
};

enum class UserMessageChannel {
    Auto,
    Status,
    Log,
    Popup,
    Modal
};

struct UserMessage {
    UserMessageSeverity severity = UserMessageSeverity::Info;
    UserMessageChannel channel = UserMessageChannel::Auto;
    std::string title;
    std::string text;
    std::string details;
};

struct UserQuestion {
    UserMessageChannel channel = UserMessageChannel::Modal;
    std::string title;
    std::string text;
    std::string details;
    std::string button0 = "Yes";
    std::string button1 = "No";
    std::string button2;
};
