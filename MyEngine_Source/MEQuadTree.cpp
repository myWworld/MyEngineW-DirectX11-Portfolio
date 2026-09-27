#include "MEQuadTree.h"
#include "MEGameObject.h"
#include "METransform.h"


namespace ME
{
	QuadTree::QuadTree(int level, math::Vector2 boundsMin, math::Vector2 boundsMax) : mLevel(level), mBoundsMin(boundsMin), mBoundsMax(boundsMax)
	{
		for (int i = 0; i < 4; i++)
		{
			mNodes[i] = nullptr;
		}
	}

	QuadTree::~QuadTree()
	{
		Clear();
	}

	void QuadTree::Clear()
	{
		mObjects.clear();
		for (int i = 0; i < 4; i++)
		{
			if (mNodes[i] != nullptr)
			{
				mNodes[i]->Clear();
				delete mNodes[i];
				mNodes[i] = nullptr;
			}
		}
	}

	void QuadTree::Split()
	{
		float subWidth = (mBoundsMax.x - mBoundsMin.x) / 2.0f;
		float subHeight = (mBoundsMax.y - mBoundsMin.y) / 2.0f;	

		float x = mBoundsMin.x;
		float y = mBoundsMin.y;

		mNodes[0] = new QuadTree(mLevel + 1, math::Vector2(x + subWidth, y + subHeight), math::Vector2(x + subWidth * 2, y + subHeight * 2));
		mNodes[1] = new QuadTree(mLevel + 1, math::Vector2(x , y + subHeight), math::Vector2(x + subWidth, y + 2 * subHeight));
		mNodes[2] = new QuadTree(mLevel + 1, math::Vector2(x, y), math::Vector2(x + subWidth, y + subHeight));
		mNodes[3] = new QuadTree(mLevel + 1, math::Vector2(x + subWidth, y), math::Vector2(x + 2 * subWidth, y + subHeight));
	}


	int QuadTree::GetIndex(Collider* col)
	{
		int index = -1;

		Vector3 pos = col->GetOwner()->GetComponent<Transform>()->GetPosition();
		Vector3 size = col->GetSize();

		float verticalMidPoint = mBoundsMin.x + (mBoundsMax.x - mBoundsMin.x) / 2.0f;
		float horizontalMidPoint = mBoundsMin.y + (mBoundsMax.y - mBoundsMin.y) / 2.0f;

		bool topQuadrant = (pos.z - size.z / 2.0f > horizontalMidPoint);
		bool bottomQuadrant = (pos.z + size.z / 2.0f < horizontalMidPoint);

		if (pos.x + size.x / 2 < verticalMidPoint)
		{
			if (topQuadrant)
				index = 1;
			else if (bottomQuadrant)
				index = 2;
		}
		else if (pos.x - size.x / 2 > verticalMidPoint)
		{
			if (topQuadrant)
				index = 0;
			else if (bottomQuadrant)
				index = 3;
		}

		return index;
	}

	void QuadTree::Insert(Collider* col)
	{
		if (mNodes[0] != nullptr)
		{
			int index = GetIndex(col);

			if (index != -1)
			{
				mNodes[index]->Insert(col);
				return;
			}
		}

		mObjects.push_back(col);


		if (mObjects.size() > MAX_OBJECTS && mLevel < MAX_LEVELS)
		{
			if (mNodes[0] == nullptr)
			{
				Split();
			}

			int i = 0;
			while (i < mObjects.size())
			{
				int index = GetIndex(mObjects[i]);
				if (index != -1)
				{
					mNodes[index]->Insert(mObjects[i]);
					mObjects.erase(mObjects.begin() + i);
				}
				else
				{
					i++;
				}
			}

		}	
	}


	std::vector<Collider*> QuadTree::Retrieve(std::vector<Collider*>& returnObjects, Collider* col)
	{
		int index = GetIndex(col);

		if (index != -1 && mNodes[0] != nullptr)
		{
			mNodes[index]->Retrieve(returnObjects, col);
		}
		else if (index == -1 && mNodes[0] != nullptr)
		{
			for (int i = 0; i < 4; ++i)
			{
				mNodes[i]->Retrieve(returnObjects, col);
			}
		}

		for (int i = 0; i < mObjects.size(); i++)
		{
			returnObjects.push_back(mObjects[i]);
		}
		return returnObjects;
	}

}