#include "MERigidbody.h"
#include "METime.h"
#include "METransform.h"
#include "MEGameObject.h"
namespace ME
{
    Rigidbody::Rigidbody()
        :Component(enums::eComponentType::Rigidbody)
        , mbGround(false)
        , mMass(1.0f)
        , mAirDrag(0.02f)
		, mFriction(70.0f)
	, mForce(Vector3::Zero)
	, mVelocity(Vector3::Zero)
	, mLimitVelocity(Vector3(130.0f,1000.0f, 130.0f))
	, mAccelation(Vector3::Zero)
		, mbIsAffectedByGravity(true)
        , gravity(-9.8f)
        ,curForceMode(eForceMode::Force)
	{
        mGravity = Vector3(0.0f, gravity * mMass, 0.0f);
	}
	Rigidbody::~Rigidbody()
	{
	}
	void Rigidbody::Initialize()
	{
	}
	void Rigidbody::Update()
	{
        const float dt = Time::DeltaTime();

        // 즉시효과 (dt 없음)
        mVelocity += sumImpulse / mMass;
        mVelocity += sumVelChange;

        // 가속도
        Vector3 a = sumForce / mMass + sumAccel;
        if (mbIsAffectedByGravity) a += mGravity;

        // 속도 적분
        mVelocity += a * dt;

        // 감쇠
        if (mbGround) {
            // 수직 제거 + 지면 마찰 (필요 객체만)
            const Vector3 n = Vector3::Up;
            const float vN = mVelocity.Dot(n);
            mVelocity -= n * vN;

            const float speed = mVelocity.Length();
            if (speed > 1e-4f) {
                const float decel = mFriction * dt;           // 1/s
                const float newSpeed = max(0.0f, speed - decel);
                mVelocity *= (newSpeed / speed);
            }
        }
        else {
            const float factor = max(0.0f, 1.0f - mAirDrag * dt);
            mVelocity *= factor;
        }

        // 속도 제한 (총알은 크게)
        {
            const Vector3 n = Vector3::Up;
            float vN = mVelocity.Dot(n);
            Vector3 vVert = n * vN;
            Vector3 vHorz = mVelocity - vVert;

            float hs = vHorz.Length();
            if (hs > mLimitVelocity.x && hs > 1e-6f) vHorz *= (mLimitVelocity.x / hs);
            vN = std::clamp(vN, -mLimitVelocity.y, mLimitVelocity.y);
            mVelocity = vHorz + n * vN;
        }

        // 위치
        auto* tr = GetOwner()->GetComponent<Transform>();
        tr->SetPosition(tr->GetPosition() + mVelocity * dt);

        // 누적 초기화
        sumForce = sumAccel = sumImpulse = sumVelChange = Vector3::Zero;

	}
	void Rigidbody::LateUpdate()
	{
	}
	void Rigidbody::Render()
	{
	}

    void Rigidbody::ClearForces()
    {
        mForce = Vector3::Zero;
        mAccelation = Vector3::Zero;
        curForceMode = eForceMode::Force;
        mVelocity = Vector3::Zero;
   }


}