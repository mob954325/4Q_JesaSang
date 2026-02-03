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
        readyQueue.pop();
    }
}

void ScriptSystem::Register(Component* comp)
{
    readyQueue.push(comp);
    comps.push_back(comp);
    comp->OnInitialize();
}

void ScriptSystem::RegisterScript(Component* comp)
{
    scriptComps.push_back(comp);
    scriptCompsInitReady.push_back(comp);
}

void ScriptSystem::UnRegister(Component* comp)
{
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
    for (auto& e : comps)
    {
        if (!e->GetOwner()->GetActiveSelf()) continue;  // Object 활성화 여부
        if (!e->GetActiveSelf()) continue;              // 컴포넌트 활성화 여부

        if (!e->IsStart())
        {
            e->IsStart();
            e->SetStartTrue();
        }
        else
        {
            e->OnUpdate(delta);
        }
    }

    if (PlayModeSystem::Instance().IsPlaying())
    {
        // 1. 등록된 init 해소

        for (auto it = scriptCompsInitReady.begin(); it != scriptCompsInitReady.end();)
        {
            if (!(*it)->GetOwner()->GetActiveSelf() || !(*it)->GetActiveSelf())
            {
                it++;
            }
            else
            {
                (*it)->OnInitialize();
                it = scriptCompsInitReady.erase(it);
            }
        }

        // 3. 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            // NOTE : 호출 순서를 위해서 Update에서 한꺼번에 처리
            if (!e->GetOwner()->GetActiveSelf() || !e->GetActiveSelf()) continue; // 1. 첫 OnEnable이 활성화 체크

            if(!e->IsStart()) e->OnEnable(); // isStart 체크 이유 : 만약 중간에 disable 되지않고 실행되면, OnEnable -> OnStart 순으로 실행됨이 보장되기때문에.

            if (!e->GetOwner()->GetActiveSelf() || !e->GetActiveSelf()) continue; // 2. OnEnable에서 Disable를 호출했으면 Start를 호출하지 않고 큐를 isStart를 체크하지 않는다.
                
            if (!e->IsStart())
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
        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            if (!e->GetOwner()->GetActiveSelf() || !e->GetActiveSelf()) continue;

            e->OnFixedUpdate(dt);
        }
    }
}

void ScriptSystem::LateUpdate(float dt)
{
    if (PlayModeSystem::Instance().IsPlaying())
    {
        // 사용자 정의 component update
        for (auto& e : scriptComps)
        {
            if (!e->GetOwner()->GetActiveSelf() || !e->GetActiveSelf()) continue;

            e->OnLateUpdate(dt);
        }
    }
}

void ScriptSystem::Clear()
{
    comps.clear();
    while (!readyQueue.empty()) readyQueue.pop();

   scriptComps.clear();
   scriptCompsInitReady.clear();
}
