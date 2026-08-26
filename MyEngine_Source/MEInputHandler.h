#pragma once
#include "MECommand.h"
#include "MEMath.h"

namespace ME
{
	class ICommand;
	
	class InputHandler
	{
	
	public:
		InputHandler();

		ICommand* HandleActionInput();
		math::Vector2 GetMovementAxis();



	private:
		std::unique_ptr<ICommand> mPrimaryActionCommand;
		std::unique_ptr<ICommand> mToggleWeaponCommand;
	};

}
