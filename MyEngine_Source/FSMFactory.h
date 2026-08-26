#pragma once

#include "FSMBrainCore.h"
#include "MEFSMDecision.h"
#include "MEFSMState.h"
#include "MEFSMTask.h"
#include "json.hpp"

#include "../MyEngine_W/MEAnimFinishDecision.h"
#include "../MyEngine_W/MEDestroyTask.h"
#include "../MyEngine_W/MEDetectTargetDecision.h"
#include "../MyEngine_W/MEDistanceDecision.h"
#include "../MyEngine_W/MEMeleeAttackTask.h"
#include "../MyEngine_W/MEMoveToTargetTask.h"
#include "../MyEngine_W/MEPlayAnimTask.h"
#include "../MyEngine_W/MERandomWanderTask.h"
#include "../MyEngine_W/METimerDecision.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace ME
{
    class FSMFactory
    {
    private:
        using CreateTask = std::function<std::unique_ptr<FSMTask>()>;
        using CreateDecision = std::function<std::unique_ptr<FSMDecision>()>;

    public:
        static void Initialize()
        {
            if (mbInitialized)
                return;

            mbInitialized = true;

            RegisterTask<PlayAnimTask>("PlayAnimTask");
            RegisterTask<RandomWanderTask>("RandomWanderTask");
            RegisterTask<MoveToTargetTask>("MoveToTargetTask");
            RegisterTask<MeleeAttackTask>("MeleeAttackTask");
            RegisterTask<DestroyTask>("DestroyTask");

            RegisterDecision<TimerDecision>("TimerDecision");
            RegisterDecision<AnimFinishDecision>("AnimFinishDecision");
            RegisterDecision<DetectTargetDecision>("DetectTargetDecision");
            RegisterDecision<DistanceDecision>("DistanceDecision");
        }

        static bool MakeFSMWithJsonFile(FSMBrainCore* brain, const std::string& jsonPath)
        {
            if (brain == nullptr)
                return false;

            Initialize();

            std::ifstream file(jsonPath);

            if (!file.is_open())
            {
                std::cerr << "[FSMFactory] JSON 파일 열기 실패: " << jsonPath << '\n';
                return false;
            }

            try
            {
                const nlohmann::json data = nlohmann::json::parse(file);

                brain->Clear();

                // 1차: 상태 생성
                for (const auto& stateJson : data.at("States"))
                {
                    const std::string stateName = stateJson.at("Name").get<std::string>();
                    auto state = std::make_unique<FSMState>();

                    state->SetStateName(stateName);
                    brain->AddState(stateName, std::move(state));
                }

                // 2차: Task와 Transition 연결
                for (const auto& stateJson : data.at("States"))
                {
                    const std::string stateName = stateJson.at("Name").get<std::string>();
                    FSMState* state = brain->FindState(stateName);

                    if (state == nullptr)
                        continue;

                    if (stateJson.contains("Tasks"))
                    {
                        for (const auto& taskJson : stateJson.at("Tasks"))
                        {
                            const std::string type = taskJson.at("Type").get<std::string>();
                            auto task = GetTask(type);

                            if (!task)
                            {
                                std::cerr << "[FSMFactory] 등록되지 않은 Task: " << type << '\n';
                                continue;
                            }

                            task->LoadFromJson(taskJson);
                            state->AddTask(std::move(task));
                        }
                    }

                    if (stateJson.contains("Transitions"))
                    {
                        for (const auto& transitionJson : stateJson.at("Transitions"))
                        {
                            const auto& decisionJson = transitionJson.at("Decision");
                            const std::string decisionType = decisionJson.at("Type").get<std::string>();
                            auto decision = GetDecision(decisionType);

                            if (!decision)
                            {
                                std::cerr << "[FSMFactory] 등록되지 않은 Decision: " << decisionType << '\n';
                                continue;
                            }

                            decision->LoadFromJson(decisionJson);

                            FSMState* trueState = nullptr;
                            FSMState* falseState = nullptr;

                            if (transitionJson.contains("TrueState") &&
                                !transitionJson["TrueState"].is_null())
                            {
                                trueState = brain->FindState(
                                    transitionJson["TrueState"].get<std::string>()
                                );
                            }

                            if (transitionJson.contains("FalseState") &&
                                !transitionJson["FalseState"].is_null())
                            {
                                falseState = brain->FindState(
                                    transitionJson["FalseState"].get<std::string>()
                                );
                            }

                            state->AddTransition(std::move(decision), trueState, falseState);
                        }
                    }
                }

                if (!data.contains("InitialState"))
                    return false;

                return brain->SetInitialState(data.at("InitialState").get<std::string>());
            }
            catch (const std::exception& exception)
            {
                std::cerr << "[FSMFactory] JSON 처리 실패: " << exception.what() << '\n';
                return false;
            }
        }

    private:
        template <typename T>
        static void RegisterTask(const std::string& name)
        {
            mTaskCreators[name] = []()
                {
                    return std::make_unique<T>();
                };
        }

        template <typename T>
        static void RegisterDecision(const std::string& name)
        {
            mDecisionCreators[name] = []()
                {
                    return std::make_unique<T>();
                };
        }

        static std::unique_ptr<FSMTask> GetTask(const std::string& name)
        {
            auto iter = mTaskCreators.find(name);

            if (iter == mTaskCreators.end())
                return nullptr;

            return iter->second();
        }

        static std::unique_ptr<FSMDecision> GetDecision(const std::string& name)
        {
            auto iter = mDecisionCreators.find(name);

            if (iter == mDecisionCreators.end())
                return nullptr;

            return iter->second();
        }

    private:
        inline static bool mbInitialized = false;
        inline static std::unordered_map<std::string, CreateTask> mTaskCreators;
        inline static std::unordered_map<std::string, CreateDecision> mDecisionCreators;
    };
}