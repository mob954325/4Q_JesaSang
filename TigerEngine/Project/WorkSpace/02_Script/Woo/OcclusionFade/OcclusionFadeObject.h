#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"

/*
    [ OcclusionFadeObject Script Component ]

    카메라 시점에서 플레이어를 가릴 경우,
    투명화를 적용할 오브젝트에 붙을 컴포넌트

    - Wall
*/

class OcclusionFadeObject : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    FBXRenderer* fbxRenderer = nullptr;

    // values
    float fadeDuration = 0.4f;
    float fadeInOpacity = 0.3f;     // 투명화 목표값

    // controll
    bool isFading = false;          // 투명화/복구 진행중 flag
    float currentFadeTime = 0.0f;

    float startOpacity = 1.0f;
    float endOpacity = 1.0f;
    float currentOpacity = 1.0f;


public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    void OcclusionFadeProcess(float delta);     // 투명화/복구 진행 처리
    void OcclusionFadeReset();                  // 투명화/복구 종료 처리

    void BeginFade(float newTargetOpacity);
    void ApplyOpacity(float opacity);

public:
    // 외부 call funcs..
    void StartFadeIn();         // 투명화 시작
    void StartFadeOut();        // 원상복귀 시작
};

