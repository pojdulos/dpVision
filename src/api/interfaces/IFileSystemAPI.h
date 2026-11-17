#pragma once

#include <string>

class IFileSystemAPI {
public:
    virtual ~IFileSystemAPI() = default;
    virtual std::string getNativePath(const std::string& path) = 0;
    virtual std::wstring getNativePath(const std::wstring& path) = 0;
    virtual bool fileExists(const std::string& path) = 0;
    virtual bool fileExists(const std::wstring& path) = 0;
    virtual bool deleteFile(const std::string& path) = 0;
    virtual bool deleteFile(const std::wstring& path) = 0;
    virtual std::string readFileToString(const std::wstring& path) = 0;
};
