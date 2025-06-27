#pragma once
#ifndef _RGBA_H_BY_DP_INCLUDED_
#define _RGBA_H_BY_DP_INCLUDED_

#include "dll_global.h"


#include <sstream>
#include <iostream>
#include <stdint.h>

class DPVISION_EXPORT CRGBA {
	union {
		uint32_t m_rgba;
		unsigned char m_v[4];
		struct {
			unsigned char m_r;
			unsigned char m_g;
			unsigned char m_b;
			unsigned char m_a;
		};
	};

public:
	CRGBA() : m_v{0,0,0,255} {};
	CRGBA(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca=255) : m_v{cr,cg,cb,ca} {};

	CRGBA( float fr, float fg, float fb, float fa=1.0f )	{
		m_v[0] = (int)(fr*25500)/100;
		m_v[1] = (int)(fg*25500)/100;
		m_v[2] = (int)(fb*25500)/100;
		m_v[3] = (int)(fa*25500)/100;
	};



	// Sets RGB values, Alpha is not changed
	inline void setRGB(unsigned char cr, unsigned char cg, unsigned char cb)
	{
		m_v[0] = cr;
		m_v[1] = cg;
		m_v[2] = cb;
	};

	// Sets RGBA values, if Alpha is not specified, is set to 255
	inline void setRGBA(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca=255)
	{
		m_v[0] = cr;
		m_v[1] = cg;
		m_v[2] = cb;
		m_v[3] = ca;
	};

	// Sets RGB as float values from range [0.0,1.0], Alpha is not changed
	inline void setRGBf(float fr, float fg, float fb)
	{
		m_v[0] = (int)(fr*25500.0f) / 100;
		m_v[1] = (int)(fg*25500.0f) / 100;
		m_v[2] = (int)(fb*25500.0f) / 100;
	};

	// Sets RGBA as float values from range [0.0,1.0], if Alpha is not specified, is set to 1.0
	inline void setRGBAf(float fr, float fg, float fb, float fa=1.0f)
	{
		m_v[0] = (int)(fr*25500.0f) / 100;
		m_v[1] = (int)(fg*25500.0f) / 100;
		m_v[2] = (int)(fb*25500.0f) / 100;
		m_v[3] = (int)(fa*25500.0f) / 100;
	};



	// deprecated, use setRGBA() instead
	inline void Set(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca) { setRGBA(cr, cg, cb, ca); };

	// deprecated, use setRGBAf() instead
	inline void SetFloat( float fr, float fg, float fb, float fa ) { setRGBAf( fr, fg, fb, fa ); };

	inline unsigned char R( unsigned char ar ) { return m_v[0]=ar; };
	inline unsigned char G( unsigned char ag ) { return m_v[1]=ag; };
	inline unsigned char B( unsigned char ab ) { return m_v[2]=ab; };
	inline unsigned char A( unsigned char aa ) { return m_v[3]=aa; };

	unsigned char R() { return m_v[0]; };
	unsigned char G() { return m_v[1]; };
	unsigned char B() { return m_v[2]; };
	unsigned char A() { return m_v[3]; };

	unsigned char red() { return m_r; };
	unsigned char green() { return m_g; };
	unsigned char blue() { return m_b; };
	unsigned char alpha() { return m_a; };

	inline unsigned char setRed(unsigned char ar) { return m_r = ar; };
	inline unsigned char setGreen(unsigned char ag) { return m_g = ag; };
	inline unsigned char setBlue(unsigned char ab) { return m_b = ab; };
	inline unsigned char setAlpha(unsigned char aa) { return m_a = aa; };

	inline unsigned char setGray8(unsigned char gray) { m_a = 255; return m_r = m_g = m_b = gray; };
	inline unsigned char setGray16(uint16_t gray) { m_a = 255; return m_r = m_g = m_b = ( gray >> 8 ); };

	unsigned char* V() { return m_v; };

	float fR(float ar) { m_r = (unsigned char)(ar * 255.0f); return fR(); };
	float fG(float ag) { m_g = (unsigned char)(ag * 255.0f); return fG(); };
	float fB(float ab) { m_b = (unsigned char)(ab * 255.0f); return fB(); };
	float fA(float aa) { m_a = (unsigned char)(aa * 255.0f); return fA(); };

	float fR() { return (float)m_v[0]/255; };
	float fG() { return (float)m_v[1]/255; };
	float fB() { return (float)m_v[2]/255; };
	float fA() { return (float)m_v[3]/255; };

	float* fV( float bufor[4] ) {
		bufor[0] = (float)m_v[0]/255;
		bufor[1] = (float)m_v[1]/255;
		bufor[2] = (float)m_v[2]/255;
		bufor[3] = (float)m_v[3]/255;
		return bufor;
	};

	inline static CRGBA fromArgb32( unsigned char t[4] ) {
		return CRGBA(	t[2], //R
						t[1], //G
						t[0], //B
						t[3]); //A
	}

	inline static CRGBA fromArgb32( uint32_t argb ) {
		unsigned char * t = (unsigned char *)&argb;
		return CRGBA(	t[2], //R
						t[1], //G
						t[0], //B
						t[3]); //A
	}

	inline uint32_t toArgb32() {
		uint32_t argb = m_v[3];  //A
		argb <<= 8;
		argb |= m_v[0]; //R
		argb <<= 8;
		argb |= m_v[1]; //G
		argb <<= 8;
		argb |= m_v[2]; //B

		return argb;
	}
	
	static CRGBA fromHexRGBA(std::string sValue);
	std::string asHexRGBA();
};

//class DPVISION_EXPORT CArray_OfRGBA : public std::vector<CRGBA> {};

#endif /* _RGBA_H_BY_DP_INCLUDED_ */