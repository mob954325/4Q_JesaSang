#include "MiniGameManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"

#include "../Item/Item.h"
#include "Game/IMinigame.h"
#include "Game/Game_Cutting.h"
#include "Game/Game_FireControl.h"
#include "Game/Game_Assembling.h"

#include "../Player/PlayerController.h"


REGISTER_COMPONENT(MiniGameManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<MiniGameManager>("MiniGameManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void MiniGameManager::OnInitialize()
{
    s_instance = this;
}

void MiniGameManager::OnUpdate(float delta)
{
    // game update
    if (currentMiniGame)
    {
        currentMiniGame->UpdateGame(delta);
        if (currentMiniGame->IsFinished())
        {
            EndMiniGame(currentMiniGame->IsSuccess());
        }
    }
}

void MiniGameManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
    CleanupMinigame();
}

nlohmann::json MiniGameManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MiniGameManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

std::unique_ptr<IMiniGame> MiniGameManager::CreateMinigameForIngredientId(const std::string& foodId)
{
    // 음식 재료에 따른 미니게임 선택
    if (foodId == "Ingredient_Apple" || foodId == "Ingredient_Pear")
    {
        return std::make_unique<Game_Cutting>();
    }
    if (foodId == "Ingredient_Batter" || foodId == "Ingredient_Tofu")
    {
        return std::make_unique<Game_FireControl>();
    }
    if (foodId == "Ingredient_Sanjeok" || foodId == "Ingredient_Donggeurangttaeng")
    {
        return std::make_unique<Game_Assembling>();
    }

    return nullptr;
}

void MiniGameManager::CleanupMinigame()
{
    if (currentMiniGame)
    {
        currentMiniGame->EndGame();
        currentMiniGame.reset();
    }
    curIngredient.reset(); 
}

void MiniGameManager::StartMiniGame(std::unique_ptr<IItem> ingredient)
{
    if (!ingredient)
        return;

    if (ingredient->itemType != ItemType::Ingredient)
        return;

    // mini game create
    const std::string foodId = ingredient->itemId;
    auto newGame = CreateMinigameForIngredientId(foodId);
    if (!newGame) return;
        
    // ownership
    curIngredient = std::move(ingredient);
    currentMiniGame = std::move(newGame);

    // start game
    currentMiniGame->StartGame();
}

void MiniGameManager::EndMiniGame(bool isSuccess)
{
    // end game
    if (currentMiniGame)
    {
        currentMiniGame->EndGame();
        currentMiniGame.reset();
    }

    // ingredient
    if (curIngredient)
    { 
        // 게임 성공이면 재료 소멸
        if (isSuccess)
        {
            curIngredient.reset();
            cout << "[MiniGameManager] : Game Success!" << endl;
        }
        // TODO :: 실패시 플레이어에게 반환
        else
        {
            // std::move(curIngredient)  player inventory
            curIngredient.reset();      // 임시
            cout << "[MiniGameManager] : Game Fail..." << endl;
        }
    }

    // player fsm change
    auto* pc = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player")->GetComponent<PlayerController>();
    pc->ChangeState(PlayerState::Idle);
}
