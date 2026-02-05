#pragma once
#include "Components/ScriptComponent.h"

/*
    [ GameManager Script Component ] <Singleton>

    게임매니저입니다.
*/

class GameManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static GameManager* s_instance = nullptr;

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs..


public:
    // 외부 call fucns..
    static GameManager* Instance() { return s_instance; }

    void GameSuccess();
    void GameOver();
};

