#pragma once

#ifndef _BaseObject_H_BY_DP_INCLUDED_
#define _BaseObject_H_BY_DP_INCLUDED_

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "Global.h"

#include "dll_global.h"

#include <set>
#include <map>
#include <string>

#include <QtCore>

class CModel3D;
class CFileInfo;
class CTransform;

class DPVISION_EXPORT CBaseObject
{
public:
	//typedef enum {
	//	HIDE_ALL = Qt::CheckState::Unchecked,
	//	HIDE_SELF = Qt::CheckState::PartiallyChecked,
	//	SHOW = Qt::CheckState::Checked
	//} Visibility;

	typedef enum {
		UNKNOWN,
		OBJECT,
		ANNOTATION
	} Category;

	typedef enum {
		//objects
		GENERIC,
		GROUP,
		MODEL,
		MOVEMENT,
		IMAGE,
		CLOUD,
		ORDEREDCLOUD,
		MESH,
		//IMAGE2D,
		VOLUMETRIC,
		VOLUMETRIC_NEW,
		VOLTK,
		LIDAR,
		OTHER,

		//annotations
		BASEANNOTATION,
		POINT,
		POINTS,
		BOUNDVECTORS,
		VERTEX,
		SETOFVERTICES,
		PATH,
		VPATH,
		ELASTICPATH,
		PLANE,
		BOX,
		CURVATUREMAP,
		ATSHELL,
		SETOFFACES,
		EDGES,
		POLYGON,
		POLYGON2D,
		CAMERA,
		SETOFCAMERAS,
		PYRAMID,
		CYLINDER,
		PODKOWA,
		HISTOGRAM,
		TRIPLE,
		SPHERE
	} Type;


	// konstruktor ze wskazaniem rodzica
	CBaseObject(CBaseObject *p = nullptr);

	// konstruktor ze wskazaniem rodzica
	CBaseObject(int objId);

	// konstruktor kopiuj�cy
	CBaseObject(const CBaseObject &b);

	virtual ~CBaseObject(void) {};

	virtual inline CBaseObject::Category category() { return CBaseObject::Category::UNKNOWN; };
	virtual inline int type() { return CBaseObject::Type::GENERIC; };

	virtual inline bool hasCategory(CBaseObject::Category cat)
	{
		return cat == category();
	};

	virtual inline bool hasType(CBaseObject::Type typ)
	{
		return typ == type();
	}

	virtual const QString xmlInfo() { return QString(); };

	virtual void applyTransformation(CTransform& /*from*/, CTransform& /*to*/) {};

	virtual CBaseObject *getCopy() { return new CBaseObject( *this ); }

	virtual void switchOpt(int /*key*/, int /*val*/) {};

	virtual void afterLoad(CModel3D*) {};

	virtual void renderTransform() {};
	virtual void renderSelf() {};
	virtual void renderKids() {};
	virtual void render();

	virtual void prepare() {};
	
	//virtual size_t size() { return 0; };
	virtual void clear() {};
	
	virtual bool isModified() { return m_modified; };
	virtual void setModified(bool b = true) { m_modified = b; };

	bool m_modified;
	virtual std::wstring infoRow() { return L". Object type: Generic CBaseObject()"; };

	virtual void info( std::wstring i[4] )
	{
		i[0] = L"simple data";
		i[1] = L"???";
		i[2] = L"generic";
		i[3] = L"Object type: Generic CBaseObject()";
	}


	CBaseObject * getParent() { return m_parent; };
	void setParent(CBaseObject *p) { m_parent = p; };
	void setParent(int objId);

	virtual void setVisible(bool v) { m_showSelf = v; };

	virtual void setSelfVisibility(bool v) { m_showSelf = v; }
	virtual void setKidsVisibility(bool v) { m_showKids = v; };
	virtual bool getSelfVisibility() { return m_showSelf; };
	virtual bool getKidsVisibility() { return m_showKids; };
	virtual bool switchSelfVisibility();
	virtual bool switchKidsVisibility();

	virtual void showChildren(bool show, QSet<CBaseObject::Type> inc = {}, QSet<CBaseObject::Type> exc = {}) {};

	int id() { return m_Id; };
	int parentId() { return (m_parent != nullptr) ? m_parent->id() : NO_CURRENT_MODEL; };

	virtual inline bool setSelected(bool sel) { return m_selected = sel; };
	virtual inline bool isSelected() { return m_selected; };

	// dla zachowania zgodno�ci przy wyszukiwaniu potomk�w
	virtual CBaseObject *findId(int /*id*/) { return NULL; };
	virtual inline bool hasChildren() { return false; };

	virtual bool hasTransformation() { return false; };
	virtual Eigen::Matrix4d getTransformationMatrix() {	return Eigen::Matrix4d::Identity();	};
	virtual Eigen::Matrix4d getGlobalTransformationMatrix();
	
	//std::string getLabel() { return m_label.toStdString(); };
	const QString& getLabel() { return m_label; };
	const std::string getLabelA() { return m_label.toUtf8().toStdString(); };
	const std::wstring getLabelW() { return m_label.toStdWString(); };

	inline void setLabel(const QString &label) { m_label = label; }
	inline void setLabel(const char* label) { m_label = QString::fromUtf8(label); }
	inline void setLabel(const std::string &label) { m_label = QString::fromUtf8(label.c_str()); }
	inline void setLabel(const std::wstring &label) { m_label = QString::fromWCharArray(label.c_str()); }

	inline void addKeyword(const QString keyword) { m_keywords.insert(keyword); }
	inline void delKeyword(const QString keyword) { m_keywords.remove(keyword); }
	inline bool hasKeyword(const QString keyword) { return m_keywords.find(keyword) != m_keywords.end(); }
	inline QSet<QString>& keywords() { return m_keywords; }

	inline void setDescr(const QString descr) { m_descr = descr; }
	const QString& getDescr() { return m_descr; }

	inline virtual void setPath(const QString& path) { m_path = path; }
	inline virtual const QString& path() { return m_path; }
	//virtual CBaseObject* getSomethingWithId(int id) { if (m_Id == id) return this; else return nullptr; };
protected:
	int m_Id;
	QString m_label;
	QString m_descr;
	QSet<QString> m_keywords;
	QString m_path;

	CBaseObject *m_parent;

	bool m_selected;

	//Visibility m_visible;
	bool m_showSelf;
	bool m_showKids;
};

typedef CBaseObject* PtrBaseObject;
typedef CBaseObject& RefBaseObject;


#endif /* _BaseObject_H_BY_DP_INCLUDED_ */