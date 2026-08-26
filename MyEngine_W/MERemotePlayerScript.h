#pragma once

#include "MEActorScript.h"
#include "MEMath.h"
#include "MEBone.h"
#include "../MyEngine_Source/Protocol.h"

namespace ME
{
    class Transform;
    class Animator3D;
    class WeaponScript;
    class Collider;

    class RemotePlayerScript : public ActorScript
    {
    public:
        RemotePlayerScript();
        ~RemotePlayerScript();

        void Initialize() override;
        void Update() override;
        void LateUpdate() override;
        void Render() override;

        void OnCollisionEnter(Collider* other) override;
        void OnCollisionStay(Collider* other) override;
        void OnCollisionExit(Collider* other) override;

        void OnPrimaryAction() override;
        void OnToggleWeapon() override;

        void OnDeath() override;
        void DamageProcess(DamageInfo damageInfo) override;

        Bone* GetWeaponSocketBone() override;
        Bone* GetWeaponSocketBoneRight() override;
        Vector3 GetAimDirection() override;

        void RegisterWeapon(
            eWeaponType type,
            WeaponScript* weapon);

        void ApplyMove(
            float x,
            float y,
            float z,
            float yaw);

        void ApplyState(
            ePlayerState state,
            bool forced = false);

        void ApplyAttack(
            eWeaponType weaponType,
            std::uint8_t attackIndex,
            const Vector3& direction);

        void ApplyWeaponChange(
            eWeaponType weaponType);

        void ApplyServerDamage(
            float remainingHp,
            bool isDead,
            const Vector3& hitPosition);


    private:
        void CacheComponents();
        void CacheBones();

    private:
        Transform* mTransform = nullptr;

        ePlayerState mCurrentRemoteState = ePlayerState::IDLE;
        Bone* mLeftHandBone;
        Bone* mRightHandBone;

        eWeaponType mCurrentWeaponType = eWeaponType::None;
        WeaponScript* mGun = nullptr;
        WeaponScript* mSword = nullptr;

        WeaponScript* mCurrentWeapon = nullptr;

        std::unordered_map<eWeaponType, WeaponScript*> mWeaponMap;

        bool mbPlayingAction = false;
    };
}