#include "BackgroundPlane.h"

#include "AP.h"

#include "Global.h"

CBackgroundPlane::CBackgroundPlane(void)
{
	iBgCount = 0;
	iCurrentBG = 0;
	fDepth = _DP_Z_TLA;
}

CBackgroundPlane::~CBackgroundPlane(void)
{
//	this->vImages.clear();
}

void CBackgroundPlane::DrawRect( RECT b, RECT t )
{
	glBegin(GL_QUADS);
	glTexCoord2f( t.left, t.top );
	glVertex2f( b.left, b.top );

	glTexCoord2f( t.right, t.top );
	glVertex2f( b.right, t.top );

	glTexCoord2f( t.right, t.bottom );
	glVertex2f( b.right, b.bottom );

	glTexCoord2f( t.left, t.bottom );
	glVertex2f( b.left, b.bottom );
	glEnd();
}

void CBackgroundPlane::DrawRect( float l, float t, float r, float b, float tl, float tt, float tr, float tb  )
{
	glBegin(GL_QUADS);
		glTexCoord2f(tl,tt);
		glVertex2f( l, t );

		glTexCoord2f(tr,tt);
		glVertex2f( r, t );

		glTexCoord2f(tr,tb);
		glVertex2f( r, b );

		glTexCoord2f(tl,tb);
		glVertex2f( l, b );
	glEnd();
}

void CBackgroundPlane::Render( int type )
{
	if ( iCurrentBG < 0 ) return;

	float fShiftX = 0.0;

	switch ( type )
	{
		case LEFT_IMAGE:
			fShiftX = vImages[iCurrentBG].bSwapImages ? 0.5f : 0.0f;
			break;
		case RIGHT_IMAGE:
			fShiftX = vImages[iCurrentBG].bSwapImages ? 0.0f : 0.5f;
			break;
		case SINGLE_IMAGE:
		default:
			fShiftX = 0.0;
			break;
	}

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	
	//glClearAccum( 0.0f, 0.0f, 0.0f, 0.0f );
	//glClear(GL_ACCUM_BUFFER_BIT);

	

	glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_BACK,GL_AMBIENT_AND_DIFFUSE);	
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	
	glColor3f( 1.0f, 1.0f, 1.0f );
	
	int tst;

	tst = vImages[iCurrentBG].tex->textureId();
	glBindTexture(GL_TEXTURE_2D, tst );
	glEnable(GL_TEXTURE_2D);

	float bgRtX;
	float bgX, bgY;

	bgX = fShiftX ? vImages[iCurrentBG].RightImgHShift : 0.0f;
	bgY = fShiftX ? vImages[iCurrentBG].RightImgVShift : 0.0f;

	if ( vImages[iCurrentBG].ImageIsStereoPair )
	{
		bgRtX = 0.5f;
	}
	else
	{
		bgRtX = 1.0;
		fShiftX = 0.0;
	}

    glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
		glTranslatef( 0, 0, 0 );
//		glTranslatef( bgX, bgY, fDepth );
		DrawRect(
			-4, -3,
			4, 3,
			0, 0, 1, 1 );
//		DrawRect(
//			-vImages[iCurrentBG].SizeX, -vImages[iCurrentBG].SizeY,
//			vImages[iCurrentBG].SizeX, vImages[iCurrentBG].SizeY,
//			fShiftX+0, 0, fShiftX+bgRtX, 1 );
    glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	//glDisable(GL_TEXTURE_2D);

	glDisable(GL_COLOR_MATERIAL);

	//glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	//glAccum(GL_LOAD,1.0);

	glPopAttrib();
}

bool CBackgroundPlane::AddImage( const char *f, int sX, int sY, int IsStereo, int RHSh, int RVSh )
{
	SBgImage item;

	if ( iBgCount >= 25 ) return false;

	strncpy( vImages[iBgCount].Path, f, 255 );
	vImages[iBgCount].path = QString( f );
	vImages[iBgCount].SizeX = sX;
	vImages[iBgCount].SizeY = sY;
	vImages[iBgCount].ImageIsStereoPair = IsStereo;
	vImages[iBgCount].RightImgHShift = RHSh;
	vImages[iBgCount].RightImgVShift = RVSh;
	vImages[iBgCount].bSwapImages = false;

	CTexture* tex = vImages[iBgCount].tex = new CTexture( vImages[iBgCount].path );

	if ( tex != nullptr )
	{
		//this->vImages.push_back( item );
		iBgCount++;
		return true;
	}
	else
	{
		vImages[iBgCount].path = QString(AP::getExeFilePath() + "/" + QFileInfo(vImages[iBgCount].path).path() + "/" + QFileInfo(vImages[iBgCount].path).fileName());

		tex = vImages[iBgCount].tex = new CTexture( vImages[iBgCount].path );
		if ( tex != nullptr )
		{
			//this->vImages.push_back( item );
			iBgCount++;
			return true;
		}
	}
	return false;
}

int CBackgroundPlane::SelectNext()
{
	iCurrentBG++;
	if ( iCurrentBG >= iBgCount ) iCurrentBG = -1;
	return iCurrentBG;
}

int CBackgroundPlane::SelectNone()
{
	return iCurrentBG = -1;
}

int CBackgroundPlane::SelectPrev()
{
	iCurrentBG--;
	if ( iCurrentBG < -1 ) iCurrentBG = ( iBgCount - 1 );
	return iCurrentBG;
}

const char *CBackgroundPlane::GetCurrentName()
{
	if ( iCurrentBG < 0 ) return "";
	return vImages[iCurrentBG].Path;
}

int CBackgroundPlane::GetCount()
{
	return iBgCount;
}

bool CBackgroundPlane::SwapImages()
{
	if ( iCurrentBG < 0 ) return false;
	return vImages[iCurrentBG].bSwapImages = !vImages[iCurrentBG].bSwapImages;
}

bool CBackgroundPlane::ToggleStereo()
{
	if ( iCurrentBG < 0 ) return false;
	return vImages[iCurrentBG].ImageIsStereoPair = !vImages[iCurrentBG].ImageIsStereoPair;
}


int CBackgroundPlane::AddHorizontalShift( int delta )
{
	if ( iCurrentBG < 0 ) return 0;
	return vImages[iCurrentBG].RightImgHShift += delta;
}

int CBackgroundPlane::AddVerticalShift( int delta )
{
	if ( iCurrentBG < 0 ) return 0;
	return vImages[iCurrentBG].RightImgVShift += delta;
}