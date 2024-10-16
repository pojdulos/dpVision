#pragma once
#ifndef DPVISION_GLOBAL_DEFINITIONS_H
#define DPVISION_GLOBAL_DEFINITIONS_H

#define INIFILE "dpVision.ini"
#define APPNAME "dpVision"

#ifndef PI
#define PI       3.1415926535897932384626433832795
#endif

#ifndef PI_180
#define PI_180	(PI/180.0)
#endif

#ifndef PI_2
#define PI_2     1.5707963267948966192313216916398
#endif

#ifndef MAX_FLOAT
#define MAX_FLOAT 3.402823e38f
#endif

#ifndef MIN_FLOAT
#define MIN_FLOAT -3.402823e38f
#endif

#include <stdint.h>

#ifndef INDEX_TYPE
#define INDEX_TYPE uint32_t
#endif


#define TEMPORARY_ID 0000000000
#define MODEL_ID_OFFSET 1000000
//#define MODEL_DATA_SPACE 1000000
#define NO_CURRENT_MODEL -1
#define PLUGIN_ID_OFFSET 2000000000

#define SINGLE_IMAGE 2
#define LEFT_IMAGE 1
#define RIGHT_IMAGE 0

#define _DEF_AMBIENT 0.2f, 0.2f, 0.2f, 1.0f
#define _DEF_DIFFUSE 0.8f, 0.8f, 0.8f, 1.0f
#define _DEF_SPECULAR 0.0f, 0.0f, 0.0f, 1.0f
#define _DEF_EMISSION 0.0f, 0.0f, 0.0f, 1.0f
#define _DEF_SHININESS 0.0f

#define _DP_Z_TLA -9000.0f

//typedef union {
//	unsigned int opt0;
//	unsigned int opt1;
//	struct {
//		unsigned opt2_1 :1;
//		unsigned opt2_2 :1;
//		unsigned opt2_3 :1;
//		unsigned opt2_4 :1;
//		unsigned	:0;		// wyrównanie do unsigned int
//		unsigned opt3_1 :1;
//		unsigned opt3_2 :1;
//	};
//} _MyOpt;

#endif //DPVISION_GLOBAL_DEFINITIONS_H