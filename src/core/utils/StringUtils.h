#pragma once
#include <string>
#include <QString>

namespace dpVisionCore {
    namespace utils {

        inline std::string wstr2str(const std::wstring& ws) {
            return QString::fromWCharArray(ws.c_str()).toUtf8().toStdString();
        }
        inline std::wstring str2wstr(const std::string& s) {
            return QString::fromUtf8(s.c_str()).toStdWString();
        }

    } // namespace utils
} // namespace dpVisionCore
