#pragma once

#include <string>

class IMessageListener {
public:
    virtual ~IMessageListener() = default;
    virtual void information(const std::string &msg, const std::string &tittle) = 0;
    virtual void warning(const std::string &msg, const std::string &tittle) = 0;
    virtual void error(const std::string &msg, const std::string &tittle) = 0;
    //virtual int question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) = 0;
};
