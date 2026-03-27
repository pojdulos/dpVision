#include "Workspace.h"

#include "Model3D.h"
#include "Image.h"
#include "dpLog.h"

namespace {
	void syncCheckedStateOnAdd(CWorkspace* workspace, const std::shared_ptr<CBaseObject>& obj)
	{
		if (obj == nullptr)
		{
			return;
		}

		if (obj->isChecked())
		{
			workspace->addChecked(obj->id());
		}

		if (auto object = std::dynamic_pointer_cast<CObject>(obj))
		{
			for (const auto& child : object->children())
			{
				syncCheckedStateOnAdd(workspace, child.second);
			}

			for (const auto& annotation : object->annotations())
			{
				syncCheckedStateOnAdd(workspace, annotation.second);
			}
		}
		else if (auto annotation = std::dynamic_pointer_cast<CAnnotation>(obj))
		{
			for (const auto& child : annotation->annotations())
			{
				syncCheckedStateOnAdd(workspace, child.second);
			}
		}
	}

	void clearCheckedStateOnRemove(CWorkspace* workspace, const std::shared_ptr<CBaseObject>& obj)
	{
		if (obj == nullptr)
		{
			return;
		}

		workspace->removeChecked(obj->id());

		if (auto object = std::dynamic_pointer_cast<CObject>(obj))
		{
			for (const auto& child : object->children())
			{
				clearCheckedStateOnRemove(workspace, child.second);
			}

			for (const auto& annotation : object->annotations())
			{
				clearCheckedStateOnRemove(workspace, annotation.second);
			}
		}
		else if (auto annotation = std::dynamic_pointer_cast<CAnnotation>(obj))
		{
			for (const auto& child : annotation->annotations())
			{
				clearCheckedStateOnRemove(workspace, child.second);
			}
		}
	}
}


	void CWorkspace::addListener(std::shared_ptr<IWorkspaceEvents> l) {
		if (std::find(listeners_.begin(), listeners_.end(), l) == listeners_.end())
			listeners_.push_back(l);
	}

	void CWorkspace::removeListener(std::shared_ptr<IWorkspaceEvents> l) {
		auto it = std::find(listeners_.begin(), listeners_.end(), l);
		if (it != listeners_.end())
			listeners_.erase(it);
	}

	// Listener notifiers:

	void CWorkspace::notifyObjectActivated(int id) {
		WorkspaceEvent e;
		e.type = WorkspaceEventType::ObjectActivated;
		e.objectId = id;
		for (auto l : listeners_) l->emitWorkspaceEvent(e);
	}

	void CWorkspace::notifyObjectStateChanged(int id) {
		WorkspaceEvent e;
		e.type = WorkspaceEventType::ObjectStateChanged;
		e.objectId = id;
		for (auto l : listeners_) l->emitWorkspaceEvent(e);
	}

	void CWorkspace::notifyObjectAdded(int id) {
		WorkspaceEvent e;
		e.type = WorkspaceEventType::ObjectAdded;
		e.objectId = id;
		for (auto l : listeners_) l->emitWorkspaceEvent(e);
	}

	void CWorkspace::notifyObjectRemoved(int id, CBaseObject::Type tp) {
		WorkspaceEvent e;
		e.type = WorkspaceEventType::ObjectRemoved;
		e.objectId = id;
		e.objectType = tp;
		for (auto l : listeners_) l->emitWorkspaceEvent(e);
	}

	void CWorkspace::notifyStructureChanged() {
		WorkspaceEvent e;
		e.type = WorkspaceEventType::StructureChanged;
		for (auto l : listeners_) l->emitWorkspaceEvent(e);
	}

	// API interfaces for windows and user code:

	void CWorkspace::_objectActivate(int id) {
		int top_i = id;
		if (id != NO_CURRENT_MODEL) {
			if (m_data.find(id) == m_data.end()) //it is not TopModel Id
			{
				top_i = NO_CURRENT_MODEL;
			}
		}
		m_idOfCurrentModel = top_i;
		notifyObjectActivated(id);
	}

	int CWorkspace::_objectAdd(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> parent)
	{
		auto old_p = obj->getParent();
		if (old_p || (m_data.find(obj->id()) != m_data.end())) {
			_objectRemove(obj->id());
		}

		int id = obj->id();
        int result = -1;

		if (parent) {
			if (auto p = std::dynamic_pointer_cast<CObject>(parent)) {
				if (auto c = std::dynamic_pointer_cast<CAnnotation>(obj)) {
					result = CObject::addAnnotation(p, c);
				}
				else {
					result = CObject::addChild(p, obj);
				}
			}
			else if (auto p = std::dynamic_pointer_cast<CAnnotation>(parent)) {
				if (auto c = std::dynamic_pointer_cast<CAnnotation>(obj)) {
					result = p->addAnnotation(p, c);
				}
			}
		}
		else { // add TopModel -- currently we need to check if CModel3D or CImage
			if (auto c = std::dynamic_pointer_cast<CModel3D>(obj)) {
				m_data[id] = c;
			}
			else if (auto c = std::dynamic_pointer_cast<CImage>(obj)) {
				m_data[obj->id()] = c;
			}
			else { 
				auto tmp = std::make_shared<CModel3D>();
				tmp->addChild(tmp, obj);
				id = tmp->id();
                m_data[id] = tmp;
			}
		}
	
		//auto parentId = obj->getParentPtr() ? obj->getParentPtr()->id() : -1;
		//dpInfo() << "CWorkspace::_objectAdd() id=" << obj->id() << " (id = " << id << ") parentId= " << parentId;

		syncCheckedStateOnAdd(this, obj);
		notifyObjectAdded(id);

        return id;
	}

	bool CWorkspace::_objectRemove(std::shared_ptr<CBaseObject> obj)
	{
		//dpInfo() << "_objectRemove(): " << obj.use_count();
		if (obj == nullptr) return false;
		auto id = obj->id();

		auto parent = obj->getParentPtr();
		if (parent) { // it is not Workspace TopModel
			if (auto c = std::dynamic_pointer_cast<CAnnotation>(obj)) {
				if (auto p = std::dynamic_pointer_cast<CObject>(parent)) {
					p->removeAnnotation(c);
				}
				else if (auto p = std::dynamic_pointer_cast<CAnnotation>(parent)) {
					p->removeAnnotation(id);
				}
			}
			else {
				if (auto p = std::dynamic_pointer_cast<CObject>(parent)) {
					p->removeChild(id);
				}
				else {
					return false;
				}
			}
			obj->setParent(nullptr);
		}
		else {
			auto found = m_data.find(id);
			if (found != m_data.end()) {//it is Workspace TopModel Id
				m_data.erase(found);
			}
			else {
				return false;
			}
		}

		clearCheckedStateOnRemove(this, obj);
		if (id == m_idOfCurrentModel) _objectActivate(NO_CURRENT_MODEL);

		notifyObjectRemoved(id, (CBaseObject::Type)obj->type());

		//dpInfo() << "_objectRemove(): " << obj.use_count();
		return true;
	}

	bool CWorkspace::_objectRemove(int id)
	{
		if (auto obj = getSomethingWithId(id))
			return _objectRemove(obj);
		else
			return false;
	}

	bool CWorkspace::removeAll()
	{
		const bool result = _removeAllModels();
		notifyStructureChanged();
		return result;
	}

	bool CWorkspace::removeChecked()
	{
		const std::list<int> selection = checkedIds();
		bool removed = false;
		for (std::list<int>::const_reverse_iterator it = selection.rbegin(); it != selection.rend(); ++it)
		{
			removed = _objectRemove(*it) || removed;
		}
		return removed;
	}

	bool CWorkspace::removeCurrent()
	{
		if (m_idOfCurrentModel == NO_CURRENT_MODEL)
		{
			return false;
		}
		return _objectRemove(m_idOfCurrentModel);
	}

	void CWorkspace::setAllVisible(bool visible)
	{
		for (CWorkspace::iterator it = begin(); it != end(); ++it)
		{
			it->second->setSelfVisibility(visible);
			it->second->setKidsVisibility(visible);
		}
		notifyStructureChanged();
	}

	void CWorkspace::setCheckedVisible(bool visible)
	{
		const std::list<int> selection = checkedIds();
		for (int id : selection)
		{
			if (std::shared_ptr<CModel3D> model = _getModel(id))
			{
				model->setSelfVisibility(visible);
			}
		}
		notifyStructureChanged();
	}

	void CWorkspace::checkAll()
	{
		clearChecked();
		for (CWorkspace::iterator it = begin(); it != end(); ++it)
		{
			addChecked(it->first);
		}
		notifyStructureChanged();
	}

	std::shared_ptr<CModel3D> CWorkspace::duplicateModel(int id)
	{
		std::shared_ptr<CModel3D> original = _getModel(id);
		if (original == nullptr)
		{
			return nullptr;
		}

		std::shared_ptr<CModel3D> copy = std::dynamic_pointer_cast<CModel3D>(original->getCopy());
		if (copy != nullptr && _objectAdd(copy) != -1)
		{
			return copy;
		}

		return nullptr;
	}

	std::shared_ptr<CModel3D> CWorkspace::duplicateCurrentModel()
	{
		return duplicateModel(_getCurrentModelId());
	}

