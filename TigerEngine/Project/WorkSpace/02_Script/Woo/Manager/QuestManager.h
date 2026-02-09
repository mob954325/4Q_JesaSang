#pragma once
#include "Components/ScriptComponent.h"

/*
    [ GameManager Script Component ] <Singleton>

    메인 게임 퀘스트 매니저입니다.

    1. 
    2. 
    3. 
    4. 
*/

class QuestManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static QuestManager* s_instance = nullptr;

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // 외부 call fucns..
    static QuestManager* Instance() { return s_instance; }
};

