#include "Workspace.h"
//#include "WorkspaceXML.h"
#include "WorkspaceFile.h"

#include "../renderers/IWorkspaceRenderer.h"
#include "StatusBarManager.h"

#include <algorithm>
#include <chrono>

CWorkspace::CWorkspace() //: QObject()
{
	m_bOK = true;
	m_idOfCurrentModel = NO_CURRENT_MODEL;

	renderer_ = std::make_shared<IWorkspaceRenderer>();

	m_orderedIds.clear();
	m_checkedIds.clear();

	InitLights();
}

CWorkspace* CWorkspace::instance()
{
	static CWorkspace instance;
	return &instance;
}

void CWorkspace::clear()
{
	m_orderedIds.clear();
	m_checkedIds.clear();
	m_data.clear();

	_objectActivate(NO_CURRENT_MODEL);
}

/*
	Add an existing Model to the Workspace
*/
bool CWorkspace::_addModel(std::shared_ptr<CModel3D> pMdlR)
{
	if (nullptr == pMdlR) return false;

	try {
		this->m_data.insert(CWorkspace::value_type(pMdlR->id(), pMdlR));
		pMdlR->setParent(nullptr);
		m_orderedIds.append(pMdlR->id());
	}
	catch (std::bad_alloc& e) {
		return false;
	}

	m_bOK = true;

	return true;
}

bool CWorkspace::_removeAllModels()
{
	m_orderedIds.clear();
	for (Children::iterator it = m_data.begin(); it != m_data.end();)
	{
		it = m_data.erase(it);
	}
	m_checkedIds.clear();

	_objectActivate(NO_CURRENT_MODEL);

	return true;
}

std::shared_ptr<CWorkspace::ChildType> CWorkspace::_getModel(int i)
{
	if (NO_CURRENT_MODEL == i) return nullptr;

	Children::iterator it = m_data.find(i);

	if (it == m_data.end()) return nullptr;

	return m_data[i];
}

std::shared_ptr<CWorkspace::ChildType> CWorkspace::first()
{
	for (int id : m_orderedIds.ids())
	{
		if (auto obj = _getModel(id))
		{
			return obj;
		}
	}

	return nullptr;
}

std::shared_ptr<CWorkspace::ChildType> CWorkspace::last()
{
	for (std::vector<int>::const_reverse_iterator it = m_orderedIds.ids().rbegin(); it != m_orderedIds.ids().rend(); ++it)
	{
		if (auto obj = _getModel(*it))
		{
			return obj;
		}
	}

	return nullptr;
}

bool CWorkspace::isChecked(int id) const
{
	return m_checkedIds.end() != std::find(m_checkedIds.begin(), m_checkedIds.end(), id);
}

int CWorkspace::checkedIndex(int id) const
{
	std::list<int>::const_iterator it = std::find(m_checkedIds.begin(), m_checkedIds.end(), id);
	if (it != m_checkedIds.end())
	{
		return (int)std::distance(m_checkedIds.begin(), it);
	}

	return -1;
}

void CWorkspace::addChecked(int id)
{
	if (isChecked(id))
	{
		return;
	}

	auto obj = getSomethingWithId(id);
	if (obj == nullptr)
	{
		return;
	}

	m_checkedIds.push_back(id);
	obj->setChecked(true);

	StatusBarManager::setText(QString("Object checked (pos.: %1)").arg(checkedIndex(id)));
}

void CWorkspace::removeChecked(int id)
{
	if (!isChecked(id))
	{
		return;
	}

	StatusBarManager::setText(QString("Object at pos.: %1 unchecked").arg(checkedIndex(id)));
	m_checkedIds.remove(id);

	if (auto obj = getSomethingWithId(id))
	{
		obj->setChecked(false);
	}
}

bool CWorkspace::setChecked(int id, bool checked)
{
	if (checked == isChecked(id))
	{
		return false;
	}

	if (checked)
	{
		addChecked(id);
	}
	else
	{
		removeChecked(id);
	}

	notifyObjectStateChanged(id);
	return true;
}

std::list<int> CWorkspace::checkedIds() const
{
	std::list<int> result;
	for (auto id : m_checkedIds)
	{
		Children::const_iterator it = m_data.find(id);
		if ((it != m_data.end()) && (it->second->hasType(CObject::MODEL)))
		{
			result.push_back(id);
		}
	}

	return result;
}

std::list<int> CWorkspace::checkedIds(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> dad) const
{
	std::list<int> result;
	for (auto id : m_checkedIds)
	{
		std::shared_ptr<CBaseObject> kid;

		if (dad == nullptr)
			kid = getSomethingWithId(id);
		else if (dad->id() == id)
			kid = dad;
		else
			kid = dad->getSomethingWithId(id);

		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end()))
		{
			result.push_back(id);
		}
	}

	return result;
}

std::list<std::shared_ptr<CBaseObject>> CWorkspace::checkedObjects(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> dad) const
{
	std::list<std::shared_ptr<CBaseObject>> result;
	for (auto id : m_checkedIds)
	{
		std::shared_ptr<CBaseObject> kid;

		if (dad == nullptr)
			kid = getSomethingWithId(id);
		else if (dad->id() == id)
			kid = dad;
		else
			kid = dad->getSomethingWithId(id);

		if ((kid != nullptr) && (types.empty() || types.find((CBaseObject::Type)kid->type()) != types.end()))
		{
			result.push_back(kid);
		}
	}

	return result;
}

void CWorkspace::clearChecked()
{
	if (m_checkedIds.empty())
	{
		return;
	}

	for (auto id : m_checkedIds)
	{
		if (auto obj = getSomethingWithId(id))
		{
			obj->setChecked(false);
		}
	}

	m_checkedIds.clear();
}

void CWorkspace::InitLights()
{
	m_lights[0].light = GL_LIGHT0;
	m_lights[1].light = GL_LIGHT1;
	m_lights[2].light = GL_LIGHT2;
	m_lights[3].light = GL_LIGHT3;
	m_lights[4].light = GL_LIGHT4;
	m_lights[5].light = GL_LIGHT5;
	m_lights[6].light = GL_LIGHT6;
	m_lights[7].light = GL_LIGHT7;

	m_lights[0].setAmbient(0.6f, 0.6f, 0.6f, 1.0f);
	m_lights[0].setDiffuse(0.6f, 0.6f, 0.6f, 1.0f);
	m_lights[0].setSpecular(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[0].setPosition(0.0f, 0.0f, 1000.0f, 0.0f);
	m_lights[0].setSpot(-5000.0f, -20000.0f, -100000.0f, 180.0f);

	m_lights[0].fixed = true;
	m_lights[0].active = true;

	m_lights[1].setAmbient(0.2f, 0.2f, 0.2f, 1.0f);
	m_lights[1].setDiffuse(0.4f, 0.4f, 0.4f, 1.0f);
	m_lights[1].setSpecular(0.6f, 0.6f, 0.6f, 1.0f);
	m_lights[1].setPosition(20000.0f, 0.0f, -20000.0f, 1.0f);
	m_lights[1].setSpot(-20000.0f, 0.0f, 20000.0f, 60.0f);

	m_lights[1].fixed = false;
	m_lights[1].active = false;
}

int CWorkspace::_setNextModelCurrent()
{
	int nextId = -1;
	auto current = std::find(m_orderedIds.ids().begin(), m_orderedIds.ids().end(), m_idOfCurrentModel);
	if (current != m_orderedIds.ids().end())
	{
		++current;
		while (current != m_orderedIds.ids().end())
		{
			if (_getModel(*current) != nullptr)
			{
				nextId = *current;
				break;
			}
			++current;
		}
	}

	_objectActivate(nextId);
	return m_idOfCurrentModel;
}

int CWorkspace::_setPreviousModelCurrent()
{
	int previousId = -1;
	auto current = std::find(m_orderedIds.ids().begin(), m_orderedIds.ids().end(), m_idOfCurrentModel);
	if (current != m_orderedIds.ids().begin() && current != m_orderedIds.ids().end())
	{
		do
		{
			--current;
			if (_getModel(*current) != nullptr)
			{
				previousId = *current;
				break;
			}
		} while (current != m_orderedIds.ids().begin());
	}

	_objectActivate(previousId);
	return m_idOfCurrentModel;
}

std::shared_ptr<CBaseObject> CWorkspace::getSomethingWithId(int id) const
{
	for (const auto& d : m_data)
	{
		if (id == d.second->id()) return d.second;
		else if (auto result = d.second->getSomethingWithId(id)) return result;
	}
	return nullptr;
}

void CWorkspace::render()
{
	if (renderer_) renderer_->render(this);
}

void CWorkspace::renderLights(bool perm)
{
	if (renderer_) renderer_->renderLights(this, perm);
}

void CWorkspace::reset()
{
	if (this->m_idOfCurrentModel < 0)
	{
		CWorkspace::iterator siatka;
		for (auto& siatka : this->m_data)
		{
			if (siatka.second->hasTransformation()) siatka.second->transform().reset();
		}
	}
	else if (!this->m_data.empty())
	{
		this->m_data[this->m_idOfCurrentModel]->transform().reset();
	}
}

std::vector<CRGBA> CWorkspace::getXRayImage(CPoint3f pkt0, int size)
{
	std::vector<CRGBA> result;

	result.reserve(size * size);

	if (m_idOfCurrentModel != NO_CURRENT_MODEL)
	{
		std::shared_ptr<CModel3D> obj = _getModel(m_idOfCurrentModel);
		if (obj == nullptr) return result;

		pkt0 = obj->getTransform().world2local(pkt0);

		std::shared_ptr<CMesh> mesh = std::dynamic_pointer_cast<CMesh>(obj->getChild());

		std::vector<CVector3f> x = mesh->getVectors(pkt0);

		auto t0 = std::chrono::steady_clock::now();

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				float maxLen;

				float dx = ((float)(x - size / 2)) / 100;
				float dy = ((float)(y - size / 2)) / 100;

				CPoint3f pkt1 = obj->getTransform().origin() + CPoint3f(CTriple<float>(dx, dy, 0.0f));

				CVector3f vRay(pkt0, pkt1);
				vRay.normalize();

				float iL = mesh->getInteriorLength(pkt0, vRay, maxLen);

				CRGBA col(0.0f, 0.0f, 0.0f, 0.0f);

				if (iL > 0)
				{
					col.SetFloat(iL / maxLen, iL / maxLen, iL / maxLen, 1.0f);
				}

				result.push_back(col);

				auto t1 = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

				StatusBarManager::setText(QString("Tested point [%1,%2] iL=%3, maxLen=%4 (time:%5)").arg(x).arg(y).arg(iL).arg(maxLen).arg(duration));
			}
		}
	}

	return result;
}

CBoundingBox CWorkspace::topBB()
{
	CBoundingBox bb;

	for (int id : m_orderedIds.ids())
	{
		std::shared_ptr<CModel3D> model = _getModel(id);
		if (model == nullptr)
		{
			continue;
		}

		CPoint3d min = model->getMin();
		CPoint3d max = model->getMax();

		Eigen::Matrix4d T = CBaseObject::getGlobalTransformationMatrix(model);

		CPoint3d m1 = T * min;
		CPoint3d m2 = T * CPoint3d(min.x, min.y, max.z);
		CPoint3d m3 = T * CPoint3d(min.x, max.y, max.z);
		CPoint3d m4 = T * CPoint3d(min.x, max.y, min.z);
		CPoint3d m5 = T * CPoint3d(max.x, max.y, min.z);
		CPoint3d m6 = T * CPoint3d(max.x, min.y, min.z);
		CPoint3d m7 = T * CPoint3d(max.x, min.y, max.z);
		CPoint3d m8 = T * max;

		bb.expand(m1);
		bb.expand(m2);
		bb.expand(m3);
		bb.expand(m4);
		bb.expand(m5);
		bb.expand(m6);
		bb.expand(m7);
		bb.expand(m8);
	}

	return bb;
}
