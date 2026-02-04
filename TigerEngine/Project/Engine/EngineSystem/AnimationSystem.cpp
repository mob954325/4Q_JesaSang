#include "AnimationSystem.h"

void AnimationSystem::Register(AnimationController* comp)
{
    pending_anims.push_back(comp);
}

void AnimationSystem::UnRegister(AnimationController* comp)
{
    for (auto it = comps.begin(); it != comps.end(); it++)
    {
        if (*it == comp)
        {
            comps.erase(it);
            return;
        }
    }

    for (auto it = pending_anims.begin(); it != pending_anims.end(); it++)
    {
        if (*it == comp)
        {
            pending_anims.erase(it);
            return;
        }
    }
}

void AnimationSystem::Update(float dt)
{
    for (auto& controller : pending_anims)
    {
        comps.push_back(controller);
    }
    pending_anims.clear();


    for (auto* controller : comps)
    {
        if (controller)
            controller->Update(dt);
    }
}

std::vector<AnimationController*> AnimationSystem::GetComponents()
{
    return comps;
}

void AnimationSystem::Clear()
{
    comps.clear();
    pending_anims.clear();
}