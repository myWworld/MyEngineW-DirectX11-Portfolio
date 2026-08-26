#pragma once

#include "json.hpp"

#include <functional>
#include <string>
#include <unordered_map>

#define BIND_PROP(var) BindProperty(#var, &var)

namespace ME
{
    class FSMBrainCore;
    class IFSMContext;

    class FSMTask
    {
    public:
        FSMTask()
            : bIsFinished(false)
            , bIsLoop(false)
        {
        }

        virtual ~FSMTask() = default;

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

        void Enter(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
            bIsFinished = false;
            OnEnter(brain, context);
        }

        void Execute(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
            if (bIsFinished)
                return;

            OnExecute(brain, context);

            if (!bIsLoop)
            {
                bIsFinished = true;
            }
        }

        void Exit(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
            OnExit(brain, context);
        }

    protected:
        virtual void OnEnter(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
        }

        virtual void OnExecute(
            FSMBrainCore* brain,
            IFSMContext& context) = 0;

        virtual void OnExit(
            FSMBrainCore* brain,
            IFSMContext& context)
        {
        }

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

    public:
        bool bIsFinished;
        bool bIsLoop;

    private:
        std::unordered_map<
            std::string,
            std::function<void(const nlohmann::json&)>>
            mProperties;
    };
}