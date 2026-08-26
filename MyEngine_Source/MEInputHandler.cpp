#include "MEInputHandler.h"
#include "MECommand.h"
#include "MEGameObject.h"
#include "../MyEngine_W/MEActorScript.h"
#include "MEInput.h"

namespace ME
{
	InputHandler::InputHandler()
	{
		mPrimaryActionCommand = std::make_unique<ActionCommand>([](GameObject* object)
			{

				ActorScript* actorScript = object->GetComponent<ActorScript>();
				if (actorScript)
				{
					actorScript->OnPrimaryAction();
				}
			}
		);

		mToggleWeaponCommand = std::make_unique<ActionCommand>([](GameObject* object)
			{
				ActorScript* actorScript = object->GetComponent<ActorScript>();
				if (actorScript)
				{
					actorScript->OnToggleWeapon();
				}
			}
		);
	}

	ICommand* InputHandler::HandleActionInput()
	{
		if (Input::GetKeyDown(ME::eKeyCode::LeftMous))
		{
			return mPrimaryActionCommand.get();
		}

		if (Input::GetKeyDown(ME::eKeyCode::T))
		{
			return mToggleWeaponCommand.get();
		}


		return nullptr;
	}

	math::Vector2 InputHandler::GetMovementAxis()
	{
		Vector2 inputDir = Vector2::Zero;

		if (Input::GetKey(ME::eKeyCode::Up) || Input::GetKey(eKeyCode::W))    inputDir.y += 1.0f;
		if (Input::GetKey(ME::eKeyCode::Down) || Input::GetKey(eKeyCode::S))  inputDir.y -= 1.0f;
		if (Input::GetKey(ME::eKeyCode::Right) || Input::GetKey(eKeyCode::D)) inputDir.x += 1.0f;
		if (Input::GetKey(ME::eKeyCode::Left) || Input::GetKey(eKeyCode::A))  inputDir.x -= 1.0f;

		if (inputDir.LengthSquared() > 0.0f)
		{
			inputDir.Normalize();
		}

		return inputDir;
	}

}