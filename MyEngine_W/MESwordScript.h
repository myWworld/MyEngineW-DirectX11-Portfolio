#pragma once
#include "MEWeaponScript.h"
#include <vector>

namespace ME
{

class SwordScript : public WeaponScript
{
public:

	SwordScript();
	~SwordScript();

	void Initialize() override;
	void Update()override;
	void LateUpdate()override;
	void Render()override;	

	bool Use(WeaponAttackInfo& outAttackInfo) override;

	void OnRegister(ActorScript* ownerActor) override;

	void OpenComboWindow()
	{
		mbCanComboInput = true;
	}

	bool CanComboInput() {return mbCanComboInput;}

private:
	
	void LockInput();
	void ResetAttackFlags();

private :

	std::vector<std::wstring> mComboAnimNames; // 콤보 애니메이션 이름 목록

	int mComboCount = 0;
	float mComboTimer = 0.0f;
	float mComboMaxTime = 2.0f; // 콤보 입력 허용 시간
	bool mbIsComboActive = false; // 콤보 입력 활성화 여부
	bool mbCanComboInput = true;


};

}

