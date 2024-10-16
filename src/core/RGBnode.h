#pragma once
#ifndef _RGBNODE_H_BY_DP_INCLUDED_
#define _RGBNODE_H_BY_DP_INCLUDED_

#include "Triple.h"

class CRGBnode : public CTriple<unsigned char> {
public:
	CRGBnode() { x=0; y=0; z=0; };
	CRGBnode( unsigned char r, unsigned char g, unsigned char b ) { x=r; y=g; z=b; };
	CRGBnode( float fr, float fg, float fb )
	{
		x = (int)(fr*25500)/100;
		y = (int)(fg*25500)/100;
		x = (int)(fb*25500)/100;
	};

	inline void Set( unsigned char r, unsigned char g, unsigned char b )
	{
		x = r;
		y = g;
		z = b;
	};

	inline void SetFloat( float fr, float fg, float fb )
	{
		x = (int)(fr*25500.0f)/100;
		y = (int)(fg*25500.0f)/100;
		z = (int)(fb*25500.0f)/100;
	};

	inline unsigned char R() const { return x; };
	inline unsigned char R( const unsigned char ax ) { return x=ax; };

	inline unsigned char G() const { return y; };
	inline unsigned char G( const unsigned char ay ) { return y=ay; };

	inline unsigned char B() const { return z; };
	inline unsigned char B( const unsigned char az ) { return z=az; };

	unsigned char* V() const { return (unsigned char*)this; };

	float fR() { return (float)x/255; };
	float fG() { return (float)y/255; };
	float fB() { return (float)z/255; };

	float* fV( float bufor[3] ) {
		bufor[0] = (float)x/255;
		bufor[1] = (float)y/255;
		bufor[2] = (float)z/255;
		return bufor;
	};

};

//class CArrayOfRGB : public std::vector<CRGBnode> {};

#endif /* _RGBNODE_H_BY_DP_INCLUDED_ */