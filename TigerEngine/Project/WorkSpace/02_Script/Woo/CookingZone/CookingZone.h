#pragma once
#include "Components/ScriptComponent.h"

class Image;

/*
    [ CookingZone Script Component ] <Singleton>

    Cooking Zone UI 인터랙션용으로 급하게 만듦

*/


class CookingZone : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static CookingZone* s_instance = nullptr;

    // child UI
    Image* image_sensorOn = nullptr;
    Image* image_interactionOn = nullptr;
    Image* image_interactionGauge = nullptr;

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);


public:
    // 외부 call fucns..
    static CookingZone* Instance() { return s_instance; }

    // UI
    void UISensorOnOff(bool flag);              // 플레이어 감지영역 UI
    void UIInteractionOnOff(bool flag);         // 플레이어 상호작용 영역 UI
    void UIGaugeUpate(float progress);
};

