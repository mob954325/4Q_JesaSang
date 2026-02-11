#pragma once
#include "pch.h"
#include "System/Singleton.h"
#include "../RenderPass/ParticleSource/Effect.h"

/*
    [ Effect System ]

    Effect Component를 Register하고 관리합니다.
    Effect Component는 따로 Update나 Render되지 않는 데이터 컴포넌트입니다.
    => EffectPass에서 사용됩니다.
*/

class EffectSystem : public Singleton<EffectSystem>
{
private:
    std::vector<Effect*> comps{};

public:
    EffectSystem(token) {}
    ~EffectSystem() = default;

    void Register(Effect* comp);
    void UnRegister(Effect* comp);

    void Update();
    void Clear();

    std::vector<Effect*> GetComponents();
};

