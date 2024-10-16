#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Separator( FILE *plik )
{
	char bufor[80];
	char ch; 
	do
	{
		//do { ch = fgetc( plik ); } while ( ch == ' ' ); 
		ch = VRML_SkipWhiteChars(plik);
		
		switch (ch)
		{
			case '#': // komentarz do koñca linii
				if (!VRML_GoToNextLine(plik)) return;
				break;
			case '}': // koniec Noda
				return;
				break;
			case 'S': // zagnie¿d¿ony Separator
				fscanf(plik, "%s { ", bufor);
				if (strstr(bufor, "eparator"))
				{
					VRML_ParseNode_Separator(plik);
				}
				break;
			case 'M': // Material
				fscanf(plik, "%s { ", bufor);
				if (strstr(bufor, "aterial"))
				{
					VRML_ParseNode_Material(plik);
				}
				break;
			case 'C': // Coordinate3
				fscanf(plik, "%s {  ", bufor);
				if (strstr(bufor, "oordinate3"))
				{
					VRML_ParseNode_Coordinate(plik);
				}
				break;
			case 'I': // IndexedFaceSet
				fscanf(plik, "%s { ", bufor);
				if (strstr(bufor, "ndexedFaceSet"))
				{
					VRML_ParseNode_IndexedFaceSet(plik);
				}
				break;
			case '{': // to nie powinno wyst¹piæ
				VRML_IgnoreNode(plik, '{', '}');
				break;
		}
	}
	while ( ! feof( plik ) ); 
}
