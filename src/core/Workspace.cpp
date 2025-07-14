#include "Workspace.h"
//#include "WorkspaceXML.h"
#include "WorkspaceFile.h"

#include "AP.h"

#include "../renderers/IWorkspaceRenderer.h"

CWorkspace::CWorkspace() : QObject()
{
	m_bOK = true;
	m_idOfCurrentModel = NO_CURRENT_MODEL;

	renderer_ = std::make_shared<IWorkspaceRenderer>();

	m_selection.clear();

	InitLights();
}

CWorkspace* CWorkspace::instance()
{
	static CWorkspace instance;
	return &instance;
}




void CWorkspace::clear()
{
	m_selection.clear();
	m_data.clear();

	_setCurrentModel(NO_CURRENT_MODEL);
}





/*
	Add an existing Model to the Workspace
*/
bool CWorkspace::_addModel( std::shared_ptr<CModel3D> pMdlR )
{
	if (nullptr == pMdlR) return false;

	try {
		this->m_data.insert( CWorkspace::value_type( pMdlR->id(), pMdlR ) );
		pMdlR->setParent(nullptr);
	}
	catch ( std::bad_alloc &e ) {
		return false;
	}

	m_bOK = true;

	return true;
}

bool CWorkspace::_removeModel( int i)
{
	if (NO_CURRENT_MODEL == i) return false;

	if ( m_data.find( i ) != m_data.end() )
	{
		if (inSelection(i)) removeFromSelection(i);

		if (i == m_idOfCurrentModel)
		{
			_setCurrentModel(NO_CURRENT_MODEL);
		}
		
		//if (deleteIt) delete m_data[i];

		m_data.erase( i );
		return true;
	}

	return false;
}

bool CWorkspace::_removeAllModels()
{
	for (Children::iterator it = m_data.begin(); it != m_data.end(); )
	{
		//delete it->second;
		it = m_data.erase(it);
	}
	m_selection.clear();

	_setCurrentModel(NO_CURRENT_MODEL);

	return true;
}

std::shared_ptr<CWorkspace::ChildType> CWorkspace::_getModel( int i )
{
	if (NO_CURRENT_MODEL == i) return nullptr;

	Children::iterator it = m_data.find( i );

	if ( it == m_data.end() ) return nullptr;

	return m_data[ i ];
}


int CWorkspace::_setCurrentModel( int i )
{
	if (i != m_idOfCurrentModel)
	{
		onCurrentObjectChanged(i);
		emit currentObjectChanged(m_idOfCurrentModel);
	}

	return m_idOfCurrentModel;
}



void CWorkspace::addToSelection(int id)
{
	if (!inSelection(id))
	{
		m_selection.push_back(id);
		UI::STATUSBAR::setText(QString("Object selected (pos.: ") + QString::number(getNumberInSelection(id)) + ")");
	}
	//m_data[id]->setSelected(true);
}

void CWorkspace::removeFromSelection(int id)
{
	if (inSelection(id))
	{
		UI::STATUSBAR::setText(QString("Object at pos. ") + QString::number(getNumberInSelection(id)) + " unselected");
		m_selection.remove(id);
	}
	//m_data[id]->setSelected(false);
}

bool CWorkspace::inSelection(int id)
{
	return ( m_selection.end() != std::find(m_selection.begin(), m_selection.end(), id ) );
}

int CWorkspace::getNumberInSelection(int id)
{
	std::list<int>::iterator it = std::find(m_selection.begin(), m_selection.end(), id);

	if (it != m_selection.end())
	{
		return std::distance( m_selection.begin(), it );
	}

	return -1;
}

bool CWorkspace::changeSelection(int id, bool bb)
{
	bool s = inSelection( id );
	bool b = bb;

	if ( s != b )
	{
		if (b)
		{
			addToSelection(id);
		}
		else
		{
			removeFromSelection(id);
		}
		return true;
	}
	return false;
}

std::list<int> CWorkspace::getSelection()
{
	std::list<int> result;
	for (auto id : m_selection)
	{
		CWorkspace::iterator it = m_data.find(id); // szukam tylko w�r�d obiekt�w z pierwszego poziomu
		if ((it != m_data.end()) && (it->second->hasType(CObject::MODEL))) // i jeszcze si� upewniam �e to jest CModel3D
		{
			result.push_back(id);
		}
	}

	return result;
}

std::list<int> CWorkspace::getSelection(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> dad)
{
	std::list<int> result;
	for (auto id : m_selection)
	{
		std::shared_ptr<CBaseObject> kid = (dad==nullptr)?getSomethingWithId(id):dad->getSomethingWithId(id); // szukam w�r�d wszystkich obiekt�w w scenie lub w obiekcie (jest to wolniejsze)
		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end())) // i sprawdzam czy typ jest na li�cie
		{
			result.push_back(id);
		}
	}
	
	return result;
}

std::list<std::shared_ptr<CBaseObject>> CWorkspace::getSelected(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> dad)
{
	std::list<std::shared_ptr<CBaseObject>> result;
	for (auto id : m_selection)
	{
		std::shared_ptr<CBaseObject> kid = (dad == nullptr) ? getSomethingWithId(id) : dad->getSomethingWithId(id); // szukam w�r�d wszystkich obiekt�w w scenie lub w obiekcie (jest to wolniejsze)
		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end())) // i sprawdzam czy typ jest na li�cie
		{
			result.push_back(kid);
		}
	}

	return result;
}


void CWorkspace::InitLights()
{
	m_lights[0].light = GL_LIGHT0;
	m_lights[1].light = GL_LIGHT1;
	m_lights[2].light = GL_LIGHT2;
	m_lights[3].light = GL_LIGHT3;
	m_lights[4].light = GL_LIGHT4;
	m_lights[5].light = GL_LIGHT5;
	m_lights[6].light = GL_LIGHT6;
	m_lights[7].light = GL_LIGHT7;

	m_lights[0].setAmbient( 0.6f, 0.6f, 0.6f, 1.0f );
	m_lights[0].setDiffuse( 0.6f, 0.6f, 0.6f, 1.0f );
	m_lights[0].setSpecular( 0.0f, 0.0f, 0.0f, 1.0f );
	//m_lights[0].setPosition( 5.0f, 20.0f, -100.0f, 0.0f );
	m_lights[0].setPosition( 0.0f, 0.0f, 1000.0f, 0.0f );
	m_lights[0].setSpot( -5000.0f, -20000.0f, -100000.0f, 180.0f );

	m_lights[0].fixed = true; //true;
	m_lights[0].active = true; //true;

	m_lights[1].setAmbient( 0.2f, 0.2f, 0.2f, 1.0f );
	m_lights[1].setDiffuse( 0.4f, 0.4f, 0.4f, 1.0f );
	m_lights[1].setSpecular( 0.6f, 0.6f, 0.6f, 1.0f );
	m_lights[1].setPosition( 20000.0f, 0.0f, -20000.0f, 1.0f );
	m_lights[1].setSpot( -20000.0f, 0.0f, 20000.0f, 60.0f );

	m_lights[1].fixed = false;
	m_lights[1].active = false;
}


int CWorkspace::_setNextModelCurrent()
{
	CWorkspace::iterator it = m_data.find(m_idOfCurrentModel);
	int i;
	
	if ( it == m_data.end() )
	{
		i = -1;
	}
	else
	{
		it++;
		if ( it != m_data.end() )
		{
			i = (*it).first;
		}
		else
		{
			i = -1;
		}
	}
	
	return _setCurrentModel( i );
}


int CWorkspace::_setPreviousModelCurrent()
{
	CWorkspace::iterator it = m_data.find(m_idOfCurrentModel);
	int i;
	
	if ( it == m_data.end() )
	{
		i = -1;
	}
	else
	{
		it--;
		if ( it != m_data.end() )
		{
			i = (*it).first;
		}
		else
		{
			i = -1;
		}
	}
	
	return _setCurrentModel( i );
}


std::shared_ptr<CBaseObject> CWorkspace::getSomethingWithId(int id)
{
	for (auto& model : this->m_data)
	{
		if (model.second->id() == id) return model.second;
		else
		{
			std::shared_ptr<CBaseObject> obj = model.second->getSomethingWithId(id);
			if (obj != nullptr) return obj;
		}
	}
	return nullptr;
}


void CWorkspace::render()
{
	if (renderer_) renderer_->render(this);
}

void CWorkspace::renderLights( bool perm )
{
	if (renderer_) renderer_->renderLights(this, perm);
}


void CWorkspace::reset()
{
	if (this->m_idOfCurrentModel <0)
	{
		CWorkspace::iterator siatka;
		for ( auto &siatka : this->m_data )
		{
			if (siatka.second->hasTransformation()) siatka.second->transform().reset();
		}
	}
	else if (! this->m_data.empty())
	{
		this->m_data[this->m_idOfCurrentModel]->transform().reset();
	}
}

//int CWorkspace::getNewId()
//{
//	// prawid�owo, ale na razie musi by� inaczej
//	//int newId = MODEL_ID_OFFSET;
//	//while ( m_data.find( newId ) != m_data.end() )
//	//{
//	//	newId += 1000;
//	//}
//	//return newId;
//
//	if ( m_data.empty() )
//		return MODEL_ID_OFFSET;
//	else
//		return last()->id()+MODEL_DATA_SPACE;
//}
//

std::vector<CRGBA> CWorkspace::getXRayImage( CPoint3f pkt0, int size )
{
	std::vector<CRGBA> result;

	result.reserve( size * size );

	if ( -1 != AP::WORKSPACE::getCurrentModelId() )
	{
			std::shared_ptr<CModel3D> obj = AP::WORKSPACE::getCurrentModel();

			//pkt0 = PointTransform( pkt0, obj->getRotation(), obj->getTranslation(), obj->getScale(), obj->getCtr(), obj->relocateCtr() );
			pkt0 = obj->getTransform().world2local( pkt0 );

			std::shared_ptr<CMesh> mesh = std::dynamic_pointer_cast<CMesh>(obj->getChild());
			
			std::vector<CVector3f> x = mesh->getVectors( pkt0 );

			//ULONGLONG t0=GetTickCount64();
			auto t0 = std::chrono::steady_clock::now();

			for ( int x=0; x<size; x++ )
			{
				for  ( int y=0; y<size; y++ )
				{
					float maxLen;

					float dx = ( (float) (x - size/2) ) / 100;
					float dy = ( (float) (y - size/2) ) / 100;

					CPoint3f pkt1 = obj->getTransform().origin() + CPoint3f( CTriple<float>( dx, dy, 0.0f) );

					CVector3f vRay( pkt0, pkt1 );
					vRay.normalize();

					float iL = mesh->getInteriorLength( pkt0, vRay, maxLen );

					CRGBA col( 0.0f, 0.0f, 0.0f, 0.0f );

					if ( iL > 0 ) 
					{
						col.SetFloat ( iL/maxLen, iL/maxLen, iL/maxLen, 1.0f );
					}

					result.push_back( col );

					// ULONGLONG t1=GetTickCount64();
					auto t1 = std::chrono::steady_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

					UI::STATUSBAR::printf( "Tested point [%d,%d] iL=%f, maxLen=%f (time:%ld)", x, y, iL, maxLen, duration );
				}
			}

	}

	return result;
}




// SLOTS

void CWorkspace::onCurrentObjectChanged(CBaseObject* obj)
{
	if (obj != nullptr)
		onCurrentObjectChanged(obj->id());
}

void CWorkspace::onCurrentObjectChanged(int i)
{
	if ((i == NO_CURRENT_MODEL) || (m_data.find(i) == m_data.end()))
	{
		m_idOfCurrentModel = NO_CURRENT_MODEL;
	}
	else
	{
		m_idOfCurrentModel = i;
	}
}

CBoundingBox CWorkspace::topBB()
{
	CBoundingBox bb;

	for (auto &m : m_data)
	{
		CPoint3d min = m.second->getMin();
		CPoint3d max = m.second->getMax();
		
		Eigen::Matrix4d T = m.second->transform().toEigenMatrix4d();

		CPoint3d m1 = T * min;
		CPoint3d m2 = T * CPoint3d(min.x, min.y, max.z);
		CPoint3d m3 = T * CPoint3d(min.x, max.y, max.z);
		CPoint3d m4 = T * CPoint3d(min.x, max.y, min.z);
		CPoint3d m5 = T * CPoint3d(max.x, max.y, min.z);
		CPoint3d m6 = T * CPoint3d(max.x, min.y, min.z);
		CPoint3d m7 = T * CPoint3d(max.x, min.y, max.z);
		CPoint3d m8 = T * max;

		bb.expand(m1);
		bb.expand(m2);
		bb.expand(m3);
		bb.expand(m4);
		bb.expand(m5);
		bb.expand(m6);
		bb.expand(m7);
		bb.expand(m8);

		//min = CPoint3d(
		//	std::min(std::min(std::min(m1.x, m2.x), std::min(m3.x, m4.x)), std::min(std::min(m5.x, m6.x), std::min(m7.x, m8.x))),
		//	std::min(std::min(std::min(m1.y, m2.y), std::min(m3.y, m4.y)), std::min(std::min(m5.y, m6.y), std::min(m7.y, m8.y))),
		//	std::min(std::min(std::min(m1.z, m2.z), std::min(m3.z, m4.z)), std::min(std::min(m5.z, m6.z), std::min(m7.z, m8.z))));

		//max = CPoint3d(
		//	std::max(std::max(std::max(m1.x, m2.x), std::max(m3.x, m4.x)), std::max(std::max(m5.x, m6.x), std::max(m7.x, m8.x))),
		//	std::max(std::max(std::max(m1.y, m2.y), std::max(m3.y, m4.y)), std::max(std::max(m5.y, m6.y), std::max(m7.y, m8.y))),
		//	std::max(std::max(std::max(m1.z, m2.z), std::max(m3.z, m4.z)), std::max(std::max(m5.z, m6.z), std::max(m7.z, m8.z))));

		//bb.expand(CBoundingBox(min, max));
	}

	return bb;
}


#include "ParserDPV.h"

/*
	Load Workspace from file
*/
//void CWorkspace::load(QString path)
//{
//	CFileInfo fname;
//	fname.setPath(path);
//
//	if (fname.hasExt("dpvision"))
//	{
//		AP::WORKSPACE::removeAllModels();
//
//		CParserDPVISION* parser = new CParserDPVISION();
//
//		if (NULL != parser)
//		{
//			bool result = parser->load(path, true);
//
//			delete parser;
//		}
//	}
//}


/*
	Save Workspace to file
*/
//bool CWorkspace::save(QString path)
//{
//	CFileInfo fname;
//	fname.setPath(path);
//
//	if (fname.hasExt("dpvision"))
//	{
//		CParserDPVISION* parser = new CParserDPVISION();
//
//		if (NULL != parser)
//		{
//			QVector<CBaseObject*> objects;
//
//			for (const auto& o : this->m_data)
//			{
//				objects << (CBaseObject*)o.second;
//			}
//
//			bool result = parser->save(objects, fname.absoluteFilePath());
//
//			delete parser;
//
//			return result;
//		}
//	}
//
//	return false;
//}
