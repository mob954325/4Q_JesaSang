#pragma once
#include "Components/ScriptComponent.h"

/*
    [ MainGameUIManager Script Component ] <Singleton>

    메인게임의 screen space에 있는 ui를 모두 소유하고
    다른 스크립트에게 유틸을 제공합니다.
*/

class MainGameUIManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static MainGameUIManager* s_instance = nullptr;

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // 외부 call fucns..
    static MainGameUIManager* Instance() { return s_instance; }
};

