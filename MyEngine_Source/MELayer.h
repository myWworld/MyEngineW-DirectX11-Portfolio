#pragma once

#include "MEEntity.h"
#include "CommonInclude.h"
#include "MEGameObject.h"


namespace ME
{
	class Layer:public Entity
	{
	public:
		Layer();
		~Layer();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();
		virtual void Destroy();

		void AddGameObject(std::unique_ptr<GameObject> gameObject);
		void EraseGameObject(GameObject* obj);
		std::unique_ptr<GameObject> ExtractGameObject(GameObject* gameObj);

		const std::vector<GameObject*> GetGameObject() 
		{
			std::vector<GameObject*> mGameObjPtrs;

			for (auto& gameOBj : mGameObjects)
				{
					if (gameOBj == nullptr)
						continue;

					mGameObjPtrs.push_back(gameOBj.get());
				}

			return mGameObjPtrs;
		}

	private:

		void findDeadGameObjects(OUT std::vector<GameObject*>& gameObjs);
		void deleteGameObjects(std::vector<GameObject*> deleteObjs);
		void eraseDeadGameObject();

	
	private:

		std::vector<std::unique_ptr<GameObject>> mGameObjects;
		

	};
	
		typedef std::vector<GameObject*>::iterator GameObjectIter;

}

