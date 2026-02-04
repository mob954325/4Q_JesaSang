#include "RenderSystem.h"
#include "System/TimeSystem.h"
#include "../Components/RenderComponent.h"
#include "../RenderQueue/RenderQueue.h"
#include "../Object/GameObject.h"

void RenderSystem::CheckReadyQueue()
{
    while (!readyQueue.empty())
    {
        auto comp = readyQueue.front();
        comp->OnStart();
        comp->SetStartTrue();
        readyQueue.pop();
    }
}

void RenderSystem::Register(RenderComponent* comp)
{
    if(!comp->IsStart()) 
        readyQueue.push(comp);

    pending_renderComponents.push_back(comp);
}

void RenderSystem::UnRegister(RenderComponent* comp)
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

void RenderSystem::Render(RenderQueue& queue)
{
    for (auto& e : pending_renderComponents)
    {
        comps.push_back(e);
    }
    pending_renderComponents.clear();

    for (auto& e : comps)
    {
        if (e->IsStart())
        {
            e->OnUpdate(GameTimer::Instance().DeltaTime());
            e->OnRender(queue);
        }
    }
}

void RenderSystem::Clear()
{
    comps.clear();
    while (!readyQueue.empty()) readyQueue.pop();
    pending_renderComponents.clear();
}
