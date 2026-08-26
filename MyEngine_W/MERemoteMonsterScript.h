#pragma once

#include "MEActorScript.h"
#include "MEBone.h"
#include "MEMath.h"

#include "../MyEngine_Source/Protocol.h"

namespace ME
{
    class Animator3D;
    class Collider;
    class Transform;
    class WeaponScript;

    class RemoteMonsterScript
        : public ActorScript
    {
    public:
        RemoteMonsterScript();
        ~RemoteMonsterScript();

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

        void SetLeftWeapon(
            WeaponScript* weapon);

        void SetRightWeapon(
            WeaponScript* weapon);

        void ApplyMove(
            float x,
            float y,
            float z,
            float yaw);

        void ApplyState(
            eMonsterState state,
            bool forced = false);

        void ApplyAttack(
            std::uint8_t attackIndex,
            const Vector3& direction);

        void ApplyServerDamage(
            float remainingHp,
            bool isDead,
            const Vector3& hitPosition);

    private:
        void CacheComponents();
        void CacheBones();

        void PlayPersistentState();

    private:
        Transform* mTransform = nullptr;

        Bone* mLeftHandBone = nullptr;
        Bone* mRightHandBone = nullptr;

        WeaponScript* mLeftWeapon = nullptr;
        WeaponScript* mRightWeapon = nullptr;

        eMonsterState mCurrentRemoteState =
            eMonsterState::IDLE;

        eMonsterState mLastPlayedState =
            eMonsterState::IDLE;

        Vector3 mAttackDirection =
            Vector3::Forward;

        bool mbPlayingAction = false;
        bool mbHasPlayedState = false;
    };
}