// plugininterface.h
#pragma once

#include "Global.h"

#include <set>
#include <QtPlugin>

class PluginInterface
{
public:
	struct PickEvent
	{
		void* viewer;
		int objId; // picked object ID
		size_t faceId; // picked face ID
		size_t vertexId; // closest vertex ID
		double x; // true coordinate X in object space
		double y; // true coordinate Y in object space
		double z; // true coordinate Z in object space

		std::set<INDEX_TYPE> hits;

		struct
		{
			double x;
			double y;
			double z;
		} origin;

		struct
		{
			double x;
			double y;
			double z;
		} direction;

		struct
		{
			double x;
			double y;
		} mouse;

		bool facePicked;
	};

protected:
	//friend class PluginManager;

	unsigned int m_ID;
	QString m_UUID;
	QString m_Name;
	QString m_Path;

public:
    virtual ~PluginInterface() {}

	unsigned int id() { return m_ID; };
	void setID(unsigned int id) { m_ID = id; };

	QString uuid() const { return m_UUID; }
	void setUUID(const QString& uuid) { m_UUID = uuid; }

	QString name() const { return m_Name; }
	void setName(const QString& name) { m_Name = name; }

	QString path() const { return m_Path; }
	void setPath(const QString& path) { m_Path = path; }


	// used to hide some plugins, like FileParsers
	virtual int isHidden(){ return 0; }
		
	bool hasUUID(const QString& strUUID) { return m_UUID == strUUID; }

	// called on doubleclick on plugin name
	virtual void onRun() {}

	// called when plugin is loaded
	virtual void onLoad() {}

	// called before plugin unload
	virtual void onUnload() {}

	// called when you select plugin
	virtual void onActivate() {}

	// called when you select another plugin
	virtual void onDeactivate() {}

	// called if button on PluginPanel is pressed
	virtual void onButton(std::wstring name) {}
	virtual void onButton(const QString &name) { onButton(name.toStdWString()); }

	// called if plugin is active and ctrl+MouseClick is done on any model
	virtual bool onMousePick(PickEvent pickEvent) { return false; }

	// called if plugin is active and shift+MouseClick is done on any model
	virtual bool onModelIndication(int objId) { return false; }

};

Q_DECLARE_INTERFACE(PluginInterface, "dpVision.PluginInterface/1.0")
