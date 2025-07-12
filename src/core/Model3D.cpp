#include "Model3D.h"
#include "Model3D.h"
#include "Model3D.h"
#define FREEGLUT_STATIC

#include "Model3D.h"

#include "Annotation.h"
#include "AnnotationPoint.h"

#include "PMFactory.h"
#include "Utilities.h"

#include "AP.h"

#include "MainApplication.h"

#include "GLViewer.h"
#include "Parser.h"

#include "../renderers/IModel3DRenderer.h"


CModel3D::CModel3D(std::shared_ptr<CBaseObject> p) : CObject(p)
{
	setLabel("model3d");

	m_bOK = true;

	//bAnime = (AP::mainApp().globalParams.RunAnimation != 0);
	//iAnimeDir = (AP::mainApp().globalParams.RunAnimation < 0) ? -1 : 1;
	//lastanim = 0;

	m_annotations.clear();

	bDrawBB = false;

	//m_fastmeshData.iMeshType = CMesh::MESHTYPE_NONE;
	//m_fastmeshData.setParent( this );

	renderer_ = std::make_shared<IModel3DRenderer>();
}

CModel3D::CModel3D( const CModel3D &m ) : CObject(m)
{
	m_bOK = true;

	bDrawBB = false;

	//bAnime = (AP::mainApp().globalParams.RunAnimation != 0);
	//iAnimeDir = (AP::mainApp().globalParams.RunAnimation < 0) ? -1 : 1;
	//lastanim = 0;

	m_transform = m.m_transform;

	//m_annotations = CModel3D::Annotations( m.m_annotations );

	// m_transform.setOrigin( (m_max + m_min) / 2 );

	//if ( ((CObject*)getChild())->type() == CObject::MESH )
	//{
	//	createFastMesh();
	//}

	renderer_ = std::make_shared<IModel3DRenderer>();
}


//CModel3D::~CModel3D()
//{
	// Nie jestem pewien czy tu nie ma wycieku pami�ci,
	// ale pr�ba usuwania poszczeg�lnych annotacji za pomoc� delete
	// ko�czy sie wyj�tkiem przy zamykaniu programu
	// 
	// temat do zbadania!!!

//	m_annotations.clear();
//}


void CModel3D::PMeshEcoll( GLuint rzm, bool checkPoints )
{
	if ( getChild()->type() != CObject::MESH ) return;

	CMesh &d = *(std::dynamic_pointer_cast<CMesh>(this->getChild()));

	if ( ( rzm < 0 ) || ( rzm >= d.faces().size() ) ) return;

	this->setSelfVisibility(false);
	this->setKidsVisibility(false);

	//this->deleteFastMesh();

	PMFactory pmf;

	if ( checkPoints )
	{
		for ( CModel3D::Annotations::iterator it = m_annotations.begin(); it != m_annotations.end(); it++ )
		{
			if ( it->second->type() == CAnnotation::POINT )
			{
				pmf.mSolidPoints.insert( ((CAnnotationPoint*)it->second.get())->getPoint());
			}
		}
	}

	pmf.eColl( d, rzm );

	m_min = std::dynamic_pointer_cast<CMesh>(this->getChild())->getMin();
	m_max = std::dynamic_pointer_cast<CMesh>(this->getChild())->getMax();

	CPoint3f delta = m_max - m_min;

//AT	m_transform.setOrigin( (m_max + m_min ) / 2 );
//AT	m_transform.setScale( 100.0f/MAX3( delta.X(), delta.Y(), delta.Z() ) );

	//d.iMeshType &= 0xFFF0;
	//d.iMeshType |= CMesh::MESHTYPE_PROGRESSIVEMESH3V;

	d.renderFacesAsEdges(false);
	d.renderFacesAsPoints(false);
	d.renderSmoothVertex(false);
	d.renderWithTexture(d.hasTexture());

	//this->createFastMesh();
	this->setSelfVisibility(true);
	this->setKidsVisibility(true);
}

void CModel3D::PMeshVsplit( GLuint rzm )
{
	if ( getChild()->type() != CObject::MESH ) return;

	CMesh &d = *(std::dynamic_pointer_cast<CMesh>(this->getChild()));

	PMFactory pmf;

	//CFileInfo fi;
	//
	//fi.SetFullPath( plikSiatki.absoluteFilePathW().c_str() );
	//fi.SetExt( "out" );

	pmf.vSplit( d, rzm );

	m_min = d.getMin();
	m_max = d.getMax();

	CPoint3f delta = m_max - m_min;

	m_transform.setOrigin((m_max + m_min) / 2);
	m_transform.setScale(100.0f / MAX3(delta.X(), delta.Y(), delta.Z()));

	//d.iMeshType &= 0xFFF0;
	//d.iMeshType |= CMesh::MESHTYPE_PROGRESSIVEMESH3V;

	UI::STATUSBAR::printf( "I'm returning from CModel3D::PMeshVsplitSelf()..." );
}

//void CModel3D::deleteFastMesh()
//{
//	m_fastmeshData.vertices().clear();
//	//m_fastmeshData.iMeshType = CMesh::MESHTYPE_NONE;
//}
//
//void CModel3D::createFastMesh()
//{
//	if ( getChild()->type() != CObject::MESH ) return;
//
//	CMesh &d = *((CMesh *)getChild());
//
//	CMesh::Vertices::iterator it;
//	GLuint orgsize = d.vertices().size();
//
//	m_fastmeshData.vertices().clear();
//	//m_fastmeshData.iMeshType = CMesh::MESHTYPE_NONE;
//
//	if ( orgsize > 50000 )
//	{
//		GLuint newsize = 10000;
//		float delta = (float)orgsize / newsize;
//
//		try {
//			m_fastmeshData.vertices().resize( newsize );
//		}
//		catch ( ... ) {
//			return;
//		}
//
//		float j;
//		for ( unsigned int i=0; i<newsize; i++ )
//		{
//			j = delta * i;
//
//			m_fastmeshData.vertices()[ i ] = d.vertices()[ (GLuint)j ];
//		}
//
//		//m_fastmeshData.iMeshType = CMesh::MESHTYPE_VERTEXCLOUD;
//	}
//}


void CModel3D::importChildrenGeometry()
{
	resetBoundingBox();
	for (auto child : m_data)
	{
		if (child.second->hasCategory(CBaseObject::Category::OBJECT))
		{
			CObject* obj = (CObject*)child.second.get();
			expand(*obj);
		}
	}

	if (CBoundingBox::isInvalid())
		resetBoundingBox(CBoundingBox::NullBB);
//	m_min = ((CObject*)getChild())->getMin();
//	m_max = ((CObject*)getChild())->getMax();

//	CPoint3f delta = m_max - m_min;
	CPoint3f ctr = (m_max + m_min) / 2;
	
	m_transform.setOrigin( ctr );
}


void CModel3D::setOrigin(CTransform::Origin t)
{
	switch (t)
	{
	case CTransform::Origin::BBOX_CTR:
		m_transform.setOrigin( (m_max + m_min) / 2 );
		break;
	case CTransform::Origin::WEIGHT_CTR:
		if ((getChild()->type() == CObject::MESH) || (getChild()->type() == CObject::CLOUD) || (getChild()->type() == CObject::ORDEREDCLOUD))
		{
			m_transform.setOrigin((std::dynamic_pointer_cast<CPointCloud>(this->getChild())->getCenterOfWeight()));
		}
		break;
	case CTransform::Origin::ZERO:
		m_transform.setOrigin( CPoint3d(0.0, 0.0, 0.0) );
		break;
	}
}


void CModel3D::applyTransform(CTransform to)
{
	//set the new origin to the same point as before
	CPoint3d c = m_transform.origin() + m_transform.translation() - to.translation();
	to.setOrigin(c);

	// transform all children
	CObject::applyTransformation(m_transform, to);

	//transform all annotations
	for (auto &a : m_annotations)
	{
		a.second->applyTransformation(m_transform, to);
	}
	
	//CModel3D::Annotations::iterator it;
	//for (it = m_annotations.begin(); it != m_annotations.end(); it++)
	//{
	//	(*it).second->applyTransform(m_transform, to);
	//}

	importChildrenGeometry();

	m_transform.translation() = to.translation();
	m_transform.rotation() = to.rotation();
	m_transform.scale() = to.scale();
	m_transform.origin() = to.origin();
}


//CBaseObject* CModel3D::getSomethingWithId(int id)
//{
//	if (m_Id == id) return this;
//	else if (nullptr != getChild(id)) return getChild(id);
//	else if (nullptr != annotation(id)) return (CBaseObject*)annotation(id);
//	else return nullptr;
//}

void CModel3D::prepare()
{
	for (auto& c:m_data) c.second->prepare();
}




bool CModel3D::switchOption( CModel3D::Opt iOpt, CModel3D::Switch iSet )
{
	bool result = false;

	std::shared_ptr<CMesh> d = std::dynamic_pointer_cast<CMesh>(this->getChild());

	switch ( iOpt )
	{
//		case OPTION_RENDER_SMOOTH_VERTEX:
		case CModel3D::Opt::optSmoothVertices:
			if ( iSet == CModel3D::Switch::switchOn )
				result = d->renderSmoothVertex( d->hasVertexNormals() );
			else if ( iSet == CModel3D::Switch::switchOff )
				result = d->renderSmoothVertex( false );
			else
				result = d->renderSmoothVertex( d->hasVertexNormals() && !d->renderSmoothVertex() );
	
			prepare();
			break;

//		case OPTION_RENDER_FACES_AS_POINTS:
		case CModel3D::Opt::optRenderAsPoints:
			if ( getChild()->type() == CObject::LIDAR )
			{
				getChild()->switchOpt( 0, 0 );
				importChildrenGeometry();
			}
			else 
			{
				if ( iSet == CModel3D::Switch::switchOn )
					result = d->renderFacesAsPoints( d->hasFaces() );
				else if ( iSet == CModel3D::Switch::switchOff )
					result = d->renderFacesAsPoints( false );
				else
					result = d->renderFacesAsPoints( d->hasFaces() && !d->renderFacesAsPoints() );
			}
			break;

//		case OPTION_RENDER_FACES_AS_EDGES:
		case CModel3D::Opt::optRenderAsEdges:
			if ( iSet == CModel3D::Switch::switchOn )
				result = d->renderFacesAsEdges(d->hasFaces());
			else if ( iSet == CModel3D::Switch::switchOff )
				result = d->renderFacesAsEdges(false);
			else
				result = d->renderFacesAsEdges( d->hasFaces() && !d->renderFacesAsEdges());

			break;

//		case OPTION_RENDER_WITH_TEXTURE:
		case CModel3D::Opt::optRenderWithTexture:
			if ( iSet == CModel3D::Switch::switchOn )
				result = d->renderWithTexture( d->hasTexture() );
			else if ( iSet == CModel3D::Switch::switchOff )
				result = d->renderWithTexture( false );
			else
				result = d->renderWithTexture(d->hasTexture() && !d->renderWithTexture());
			
			prepare();
			break;

		default:
			break;
	}
	return result;
}



bool CModel3D::testOption( CModel3D::Opt iOption )
{
	bool result = false;

	std::shared_ptr<CMesh> d = std::dynamic_pointer_cast<CMesh>(this->getChild());

	if (d == nullptr) return false;

	switch ( iOption )
	{
//		case OPTION_RENDER_SMOOTH_VERTEX:
		case CModel3D::Opt::optSmoothVertices:
			result = d->renderSmoothVertex();
			break;

//		case OPTION_RENDER_FACES_AS_POINTS:
		case CModel3D::Opt::optRenderAsPoints:
			result = d->renderFacesAsPoints();
			break;

//		case OPTION_RENDER_FACES_AS_EDGES:
		case CModel3D::Opt::optRenderAsEdges:
			result = d->renderFacesAsEdges();
			break;

//		case OPTION_RENDER_WITH_TEXTURE:
		case CModel3D::Opt::optRenderWithTexture:
			result = d->renderWithTexture();
			break;

		default:
			break;
	}
	return result;
}


std::wstring CModel3D::infoRow()
{
	size_t n = m_data.size();
	size_t np = m_annotations.size();
	std::wstring ret = L"Model3D (id:"+ std::to_wstring(m_Id) + L"). Has ";
	
	if ( n+np == 0 )
		ret += L"not any children";
	if (n > 0)
	{
		ret += std::to_wstring(n) + ((n < 2) ? L" data child " : L" data children ");
		if (np > 0) ret += L" and ";
	}
	
	if (np > 0)
	{
		ret += std::to_wstring(np) + ((np < 2) ? L" annotation " : L" annotations ");
	}
	
	return ret;
}

//inline int CModel3D::setId(int i)
//{
//	CBaseObject::setId(i);
//
//	CBaseObject::Children newChildren;
//
//	for (CBaseObject::Children::iterator it = m_data.begin(); it != m_data.end(); it++)
//	{
//		int newId = getNewId();
//		it->second->setId(newId);
//		newChildren[newId] = it->second;
//	}
//
//	m_data = newChildren;
//
//	CModel3D::Annotations newAnnot;
//	for (CModel3D::Annotations::iterator it = m_annotations.begin(); it != m_annotations.end(); it++)
//	{
//		int newId = getNewId();
//		it->second->setId(newId);
//		newAnnot[newId] = it->second;
//	}
//
//	m_annotations = newAnnot;
//
//	return 0;
//}


std::shared_ptr<CBaseObject> CModel3D::getCopy()
{
	return std::make_shared<CModel3D>(*this);
}

void CModel3D::info(std::wstring i[4])
{
	i[0] = m_label.toStdWString();
	i[1] = std::to_wstring( m_Id );
	i[2] = L"Model 3D";
	i[3] = infoRow();
}



void CModel3D::calcVN()
{
	if ( getChild()->type() != CObject::MESH ) return;

	if (auto m = std::dynamic_pointer_cast<CMesh>(this->getChild())) m->calcVN();
}

//unsigned int CModel3D::getNewId()
//{
//	unsigned int newId = this->m_Id + 1;
//
//	while ((m_annotations.find(newId) != m_annotations.end()) || (m_data.find(newId) != m_data.end())) newId++;
//
//	return newId;
//}

//int CModel3D::addAnnotation(CAnnotation * ad)
//{
//	//unsigned int newId = getNewId();
//	//ad->setId(newId);
//
//	m_annotations[ad->id()] = ad;
//	ad->setParent(this);
//
//	return ad->id();
//}
//
//CAnnotation * CModel3D::removeAnnotation( int id )
//{
//	CBaseObject *an = getSomethingWithId( id );
//	
//	if (an == nullptr) return nullptr;
//	if (an->category() != CBaseObject::Category::ANNOTATION) return nullptr;
//
//	CBaseObject* parent = an->getParent();
//
//	if (parent == this)
//	{
//		m_annotations.erase(id);
//	}
//	else
//	{
//		((CAnnotation*)parent)->removeAnnotation(id);
//	}
//
//	return (CAnnotation*) an;
//}
//
//CAnnotation * CModel3D::annotation(int id)
//{
//	Annotations::iterator it;
//	for (it = m_annotations.begin(); it != m_annotations.end(); it++)
//	{
//		if ( it->first == id ) return it->second;
//		CAnnotation* obj = it->second->annotation(id);
//		if (obj != nullptr) return obj;
//	}
//	return nullptr;
//}

#include "FileConnector.h"

std::shared_ptr<CModel3D> CModel3D::load(const QString fext, const QString path, bool synchronous)
{
	CParser* parser = CFileConnector::getLoadParser(fext);

	if (nullptr != parser)
	{
		//unsigned long t1, t2;

		// t1 = GetTickCount();
		auto t0 = std::chrono::steady_clock::now();

		std::shared_ptr<CModel3D> obj = parser->load(path, synchronous);

		// t2 = GetTickCount();
		auto t1 = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

		qInfo() << "Load time: " << duration;

		if (!parser->inPlugin()) delete parser;

		return obj;
	}

	return nullptr;
}

std::shared_ptr<CModel3D> CModel3D::load(const QString path, bool synchronous)
{
	CParser* parser = CFileConnector::getLoadParser( path );

	if (nullptr != parser)
	{
		// unsigned long t1, t2;

		// t1 = GetTickCount();
		auto t0 = std::chrono::steady_clock::now();

		std::shared_ptr<CModel3D> obj = parser->load( path, synchronous );

		// t2 = GetTickCount();
		auto t1 = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

		qInfo() << "Load time: " << duration;

		if (!parser->inPlugin()) delete parser;

		return obj;
	}

	return nullptr;
}

bool CModel3D::save(const char* path)
{
	return save( QString(path).toStdWString() );
}

bool CModel3D::save(const QString path)
{
	CParser* parser = CFileConnector::getSaveParser( path );

	if (NULL != parser)
	{
		//parser->set( UI::getNativePath(path), this );
		//bool result = parser->save();

		bool result = parser->save(std::dynamic_pointer_cast<CModel3D>(this->shared_from_this()), path);

		if (!parser->inPlugin()) delete parser;

		return result;
	}

	return CFileConnector::save( this, path );
}

bool CModel3D::save(const std::string path)
{
	return save( QString::fromStdString(path).toStdWString() );
}

#include "FileConnector.h"

bool CModel3D::save(std::wstring path)
{
	return save( QString::fromStdWString(path) );
}



//int CModel3D::setAnimation(int anim)
//{
//	if (bAnime = (0 != anim))
//		iAnimeDir = (anim<0) ? -1 : 1;
//
//	lastanim = GetTickCount64();
//
//	return bAnime ? iAnimeDir : 0;
//}
//
//int CModel3D::toggleAnimation()
//{
//	lastanim = GetTickCount64();
//	return (bAnime = !bAnime) ? iAnimeDir : 0;
//}
//
//int CModel3D::toggleAnimationDir()
//{
//	if (bAnime)
//		return iAnimeDir = -iAnimeDir;
//	return 0;
//}
//


//CBaseObject::Visibility CModel3D::setVisibility(CBaseObject::Visibility v)
//{
//	if (v == Visibility::HIDE_SELF)
//	{
//		return (m_visible = Visibility::HIDE_ALL);
//	}
//	return (m_visible = v);
//}
//
//CBaseObject::Visibility CModel3D::switchVisibility()
//{
//	if 	((m_visible == HIDE_ALL) || (m_visible == HIDE_SELF))
//		return m_visible = SHOW;
//	else
//		return m_visible = HIDE_ALL;
//}
