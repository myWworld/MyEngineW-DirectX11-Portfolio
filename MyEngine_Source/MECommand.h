#pragma once
#include "CommonInclude.h"


namespace ME
{
	class GameObject;
	class ICommand
	{
	public:
		virtual ~ICommand() = default;
		virtual void Execute(GameObject* actor) = 0;

	};


	class ActionCommand :public ICommand
	{
	public :
		std::function<void(GameObject*)> mAction;

	public:
		ActionCommand(std::function<void(GameObject*)> action) : mAction(action) {}

		void Execute(GameObject* actor) override
		{
			if(mAction)
				mAction(actor);
		}
	};
}