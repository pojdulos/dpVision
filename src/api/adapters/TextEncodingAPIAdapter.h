#pragma once

#include "../interfaces/ITextEncodingAPI.h"
#include <QString>

class TextEncodingAPIAdapter : public ITextEncodingAPI {
public:
    std::string utf8Encode(const std::wstring& value) override {
        return QString::fromStdWString(value).toUtf8().toStdString();
    }

    std::wstring utf8Decode(const std::string& value) override {
        return QString::fromUtf8(value.c_str()).toStdWString();
    }

    std::string unicodeToAnsi(const std::wstring& value) override {
        return QString::fromStdWString(value).toLocal8Bit().toStdString();
    }

    std::wstring ansiToUnicode(const std::string& value) override {
        return QString::fromLocal8Bit(value.c_str()).toStdWString();
    }

    std::wstring utf8ToWString(const std::string& value) override {
        return QString::fromUtf8(value.c_str()).toStdWString();
    }

    std::wstring stringToWString(const std::string& value) override {
        return QString::fromStdString(value).toStdWString();
    }

    std::string wstringToString(const std::wstring& value) override {
        return QString::fromStdWString(value).toUtf8().toStdString();
    }
};
