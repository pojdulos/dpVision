#pragma once

#include "dll_global.h"

#include "Global.h"

#include "RGBA.h"
#include "Texture.h"
#include "TCoord.h"
#include "TIndex.h"
#include "DPFileInfo.h"

struct SGLColors
{
	CRGBA ambient;
	CRGBA diffuse;
	CRGBA specular;
	CRGBA emission;
	float shininess;
	float transparency;
};


class CMaterial
{
public:
	CMaterial();
	CMaterial( CMaterial &m );
	~CMaterial();

	bool m_force;

	typedef CTexture Texture;
	typedef std::vector<CTCoord> TextureCoordinates;
	typedef std::vector<CTIndex> TextureIndexes;

	// TEXTURE DATA ---------
	CMaterial::TextureCoordinates texcoord;
	CMaterial::TextureIndexes texindex;
	CMaterial::Texture* tekstura;
	QString	TexInfo;
	// ----------------------
	// SOME OPENGL EXTENTIONS --------
	SGLColors FrontColor;
	SGLColors BackColor;
	// -------------------------------

	bool attachTexture(const QString &);

	inline bool hasTextureIndexes() { return ! texindex.empty(); };
	inline bool hasTextureCoords() { return ! texcoord.empty(); };

	inline bool hasTexture() { return (tekstura != nullptr) && tekstura->isCreated() && hasTextureCoords() && hasTextureIndexes(); };
};

