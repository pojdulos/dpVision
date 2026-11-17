#pragma once

#include <algorithm>
#include <vector>

class OrderedIdList
{
public:
	const std::vector<int>& ids() const { return ids_; }

	bool contains(int id) const
	{
		return std::find(ids_.begin(), ids_.end(), id) != ids_.end();
	}

	void append(int id)
	{
		if (!contains(id))
		{
			ids_.push_back(id);
		}
	}

	void remove(int id)
	{
		ids_.erase(std::remove(ids_.begin(), ids_.end(), id), ids_.end());
	}

	void clear()
	{
		ids_.clear();
	}

	bool moveBefore(int movedId, int anchorId)
	{
		if (movedId == anchorId)
		{
			return false;
		}

		std::vector<int>::iterator movedIt = std::find(ids_.begin(), ids_.end(), movedId);
		std::vector<int>::iterator anchorIt = std::find(ids_.begin(), ids_.end(), anchorId);
		if ((movedIt == ids_.end()) || (anchorIt == ids_.end()))
		{
			return false;
		}

		const int movedValue = *movedIt;
		ids_.erase(movedIt);
		anchorIt = std::find(ids_.begin(), ids_.end(), anchorId);
		ids_.insert(anchorIt, movedValue);
		return true;
	}

	bool moveAfter(int movedId, int anchorId)
	{
		if (movedId == anchorId)
		{
			return false;
		}

		std::vector<int>::iterator movedIt = std::find(ids_.begin(), ids_.end(), movedId);
		std::vector<int>::iterator anchorIt = std::find(ids_.begin(), ids_.end(), anchorId);
		if ((movedIt == ids_.end()) || (anchorIt == ids_.end()))
		{
			return false;
		}

		const int movedValue = *movedIt;
		ids_.erase(movedIt);
		anchorIt = std::find(ids_.begin(), ids_.end(), anchorId);
		if (anchorIt != ids_.end())
		{
			++anchorIt;
		}
		ids_.insert(anchorIt, movedValue);
		return true;
	}

private:
	std::vector<int> ids_;
};
