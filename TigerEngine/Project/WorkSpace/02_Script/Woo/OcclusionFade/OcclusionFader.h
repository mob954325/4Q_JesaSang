#pragma once
#include "Components/ScriptComponent.h"
#include "EngineSystem/PhysicsSystem.h"
#include <Util/PhysXUtils.h>
#include <unordered_set>
#include <vector>

class OcclusionFadeObject;

/*
    [ OcclusionFader Script Component ]

    target으로 ray를 쏴서, Occulsion Fader Object들을 찾아 투명화/복구를 처리하는 컴포넌트
*/

class OcclusionFader : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    Transform* transform = nullptr;
    Transform* targetTr = nullptr;

    // hit objects
    std::unordered_set<OcclusionFadeObject*> previousHits;
    std::unordered_set<OcclusionFadeObject*> currentHits;
    std::vector<RaycastHit> hitBuffer;


public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    OcclusionFadeObject* FindFadeObjectFromHit(const RaycastHit& hit);
};

