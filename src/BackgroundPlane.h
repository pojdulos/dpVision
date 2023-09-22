#pragma once

#include "Texture.h"
#include "DPFileInfo.h"

struct __declspec(dllexport) SBgImage {
	char Path[255];
	QString path;
	int SizeX;
	int SizeY;
	int ImageIsStereoPair;
	int RightImgHShift;
	int RightImgVShift;
	bool bSwapImages;
	CTexture* tex;
};

class __declspec(dllexport) CBackgroundPlane
{
	//std::vector<SBgImage> vImages;
	SBgImage vImages[25];

	int iBgCount;
	int iCurrentBG;
	float fDepth;

public:
	CBackgroundPlane(void);
	~CBackgroundPlane(void);

	bool AddImage( const char *f, int sX, int sY, int IsStereo, int RHSh, int RVSh );

	void DrawRect( float l, float t, float r, float b, float tl, float tt, float tr, float tb  );
	void DrawRect( RECT b, RECT t );
	void Render( int type );
	int SelectNext();
	int SelectNone();
	int SelectPrev();
	const char *GetCurrentName();
	int GetCount();
	bool SwapImages();
	bool ToggleStereo();
	int AddHorizontalShift( int delta );
	int AddVerticalShift( int delta );
};
