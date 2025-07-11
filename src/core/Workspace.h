#pragma once
#ifndef _WORKSPACE_H_BY_DP_INCLUDED_
#define _WORKSPACE_H_BY_DP_INCLUDED_
#include "dll_global.h"

#include "Global.h"
#include "Model3D.h"
#include "Utilities.h"


struct _light
{
	GLenum light;
	bool active;
	bool fixed;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
	float spot_direction[3];
	float spot_cut_off;

	_light(void) {
		light = GL_LIGHT0;
		active = false;
		fixed = true;
		setAmbient( 0.0f, 0.0f, 0.0f, 0.0f );
		setDiffuse( 0.0f, 0.0f, 0.0f, 0.0f );
		setSpecular( 0.0f, 0.0f, 0.0f, 0.0f );
		setPosition( 0.0f, 0.0f, 0.0f, 0.0f );
		setSpot( 0.0f, 0.0f, 0.0f, 0.0f );
	};

	void setAmbient( float f0, float f1, float f2, float f3 ) { ambient[0]=f0; ambient[1]=f1; ambient[2]=f2; ambient[3]=f3; };
	void setDiffuse( float f0, float f1, float f2, float f3 ) { diffuse[0]=f0; diffuse[1]=f1; diffuse[2]=f2; diffuse[3]=f3; };
	void setSpecular( float f0, float f1, float f2, float f3 ) { specular[0]=f0; specular[1]=f1; specular[2]=f2; specular[3]=f3; };
	void setPosition( float f0, float f1, float f2, float f3=1.0f ) { position[0]=f0; position[1]=f1; position[2]=f2; position[3]=f3; };
	void setSpot( float f0, float f1, float f2, float f3 ) { spot_direction[0]=f0; spot_direction[1]=f1; spot_direction[2]=f2; spot_cut_off=f3; };
	void setSpot( float f0, float f1, float f2 ) { spot_direction[0]=f0; spot_direction[1]=f1; spot_direction[2]=f2; };
	void setSpot( float f3 ) { spot_cut_off=f3; };
};

class IWorkspaceEvents;

class CWorkspace : public QObject
{
	Q_OBJECT

private:
	IWorkspaceEvents* events_ = nullptr;

public:
	void setEventsDispatcher(IWorkspaceEvents* events) { events_ = events; }
	IWorkspaceEvents* eventsDispatcher() const { return events_; }

	typedef CModel3D ChildType;
	//typedef std::map<int, ChildType*> Children;
	typedef std::map<int, std::shared_ptr<ChildType>> Children;
	typedef Children::iterator iterator;
	typedef Children::value_type value_type;

protected:
	friend class CWorkspaceXML;
	friend class CWorkspaceFile;

private:
	Children m_data;

	std::list<int> m_selection;

	int m_idOfCurrentModel;

	_light m_lights[8];

	bool m_bOK;
	std::string m_sError;

	explicit CWorkspace();

public:
	static CWorkspace* instance();

	Children& children() { return m_data; }
	
	//ChildType*& operator[](int i)
	//{
	//	assert(m_data.find(i) != m_data.end());
	//	return m_data[i].get();
	//}

	std::shared_ptr<ChildType> first() { if (m_data.empty()) return nullptr; return m_data.begin()->second; };
	std::shared_ptr<ChildType> last() { if (m_data.empty()) return nullptr; return m_data.rbegin()->second; };

	CWorkspace::iterator begin() { return m_data.begin(); }
	CWorkspace::iterator end() { return m_data.end(); }

	std::shared_ptr<CBaseObject> getSomethingWithId(int id);

	bool isOK() const { return m_bOK; }
	const std::string &lastError() const { return m_sError; }

	void		render();
	void		renderLights( bool );

	_light &	getLightRef( int nr ) { return m_lights[nr]; };
	
	// MODELE:
	bool		_addModel(std::shared_ptr<CModel3D> obj );
	inline bool	_addModel(CModel3D* obj) { return _addModel(std::shared_ptr<CModel3D>(obj)); }

	int			_setCurrentModel( int i );

	int			_getCurrentModelId() { return m_idOfCurrentModel; };

	std::shared_ptr<CWorkspace::ChildType> _getModel( int i );

	bool		_removeModel( int i);

	bool		_removeAllModels();

	// WORKSPACE
	//void	load( QString path );
	//bool	save( QString path );
	void	clear();
	size_t	size() const { return m_data.size(); }
	bool	empty() const { return m_data.empty(); }
	void	reset();

	//======================================================

	std::vector<CRGBA> getXRayImage( CPoint3f pkt0, int size );


	//int getNewId();

	int	_setNextModelCurrent();
	int	_setPreviousModelCurrent();

	bool inSelection(int id);
	int getNumberInSelection(int id);
	void addToSelection(int id);
	void removeFromSelection(int id);
	bool changeSelection(int id, bool b);

	std::list<int> getSelection();
	std::list<int> getSelection(std::set<CBaseObject::Type> type, std::shared_ptr<CObject> obj = nullptr);
	std::list<std::shared_ptr<CBaseObject>> getSelected(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> dad);
	void clearSelection() { m_selection.clear(); };

	CBoundingBox topBB();

signals:
	void currentObjectChanged(int);
	void currentObjectChanged(CBaseObject*);

public slots:
	void onCurrentObjectChanged(int);
	void onCurrentObjectChanged(CBaseObject*);
	
private:
	void InitLights();

};


#endif /* _WORKSPACE_H_BY_DP_INCLUDED_ */