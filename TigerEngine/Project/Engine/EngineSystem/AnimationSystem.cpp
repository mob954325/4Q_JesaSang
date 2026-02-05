#include "AnimationSystem.h"

void AnimationSystem::Register(AnimationController* comp)
{
    pending_anims.push_back(comp);
}

void AnimationSystem::UnRegister(AnimationController* comp)
{
    //for (auto it = comps.begin(); it != comps.end(); it++)
    //{
    //    if (*it == comp)
    //    {
    //        comps.erase(it);
    //        return;
    //    }
    //}

    // To. 성호 
    // 미안하다 성호야.. 구조 안맞춰둔 더러운 코드에 열심히 너가 넣어줬더라...? 

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