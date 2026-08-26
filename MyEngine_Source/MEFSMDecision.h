#pragma once

#include "IFSMContext.h"
#include "json.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>

namespace ME
{
    class FSMBrainCore;

    enum class eDecisionResult
    {
        True,
        False,
        Pending
    };

    class FSMDecision
    {
    public:
        FSMDecision()
            : mEvaluationTimer(0.0f)
            , mCheckInterval(0.2f)
        {
        }

        virtual ~FSMDecision() = default;

        void LoadFromJson(
            const nlohmann::json& json)
        {
            for (auto& [key, setter] : mProperties)
            {
                if (json.contains(key))
                {
                    setter(json[key]);
                }
            }
        }

        eDecisionResult Decision(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
            mEvaluationTimer += (std::max)(0.0f, context.GetDeltaTime());

            if (mCheckInterval > 0.0f &&
                mEvaluationTimer < mCheckInterval)
            {
                return eDecisionResult::Pending;
            }

            mEvaluationTimer = 0.0f;

            return CheckDecision(brain, context)
                ? eDecisionResult::True
                : eDecisionResult::False;
        }

        void ResetRuntime()
        {
            mEvaluationTimer = 0.0f;
        }

        void SetIntervalTime(float interval)
        {
            mCheckInterval = interval;
        }

    protected:
        virtual bool CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context) = 0;

        template <typename T>
        void BindProperty(
            const std::string& name,
            T* member)
        {
            mProperties[name] =
                [member](const nlohmann::json& value)
                {
                    *member = value.get<T>();
                };
        }

    private:
        std::unordered_map<
            std::string,
            std::function<void(const nlohmann::json&)>>
            mProperties;

        float mEvaluationTimer;
        float mCheckInterval;
    };
}