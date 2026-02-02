#pragma once
#include "Components/ScriptComponent.h"
#include "Game/IMiniGame.h"

class IItem;

/* 
    [ MiniGameManager Script Component ]
    
    음식에 맞는 미니게임을 실행시키고, 미니게임 라이프 사이클을 관리하는 매니저

    Singleton Script Component로,
    플레이어가 조리대와 인터랙션 할 때 Instance에 접근하여 게임을 시작합니다.
    Minigame은 총 3가지로, Player가 소지한 음식 재료에 따라 다른 미니 게임이 진행됩니다.
*/

class MiniGameManager :  public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static MiniGameManager* s_instance = nullptr;

    // game
    std::unique_ptr<IMiniGame> currentMiniGame;
    std::unique_ptr<IItem> curIngredient;   // 재료는 게임실패시 반환, 성공시 소멸

public:
    // component process
    void OnInitialize() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs..
    std::unique_ptr<IMiniGame> CreateMinigameForIngredientId(const std::string& foodId);
    void CleanupMinigame();
    void EndMiniGame(bool isSuccess);

public:
    // 외부 call fucns..
    static MiniGameManager* Instance() { return s_instance; }

    void StartMiniGame(std::unique_ptr<IItem> ingredient);         // 플레이어 조리대 인터랙션
};

