#include "Effect.h"
#include "../Engine/EngineSystem/EffectSystem.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/Util/JsonHelper.h"

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Effect>("Effect")
        .constructor<>()(policy::ctor::as_raw_ptr) // 엔진 스타일에 맞게 as_object/as_raw_ptr 선택
        // --- properties ---
        .property("emitters", &Effect::emitters)
        .property("allFinished", &Effect::allFinished)

        .property("position", &Effect::position)

        .property("enabled", &Effect::enabled)
            (metadata(META_BOOL, true))
        .property("playing", &Effect::playing)
            (metadata(META_BOOL, true))
        .property("looping", &Effect::looping)
            (metadata(META_BOOL, true));
}

void Effect::Play()
{
    playing = true;

    for (auto& e : emitters)
    {
        e.playing = true;
        e.position = position + e.localOffset;
        e.elapsed = 0.0f;
        e.emitAcc = 0.0f;
        e.particles.clear();

        // burst
        if (e.burstCount > 0)
            e.Spawn(e.burstCount);
    }
}

void Effect::Stop()
{
    playing = false;
    for (auto& e : emitters) e.playing = false;
}

void Effect::Update()
{
    if (!enabled || !playing) return;

    allFinished = true;

    // emitter udpate
    for (auto& e : emitters)
    {
        e.position = e.localOffset + position;
        e.Update();

        if (e.playing || !e.particles.empty())
            allFinished = false;
    }

    // loop
    if (allFinished)
    {
        if (looping)
            Play();
        else
            playing = false;
    }
}

void Effect::Enable_Inner()
{
    EffectSystem::Instance().Register(this);
}

void Effect::Disable_Inner()
{
    EffectSystem::Instance().UnRegister(this);
}


nlohmann::json Effect::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Effect::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
