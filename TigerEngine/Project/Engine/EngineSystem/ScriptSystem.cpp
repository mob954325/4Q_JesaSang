#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"
#include "../EngineSystem/PlayModeSystem.h"
#include "../Object/GameObject.h"

void ScriptSystem::CheckReadyQueue()
{
    while (!readyQueue.empty())
    {
        auto comp = readyQueue.front();

        comp->SetStartTrue();
        comp->OnStart();

        readyQueue.pop();
    }
}

void ScriptSystem::Register(Component* comp)
{
    if(!comp->IsStart())
        readyQueue.push(comp);

    pending_components.push_back(comp);
}

void ScriptSystem::RegisterScript(Component* comp)
{
    pending_scriptComponents.push_back(comp);
}

void ScriptSystem::UnRegister(Component* comp)
{
    for (auto it = pending_components.begin(); it != pending_components.end(); it++)
    {
        if (*it == comp)
        {
            pending_scriptRemovals.push_back(*it);
            return;
        }
    }

    for (auto it = comps.begin(); it != comps.end(); it++)
    {
        if (*it == comp)
        {
            pending_scriptRemovals.push_back(*it);
            return;
        }
    }
}

void ScriptSystem::UnRegisterScript(Component* comp)
{
    for (auto it = pending_scriptComponents.begin(); it != pending_scriptComponents.end(); it++)
    {
        if (*it == comp)
        {
            pending_scriptRemovals.push_back(*it);
            return;
        }
    }

    for (auto it = scriptComps.begin(); it != scriptComps.end(); it++)
    {
        if (*it == comp)
        {
            pending_scriptRemovals.push_back(*it);
            return;
        }
    }
}

void ScriptSystem::Update(float delta)
{
    // 일반 component update
    for (auto& e : pending_components)
    {
        comps.push_back(e);
    }
    pending_components.clear();

    for (auto& e : comps)
    {
        if(e->IsStart())
        {
            e->OnUpdate(delta);
        }
    }


    // 스크립트 컴포넌트 업데이트
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }
        pending_scriptComponents.clear();

        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            // 이전 이벤트 함수 Oninitialize(), OnEnable은 AddComponent 시 호출됩니다.
            if (!e->IsStart()) // start 해소
            {
                e->SetStartTrue();
                e->OnStart();
            }
            else
            {
                e->OnUpdate(delta);
            }
        }
    }

    ProcessRemovals(); // 제거 대상 확인
}

void ScriptSystem::FixedUpdate(float dt)
{
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }
        pending_scriptComponents.clear();

        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            e->OnFixedUpdate(dt);
        }
    }

    ProcessRemovals(); // 제거 대상 확인
}

void ScriptSystem::LateUpdate(float dt)
{
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }
        pending_scriptComponents.clear();

        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            e->OnLateUpdate(dt);
        }
    }

    ProcessRemovals(); // 제거 대상 확인
}

void ScriptSystem::Clear()
{
    comps.clear();
    while (!readyQueue.empty()) readyQueue.pop();

   scriptComps.clear();
   pending_scriptComponents.clear();
}

void ScriptSystem::SwapErase(std::vector<Component*>& comps, Component* target)
{
    for (int i = 0; i < comps.size(); ++i)
    {
        if (comps[i] == target)
        {
            comps[i] = comps.back();
            comps.pop_back();
            return;
        }
    }
}

void ScriptSystem::ProcessRemovals()
{
    if (pending_scriptRemovals.empty()) return;

    for (auto* c : pending_scriptRemovals)
    {
        SwapErase(pending_components, c);
        SwapErase(comps, c);

        SwapErase(pending_scriptComponents, c);
        SwapErase(scriptComps, c);
    }
    pending_scriptRemovals.clear();
}