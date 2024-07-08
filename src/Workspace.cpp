#include "Workspace.h"
//#include "WorkspaceXML.h"
#include "WorkspaceFile.h"

#include "AP.h"

CWorkspace::CWorkspace() : QObject()
{
	m_bOK = true;
	m_idOfCurrentModel = NO_CURRENT_MODEL;

	m_selection.clear();

	InitLights();
}

CWorkspace::~CWorkspace()
{
	clear();
}

CWorkspace* CWorkspace::instance()
{
	static CWorkspace instance;
	return &instance;
}




void CWorkspace::clear()
{
	while ( ! m_data.empty() )
	{
		delete m_data.begin()->second;
		m_data.erase(m_data.begin());
	}
	m_selection.clear();

	_setCurrentModel(NO_CURRENT_MODEL);
}





/*
	Add an existing Model to the Workspace
*/
bool CWorkspace::_addModel( CModel3D *pMdlR )
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

bool CWorkspace::_removeModel( int i, bool deleteIt)
{
	if (NO_CURRENT_MODEL == i) return false;

	if ( m_data.find( i ) != m_data.end() )
	{
		if (inSelection(i)) removeFromSelection(i);

		if (i == m_idOfCurrentModel)
		{
			_setCurrentModel(NO_CURRENT_MODEL);
		}
		
		if (deleteIt) delete m_data[i];

		m_data.erase( i );
		return true;
	}

	return false;
}

bool CWorkspace::_removeAllModels()
{
	for (Children::iterator it = m_data.begin(); it != m_data.end(); )
	{
		delete it->second;
		it = m_data.erase(it);
	}
	m_selection.clear();

	_setCurrentModel(NO_CURRENT_MODEL);

	return true;
}

CModel3D* CWorkspace::_getModel( int i )
{
	if (NO_CURRENT_MODEL == i) return nullptr;

	Children::iterator it = m_data.find( i );

	if ( it == m_data.end() ) return nullptr;

	return (CModel3D*)m_data[ i ];
}

#include "GroupObject.h"

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
		CWorkspace::iterator it = m_data.find(id); // szukam tylko wœród obiektów z pierwszego poziomu
		if ((it != m_data.end()) && (it->second->hasType(CObject::MODEL))) // i jeszcze siê upewniam ¿e to jest CModel3D
		{
			result.push_back(id);
		}
	}

	return result;
}

std::list<int> CWorkspace::getSelection(std::set<CBaseObject::Type> types, CObject* dad)
{
	std::list<int> result;
	for (auto id : m_selection)
	{
		CBaseObject* kid = (dad==nullptr)?getSomethingWithId(id):dad->getSomethingWithId(id); // szukam wœród wszystkich obiektów w scenie lub w obiekcie (jest to wolniejsze)
		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end())) // i sprawdzam czy typ jest na liœcie
		{
			result.push_back(id);
		}
	}
	
	return result;
}

std::list<CBaseObject*> CWorkspace::getSelected(std::set<CBaseObject::Type> types, CObject* dad)
{
	std::list<CBaseObject*> result;
	for (auto id : m_selection)
	{
		CBaseObject* kid = (dad == nullptr) ? getSomethingWithId(id) : dad->getSomethingWithId(id); // szukam wœród wszystkich obiektów w scenie lub w obiekcie (jest to wolniejsze)
		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end())) // i sprawdzam czy typ jest na liœcie
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
	m_lights[0].setPosition( 0.0f, 0.0f, 1.0f, 0.0f );
	m_lights[0].setSpot( -5.0f, -20.0f, -100.0f, 180.0f );

	m_lights[0].fixed = true; //true;
	m_lights[0].active = true; //true;

	m_lights[1].setAmbient( 0.2f, 0.2f, 0.2f, 1.0f );
	m_lights[1].setDiffuse( 0.4f, 0.4f, 0.4f, 1.0f );
	m_lights[1].setSpecular( 0.6f, 0.6f, 0.6f, 1.0f );
	m_lights[1].setPosition( 20.0f, 0.0f, -20.0f, 1.0f );
	m_lights[1].setSpot( -20.0f, 0.0f, 20.0f, 60.0f );

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

/*
void TestVolum()
{
	glEnable(GL_POINT_SPRITE_ARB);

	//glBindTexture(GL_TEXTURE_2D, obiekt_tekstury);
	//glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );	
	glColor4f(0.8f, 1.0f, 0.6f, 0.3f);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	//glBegin(GL_QUADS);
	glVertex3f(5,5,5);
	glVertex3f(5,5,6);
	glVertex3f(5,5,7);
	glVertex3f(5,5,8);
	//glVertex3f(5,10,5);
	//glVertex3f(10,10,5);
	//glVertex3f(10,5,5);
	glEnd();
	glPointSize(1.0);
	glDisable( GL_COLOR_MATERIAL );

	glDisable(GL_POINT_SPRITE_ARB);

	static float fog_color[4] = { 1.0 , 1.0 , 0.0 , 0.0 };
	glEnable(GL_FOG) ;
	glFogf(GL_FOG_DENSITY, 0.005 ) ;
	glFogfv(GL_FOG_COLOR, fog_color); 
}
*/


CBaseObject* CWorkspace::getSomethingWithId(int id)
{
	CWorkspace::iterator model;
	for (model = this->m_data.begin(); model != this->m_data.end(); model++)
	{
		if (model->second->id() == id) return model->second;
		else
		{
			CBaseObject* obj = model->second->getSomethingWithId(id);
			if (obj != nullptr) return obj;
		}
	}
	return nullptr;
}

void CWorkspace::render()
{
	CWorkspace::iterator model;
	for ( model = this->m_data.begin(); model != this->m_data.end(); model++ )
	{
		glPushMatrix();

		//rysuje model
		if ((*model).second != nullptr)
		{
			(*model).second->render();
		}
		
		// przywracam macierz widoku przestrzeni roboczej
		glPopMatrix();
	}
}


void CWorkspace::renderLights( bool perm )
{
//	GLUquadric* q = gluNewQuadric();
	for (int iL=0; iL<8; iL++)
	{
		if ( perm != m_lights[iL].fixed )
			continue;
		else if ( m_lights[iL].active )
		{
			glDisable( m_lights[iL].light );
			glLightfv( m_lights[iL].light, GL_AMBIENT, m_lights[iL].ambient );
			glLightfv( m_lights[iL].light, GL_DIFFUSE, m_lights[iL].diffuse );
			glLightfv( m_lights[iL].light, GL_SPECULAR, m_lights[iL].specular );
			glLightfv( m_lights[iL].light, GL_POSITION, m_lights[iL].position );
			CVector3f w( m_lights[iL].spot_direction[0], m_lights[iL].spot_direction[1], m_lights[iL].spot_direction[2] );
			w.normalize();
			float tmp[3];
			tmp[0] = w.X();
			tmp[1] = w.Y();
			tmp[2] = w.Z();
			glLightfv( m_lights[iL].light, GL_SPOT_DIRECTION, tmp );
			glLightf( m_lights[iL].light, GL_SPOT_CUTOFF, m_lights[iL].spot_cut_off );
			glEnable( m_lights[iL].light );
/*
			if ( 0.0 != m_lights[iL].position[3] ) // jesli 0.0 to swiatlo kierunkowe z nieskonczonosci
			{
				glPushMatrix();

				glMaterialfv( GL_FRONT, GL_SPECULAR, m_lights[iL].specular );
				glMaterialfv( GL_FRONT, GL_AMBIENT, m_lights[iL].ambient );
				glMaterialfv( GL_FRONT, GL_DIFFUSE, m_lights[iL].diffuse );
				glMateriali(GL_FRONT, GL_SHININESS,255);

				glTranslatef( m_lights[iL].position[0], m_lights[iL].position[1], m_lights[iL].position[2] ); 
				gluSphere( q, 1.0, 32, 32 );

				glPopMatrix();
			}
*/
		}
		else
		{
			glDisable( m_lights[iL].light );
		}

	}
//	gluDeleteQuadric( q );
}

/*
void CWorkspace::RysujOsie()
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	float g0 = 0.5f, g1 = 1.0f, a=1.0f;
	int s0 = 32;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	//glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
	//glColorMaterial(GL_FRONT_AND_BACK,GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( this->m_idOfSelected < 0 ) glColor4f(0.8f, 0.2f, 0.2f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);

//	GLUquadric* q = gluNewQuadric();
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 0, 50 );
	glEnd();
//	glPushMatrix();
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

	if ( this->m_idOfSelected < 0 ) glColor4f(0.2f, 0.8f, 0.2f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 55, 0 );
	glEnd();
//	glPushMatrix();
//		glRotatef( 270.0, 1.0, 0.0, 0.0 ); 
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

	if ( this->m_idOfSelected < 0 ) glColor4f(0.2f, 0.2f, 0.8f, a);
	else glColor4f(0.2f, 0.2f, 0.2f, a);
	glBegin(GL_LINES);
		glVertex3f( 0, 0, 0 );
		glVertex3f( 55, 0, 0 );
	glEnd();
//	glPushMatrix();
//		glRotatef( 90.0, 0.0, 1.0, 0.0 ); 
//		glTranslatef( 0, 0, 50 ); 
//		gluCylinder( q, g1, 0.0, 5.0, s0, 1 );
//	glPopMatrix();

//	gluDeleteQuadric( q );

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}
*/

void CWorkspace::reset()
{
	if (this->m_idOfCurrentModel <0)
	{
		CWorkspace::iterator siatka;
		for ( siatka = this->m_data.begin(); siatka != this->m_data.end(); siatka++ )
		{
			((CModel3D*)(*siatka).second)->transform().reset();
		}
	}
	else if (! this->m_data.empty())
	{
		((CModel3D*)this->m_data[this->m_idOfCurrentModel])->transform().reset();
	}
}

//int CWorkspace::getNewId()
//{
//	// prawid³owo, ale na razie musi byæ inaczej
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
			CModel3D *obj = AP::WORKSPACE::getCurrentModel();

			//pkt0 = PointTransform( pkt0, obj->getRotation(), obj->getTranslation(), obj->getScale(), obj->getCtr(), obj->relocateCtr() );
			pkt0 = obj->getTransform().world2local( pkt0 );

			CMesh *mesh = (CMesh*)obj->getChild();
			
			std::vector<CVector3f> x = mesh->getVectors( pkt0 );

			ULONGLONG t0=GetTickCount64();

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

					ULONGLONG t1=GetTickCount64();
					UI::STATUSBAR::printf( "Tested point [%d,%d] iL=%f, maxLen=%f (time:%ld)", x, y, iL, maxLen, t1-t0 );
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
		
		Eigen::Matrix4d T = ((CModel3D*)m.second)->transform().toEigenMatrix4d();

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
void CWorkspace::load(QString path)
{
	CFileInfo fname;
	fname.setPath(path);

	if (fname.hasExt("dpvision"))
	{
		AP::WORKSPACE::removeAllModels();

		CParserDPVISION* parser = new CParserDPVISION();

		if (NULL != parser)
		{
			bool result = parser->load(path, true);

			delete parser;
		}
	}
	//else if (fname.hasExt("Faro"))
	//{
	//	AP::WORKSPACE::removeAllModels();

	//	CWorkspaceFile::loadFaro(fname.absoluteFilePathW(), *this);
	//}
}


/*
	Save Workspace to file
*/
bool CWorkspace::save(QString path)
{
	CFileInfo fname;
	fname.setPath(path);

	if (fname.hasExt("dpvision"))
	{
		CParserDPVISION* parser = new CParserDPVISION();

		if (NULL != parser)
		{
			QVector<CBaseObject*> objects;

			for (const auto& o : this->m_data)
			{
				objects << (CBaseObject*)o.second;
			}

			bool result = parser->save(objects, fname.absoluteFilePath());

			delete parser;

			return result;
		}
	}

	return false;
}
