#pragma once
#include "CommonDLL.h"
#include <set>

#include "PluginInterface.h"

#define PLUGIN_NAME "Plugin wzorcowy"
#define PLUGIN_UUID "00000000-0000-0000-0000-000000000000"

class DPVISION_DLL_API Plugin : public PluginInterface
{
public:
	Plugin(void);

	std::string uuid() const;
	void setUUID(const std::string& uuid);
	bool hasUUID(std::string strUUID);

	std::wstring name() const;
	void setName(const std::wstring& name);

	std::wstring getPath() const;
	void setPath(const std::wstring& name);

protected:
	friend class PluginManager;
};

//class DPVISION_DLL_API Plugin
//{
//public:
//	struct DPVISION_DLL_API PickEvent
//	{
//		void* viewer;
//		int objId; // picked object ID
//		size_t faceId; // picked face ID
//		size_t vertexId; // closest vertex ID
//		double x; // true coordinate X in object space
//		double y; // true coordinate Y in object space
//		double z; // true coordinate Z in object space
//
//		std::set<INDEX_TYPE> hits;
//
//		struct
//		{
//			double x;
//			double y;
//			double z;
//		} origin;
//
//		struct
//		{
//			double x;
//			double y;
//			double z;
//		} direction;
//
//		struct
//		{
//			double x;
//			double y;
//		} mouse;
//
//		bool facePicked;
//	};
//
//
//    Plugin(void);
//	virtual ~Plugin(void);
//
//	std::string uuid() const;
//	std::wstring name() const;
//	std::wstring getPath() const;
//
//    //unsigned int GetMenuID() const;
//    //void SetMenuID( const unsigned int& id );
//
//    void __stdcall setID( unsigned int id ) { m_ID = id; };
//	unsigned int __stdcall getID() { return m_ID; };
//
//	// used to hide some plugins, like FileParsers
//	virtual int isHidden(){ return 0; };
//	
//	bool hasUUID(std::string strUUID);
//
//	/* ============= EVENT HANDLING BEGIN =========================*/
//
//	// called on doubleclick on plugin name
//	virtual void onRun() {};
//
//	// called when plugin is loaded
//	virtual void onLoad(){};
//
//	// called before plugin unload
//	virtual void onUnload(){};
//
//	// called when you select plugin
//	virtual void onActivate() {};
//
//	// called when you select another plugin
//	virtual void onDeactivate() {};
//
//	// called if button on PluginPanel is pressed
//	virtual void onButton( std::wstring name ){};
//
//	// called if plugin is active and ctrl+MouseClick is done on any model
//	virtual bool onMousePick(Plugin::PickEvent pickEvent) { return false; };
//
//	// called if plugin is active and shift+MouseClick is done on any model
//	virtual bool onModelIndication(int objId) { return false; };
//
//	/* ============= EVENT HANDLING END =========================*/
//
//protected:
//    friend class PluginManager;
//	void setName(const std::wstring& name);
//	void setUUID(const std::string& uuid);
//	void setPath(const std::wstring& name);
//
////private:
//    std::wstring m_Name;
//    std::wstring m_Path;
//	unsigned int m_ID;
//	std::string m_UUID;
//};
