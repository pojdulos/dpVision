#include "Material.h"

CMaterial::CMaterial()
{
	tekstura = nullptr;
	FrontColor.ambient.SetFloat(_DEF_AMBIENT);
	FrontColor.diffuse.SetFloat(_DEF_DIFFUSE);
	FrontColor.specular.SetFloat(_DEF_SPECULAR);
	FrontColor.emission.SetFloat(_DEF_EMISSION);
	FrontColor.shininess = _DEF_SHININESS;
	FrontColor.transparency = 0.0;

	BackColor.ambient.Set(0, 0, 0, 255);
	BackColor.diffuse.Set(64, 64, 64, 255);
	BackColor.specular.SetFloat(0, 0, 0, 255);
	BackColor.emission.SetFloat(0, 0, 0, 255);
	BackColor.shininess = 0.0;
	BackColor.transparency = 0.0;

	m_force = false;
}

CMaterial::CMaterial(CMaterial &m)
{
	BackColor = SGLColors(m.BackColor);
	FrontColor = SGLColors(m.FrontColor);

	texcoord = std::vector<CTCoord>(m.texcoord);
	texindex = std::vector<CTIndex>(m.texindex);

	TexInfo = m.TexInfo;

	if (m.tekstura != nullptr)
		tekstura = new CTexture(*m.tekstura);
	else
		tekstura = nullptr;

	m_force = m.m_force;
}


CMaterial::~CMaterial()
{
	texindex.clear();
	texcoord.clear();
}


bool CMaterial::attachTexture(const QString &dir )
{
	if (!QFileInfo(TexInfo).exists())
	{
		TexInfo = dir + "/" + QFileInfo(TexInfo).fileName();
		if (!QFileInfo(TexInfo).exists())
		{
			return false;
		}
	}

	tekstura = new CTexture( TexInfo );
	return tekstura != nullptr;
}
