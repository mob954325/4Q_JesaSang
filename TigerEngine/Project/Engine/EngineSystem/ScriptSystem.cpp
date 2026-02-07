#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"
#include "../EngineSystem/PlayModeSystem.h"
#include "../Object/GameObject.h"

void ScriptSystem::CheckReadyQueue()
{
    while (!readyQueue.empty())
    {
        auto comp = readyQueue.front();

        comp->OnStart();
        comp->SetStartTrue();

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
            pending_components.erase(it);
            return;
        }
    }

    for (auto it = comps.begin(); it != comps.end(); it++)
    {
        if (*it == comp)
        {
            comps.erase(it);
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
            pending_scriptComponents.erase(it);
            return;
        }
    }

    for (auto it = scriptComps.begin(); it != scriptComps.end(); it++)
    {
        if (*it == comp)
        {
            scriptComps.erase(it);
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
                e->OnStart();
                e->SetStartTrue();
            }
            else
            {
                e->OnUpdate(delta);
            }
        }
    }
}

void ScriptSystem::FixedUpdate(float dt)
{
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }

        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            pending_scriptComponents.clear();

            e->OnFixedUpdate(dt);
        }
    }
}

void ScriptSystem::LateUpdate(float dt)
{
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }

        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            pending_scriptComponents.clear();

            e->OnLateUpdate(dt);
        }
    }
}

void ScriptSystem::Clear()
{
    comps.clear();
    while (!readyQueue.empty()) readyQueue.pop();

   scriptComps.clear();
   pending_scriptComponents.clear();
}
