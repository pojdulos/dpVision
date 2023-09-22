#pragma once

class CRGBA {
public:
	float w[4];

	CRGBA( float r=0.0, float g=0.0, float b=0.0, float a=1.0 )	{ Set( r, g, b, a ); };

	inline void Set( float r=0.0, float g=0.0, float b=0.0, float a=1.0 )
	{
		w[0] = r;
		w[1] = g;
		w[2] = b;
		w[3] = a;
	};

	inline float R() { return w[0]; };
	inline float R( float ar ) { return w[0]=ar; };

	inline float G() { return w[1]; };
	inline float G( float ag ) { return w[1]=ag; };

	inline float B() { return w[2]; };
	inline float B( float ab ) { return w[2]=ab; };

	inline float A() { return w[3]; };
	inline float A( float aa ) { return w[3]=aa; };
};
