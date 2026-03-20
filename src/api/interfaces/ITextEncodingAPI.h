#pragma once

#include <string>

class ITextEncodingAPI {
public:
    virtual ~ITextEncodingAPI() = default;
    virtual std::string utf8Encode(const std::wstring& value) = 0;
    virtual std::wstring utf8Decode(const std::string& value) = 0;
    virtual std::string unicodeToAnsi(const std::wstring& value) = 0;
    virtual std::wstring ansiToUnicode(const std::string& value) = 0;
    virtual std::wstring utf8ToWString(const std::string& value) = 0;
    virtual std::wstring stringToWString(const std::string& value) = 0;
    virtual std::string wstringToString(const std::wstring& value) = 0;
};
