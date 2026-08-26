#pragma once
#include "MEMath.h"

namespace ME
{
    class GameObject;
	struct DamageInfo
	{
        float damage;           // 입힐 데미지
        GameObject* attacker;   // 공격자 
        math::Vector3 hitPoint;       // 피격 지점 
        math::Vector3 knockbackDir;   // 넉백 방향 
        float knockbackForce;   // 넉백 세기

        // 필요에 따라 추가
        // bool isCritical;    // 치명타 여부
        // int elementType;    // 속성 (불, 얼음 등)
	};

}

