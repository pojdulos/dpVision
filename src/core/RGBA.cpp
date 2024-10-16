#include "RGBA.h"


//inline std::string fromHexRGBA( std::string sValue ) {
//	unsigned int x;
//	std::stringstream ss;
//	ss << std::hex << sValue;
//	ss >> x;
//	
//	unsigned int x = std::stoul(sValue, nullptr, 16);
//	
//	//QString sValue = "FF";
//	//bool bStatus = false;
//	//uint nHex = sValue.toUInt(&bStatus, 16);
//	//Result: nHex == 255, bStatus == true
//}

CRGBA CRGBA::fromHexRGBA(std::string sValue)
{
	if (sValue[0] == '#')
	{
		sValue = sValue.substr(1, sValue.size() - 1);
	}

	unsigned int x;
	std::stringstream ss;
	ss << std::hex << sValue;
	ss >> x;
	CRGBA c = CRGBA((unsigned char)((x >> 24) & 0xff), (unsigned char)((x >> 16) & 0xff), (unsigned char)((x >> 8) & 0xff), (unsigned char)(x & 0xff));
	
	return c;
}

std::string CRGBA::asHexRGBA()
{
	char c[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	char buf[10] = { '#', c[m_r / 16], c[m_r % 16], c[m_g / 16], c[m_g % 16], c[m_b / 16], c[m_b % 16], c[m_a / 16], c[m_a % 16], 0 };
	return std::string(buf);
}