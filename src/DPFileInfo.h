#pragma once
#ifndef _DPFILEINFO_H_BY_DP_INCLUDED_
#define _DPFILEINFO_H_BY_DP_INCLUDED_

#include "UI.h"

#include <string>
#include <QtCore/QString>
#include <QtCore/QFileInfo>

class DPVISION_EXPORT CFileInfo : public QFileInfo
{
public:
	CFileInfo(void) : QFileInfo() {}
	CFileInfo(const char* mypath) : QFileInfo(QString(mypath)) {}
	CFileInfo(const QString mypath) : QFileInfo(mypath) {}
	CFileInfo(const std::string mypath) : QFileInfo(QString::fromStdString(mypath)) {}
	CFileInfo(const std::wstring mypath) : QFileInfo(QString::fromStdWString(mypath)) {}

	inline void setPath(const QString mypath) { setFile(mypath); }
	inline void setPath(const char* mypath) { setFile(QString(mypath)); }
	inline void setPath(const std::string mypath) { setFile(QString::fromStdString(mypath)); }
	inline void setPath(const std::wstring mypath) { setFile(QString::fromStdWString(mypath)); }

	
	inline void setDir(const QString dir) { setFile(dir + "/" + fileName()); }
	inline void setDir(const char* dir) { setFile(QString(dir) + "/" + fileName()); }
	inline void setDir(const std::string dir) { setFile(QString::fromStdString(dir) + "/" + fileName()); }
	inline void setDir(const std::wstring dir) { setFile(QString::fromStdWString(dir) + "/" + fileName()); }

	
	inline void setName(const QString name) { setFile(absolutePath() + "/" + name + "." + suffix()); }
	inline void setName(const char* name) { setFile(absolutePath() + "/" + QString(name) + "." + suffix()); }
	inline void setName(const std::string name) { setFile(absolutePath() + "/" + QString::fromStdString(name) + "." + suffix()); }
	inline void setName(const std::wstring name) { setFile(absolutePath() + "/" + QString::fromStdWString(name) + "." + suffix()); }


	inline void setExt(const QString ext) { setFile(absolutePath() + "/" + completeBaseName() + "." + ext); }
	inline void setExt(const char* ext) { setFile(absolutePath() + "/" + completeBaseName() + "." + QString(ext) ); }
	inline void setExt(const std::string ext) { setFile(absolutePath() + "/" + completeBaseName() + "." + QString::fromStdString(ext)); }
	inline void setExt(const std::wstring ext) { setFile(absolutePath() + "/" + completeBaseName() + "." + QString::fromStdWString(ext)); }


	inline std::wstring absolutePathW(void) { return absolutePath().toStdWString(); };
	inline std::string absolutePathA(void) { return absolutePath().toStdString(); };

	inline std::wstring suffixW(void) { return suffix().toStdWString(); };
	inline std::string suffixA(void) { return suffix().toStdString(); };

	inline std::wstring completeBaseNameW(void) { return completeBaseName().toStdWString(); };
	inline std::string completeBaseNameA(void) { return completeBaseName().toStdString(); };

	inline std::wstring absoluteFilePathW(void) { return absoluteFilePath().toStdWString(); }
	inline std::string absoluteFilePathA(void) { return absoluteFilePath().toStdString(); }

	std::wstring fileNameW(void) { return fileName().toStdWString(); };
	std::string fileNameA(void) { return fileName().toStdString(); };

	inline bool hasExt(const char* myext) { return 0 == QString(myext).compare(suffix(), Qt::CaseInsensitive); }
	inline bool hasExt(const QString myext) { return 0 == myext.compare(suffix(), Qt::CaseInsensitive); }
	inline bool hasExt(const std::string myext) { return 0 == QString::fromStdString(myext).compare(suffix(), Qt::CaseInsensitive); }
	inline bool hasExt(const std::wstring myext) { return 0 == QString::fromStdWString(myext).compare(suffix(), Qt::CaseInsensitive); }
};

#endif /* _DPFILEINFO_H_BY_DP_INCLUDED_ */
