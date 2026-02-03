#pragma once
#include "Components/ScriptComponent.h"

class GameObject;

/*
    [ JesaSangManager Script Component ] <Singleton>

    플레이어 인터랙션으로 음식을 전달받고,
    모델 on/off를 관리합니다.
    게임 성공 데이터와 연결될 가능성이 있습니다. (GameManager)
*/

class JesaSangManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static JesaSangManager* s_instance = nullptr;

    // gameobjects
    GameObject* apple = nullptr;
    GameObject* pear = nullptr;
    GameObject* batter = nullptr;
    GameObject* tofu = nullptr;
    GameObject* sanjeok = nullptr;
    GameObject* dong = nullptr;

public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // 외부 call fucns..
    static JesaSangManager* Instance() { return s_instance; }


};

