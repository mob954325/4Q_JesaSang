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
    if (!isIterating)
    {
        RemoveImmediate(comp);
        return;
    }

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
    if (!isIterating)
    {
        RemoveImmediate(comp);
        return;
    }

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
    // ?ºÎ∞ò component update
    for (auto& e : pending_components)
    {
        comps.push_back(e);
    }
    pending_components.clear();

    isIterating = true;

    for (auto& e : comps)
    {
        if(e->IsStart())
        {
            e->OnUpdate(delta);
        }
    }


    // ?§ÌÅ¨Î¶ΩÌä∏ Ïª¥Ìè¨?åÌä∏ ?ÖÎç∞?¥Ìä∏
    if (PlayModeSystem::Instance().IsPlaying())
    {
        for (auto& e : pending_scriptComponents)
        {
            scriptComps.push_back(e);
        }
        pending_scriptComponents.clear();

        // ?¨Ïö©???ïÏùò component update
        for (auto& e : scriptComps)
        {
            // ?¥Ï†Ñ ?¥Î≤§???®Ïàò Oninitialize(), OnEnable?Ä AddComponent ???∏Ï∂ú?©Îãà??
            if (!e->IsStart()) // start ?¥ÏÜå
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

    isIterating = false;
    ProcessRemovals(); // ?úÍ±∞ ?Ä???ïÏù∏
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

        // ?¨Ïö©???ïÏùò component update
        isIterating = true;
        for (auto& e : scriptComps)
        {
            e->OnFixedUpdate(dt);
        }
    }

    isIterating = false;
    ProcessRemovals(); // ?úÍ±∞ ?Ä???ïÏù∏
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

        // ?¨Ïö©???ïÏùò component update
        isIterating = true;
        for (auto& e : scriptComps)
        {
            e->OnLateUpdate(dt);
        }
    }

    isIterating = false;
    ProcessRemovals(); // ?úÍ±∞ ?Ä???ïÏù∏
}

void ScriptSystem::Clear()
{
    comps.clear();
    while (!readyQueue.empty()) readyQueue.pop();

   scriptComps.clear();
   pending_scriptComponents.clear();
   pending_components.clear();
   pending_scriptRemovals.clear();
}

void ScriptSystem::RemoveFromReadyQueue(Component* comp)
{
    std::queue<Component*> q;
    while (!readyQueue.empty())
    {
        auto* c = readyQueue.front();
        readyQueue.pop();
        if (c != comp)
            q.push(c);
    }
    readyQueue.swap(q);
}

void ScriptSystem::RemoveImmediate(Component* comp)
{
    SwapErase(pending_components, comp);
    SwapErase(comps, comp);
    SwapErase(pending_scriptComponents, comp);
    SwapErase(scriptComps, comp);
    SwapErase(pending_scriptRemovals, comp);
    RemoveFromReadyQueue(comp);
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
