#pragma once

#include "../interfaces/IFileSystemAPI.h"
#include <QDir>
#include <QFile>
#include <QString>
#include <fstream>

class FileSystemAPIAdapter : public IFileSystemAPI {
public:
    std::string getNativePath(const std::string& path) override {
        return QDir::toNativeSeparators(QString::fromStdString(path)).toStdString();
    }

    std::wstring getNativePath(const std::wstring& path) override {
        return QDir::toNativeSeparators(QString::fromStdWString(path)).toStdWString();
    }

    bool fileExists(const std::string& path) override {
        return QFile::exists(QString::fromStdString(path));
    }

    bool fileExists(const std::wstring& path) override {
        return QFile::exists(QString::fromStdWString(path));
    }

    bool deleteFile(const std::string& path) override {
        return QFile(QString::fromStdString(path)).remove();
    }

    bool deleteFile(const std::wstring& path) override {
        return QFile(QString::fromStdWString(path)).remove();
    }

    std::string readFileToString(const std::wstring& path) override {
        std::ifstream stream(std::string(path.begin(), path.end()));
        if (!stream.is_open()) {
            return {};
        }

        stream.seekg(0, std::ios::end);
        const std::streamoff size = stream.tellg();
        if (size <= 0) {
            return {};
        }

        std::string buffer(static_cast<size_t>(size), '\0');
        stream.seekg(0, std::ios::beg);
        stream.read(&buffer[0], size);
        return buffer;
    }
};
