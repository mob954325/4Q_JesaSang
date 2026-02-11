#include "EffectSystem.h"

void EffectSystem::Register(Effect* comp)
{
    comps.push_back(comp);
}

void EffectSystem::UnRegister(Effect* comp)
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

void EffectSystem::Update()
{
    for (auto& fx : comps)
    {
        if (!fx) continue;
        if (!fx->transform)
            fx->OnInitialize();
        if (!fx->transform)
            continue;
        fx->Update();
    }
}

void EffectSystem::Clear()
{
    comps.clear();
}

std::vector<Effect*> EffectSystem::GetComponents()
{
    return comps;
}