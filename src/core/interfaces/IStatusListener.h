#pragma once

#include <string>

class IStatusListener {
public:
    virtual ~IStatusListener() = default;
    virtual void setText(const std::string& text) = 0;
    virtual void clear() { setText(""); }
};
