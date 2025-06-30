#include "stdafx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Material( FILE *plik )
{
	char bufor[80];

	do
	{
		float myf1, myf2, myf3;
		fscanf(plik, " %s %f %f %f", bufor, &myf1, &myf2, &myf3);

		if (m_vrmlVersion == 1)
		{
			if (strstr(bufor, "ambientColor"))
			{
				pMeshData->getMaterial().FrontColor.ambient.setRGBf( myf1, myf2, myf3 );
			}
		}
		else
		{
			if (strstr(bufor, "ambientIntensity"))
			{
				pMeshData->getMaterial().FrontColor.ambient.setRGBf(myf1, myf1, myf1);
			}
		}

		if (strstr(bufor, "diffuseColor"))
		{
			pMeshData->getMaterial().FrontColor.diffuse.setRGBf(myf1, myf2, myf3);
		}

		if (strstr(bufor, "emissiveColor"))
		{
			pMeshData->getMaterial().FrontColor.emission.setRGBf(myf1, myf2, myf3);
		}

		if (strstr(bufor, "specularColor"))
		{
			pMeshData->getMaterial().FrontColor.specular.setRGBf(myf1, myf2, myf3);
		}

		if (strstr(bufor, "shininess"))
		{
			pMeshData->getMaterial().FrontColor.shininess = myf1;
		}

		if (strstr(bufor, "transparency"))
		{
			pMeshData->getMaterial().FrontColor.transparency = myf1;
			pMeshData->getMaterial().FrontColor.ambient.fA( 1.0 - myf1 );
			pMeshData->getMaterial().FrontColor.diffuse.fA( 1.0 - myf1 );
			pMeshData->getMaterial().FrontColor.specular.fA( 1.0 - myf1 );
		}

	} while (!strstr(bufor, "}"));
}
