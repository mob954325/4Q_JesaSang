#include "MiniGameManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"

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

void MiniGameManager::OnStart()
{
    miniGameParent = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("UI_MiniGame")->GetComponent<RectTransform>();
    miniGame1_Parent = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("UI_MiniGame1");
    miniGame2_Parent = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("UI_MiniGame2");
    miniGame3_Parent = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("UI_MiniGame3");

    if (!miniGameParent || !miniGame1_Parent || !miniGame2_Parent || !miniGame3_Parent)
    {
        cout << "[MiniGameManager] Missing ui objects...!!!" << endl;
    }

    // init
    popupHiddenPos = Vector3(960, 1500, 0);
    popupShownPos = Vector3(960, 550, 0);

    popupTargetPos = popupHiddenPos;
    miniGameParent->SetPos(popupHiddenPos);
}

void MiniGameManager::OnUpdate(float delta)
{
    // popup move
    UpdatePopup(delta);

    // game update
    if (isPlaying && currentMiniGame)
    {
        currentMiniGame->UpdateGame(delta);
        if (currentMiniGame->IsFinished())
        {
            EndMiniGame(currentMiniGame->IsSuccess());
        }
    }

    // stop cheking
    StopChecking();
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
        miniGame1_Parent->SetActive(true);
        miniGame2_Parent->SetActive(false);
        miniGame3_Parent->SetActive(false);
        return std::make_unique<Game_Cutting>();
    }
    if (foodId == "Ingredient_Batter" || foodId == "Ingredient_Tofu")
    {
        miniGame1_Parent->SetActive(false);
        miniGame2_Parent->SetActive(true);
        miniGame3_Parent->SetActive(false);
        return std::make_unique<Game_FireControl>();
    }
    if (foodId == "Ingredient_Sanjeok" || foodId == "Ingredient_Donggeurangttaeng")
    {
        miniGame1_Parent->SetActive(false);
        miniGame2_Parent->SetActive(false);
        miniGame3_Parent->SetActive(true);
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

    miniGameParent->GetOwner()->SetActive(true);

    // mini game create
    const std::string foodId = ingredient->itemId;
    auto newGame = CreateMinigameForIngredientId(foodId);
    if (!newGame) return;
        
    // ownership
    curIngredient = std::move(ingredient);
    currentMiniGame = std::move(newGame);

    // start game
    currentMiniGame->StartGame();
    isPlaying = true;

    // popup ui
    ShowPopup();
}

// 미니게임 종료시 (성공/실패+패널티)
void MiniGameManager::EndMiniGame(bool isSuccess)
{
    isPlaying = false;

    // popup ui
    HidePopup();

    // end game
    if (currentMiniGame)
    {
        currentMiniGame->EndGame();
        currentMiniGame.reset();
    }

    // get playercontroller
    auto* pc = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player")->GetComponent<PlayerController>();
    if (!pc)
    {
        cout << "[MiniGameManager] Missing Player Controller!" << endl;
        return;
    }

    // result return
    if (curIngredient)
        pc->ReceiveMiniGameResult(std::move(curIngredient), isSuccess);

    // player fsm change
    pc->ChangeState(PlayerState::Idle);
}

// 미니게임 강제 종료시 (재료만 반환)
void MiniGameManager::StopMiniGame()
{
    isPlaying = false;

    // popup ui
    HidePopup();

    // end game
    if (currentMiniGame)
    {
        currentMiniGame->EndGame();
        currentMiniGame.reset();
    }

    // get playercontroller
    auto* pc = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player")->GetComponent<PlayerController>();
    if (!pc) 
    {
        cout << "[MiniGameManager] Missing Player Controller!" << endl;
        return;
    }

    // ingredient 돌려주기
    if (curIngredient)
        pc->ReceiveMiniGameItem(std::move(curIngredient));

    // player fsm change
    pc->ChangeState(PlayerState::Idle);
}

void MiniGameManager::StopChecking()
{
    if (Input::GetKeyDown(stop_key) && isPlaying)
    {
        StopMiniGame();
    }
}

void MiniGameManager::UpdatePopup(float delta)
{
    if (!miniGameParent) return;

    Vector3 cur = miniGameParent->GetPos();
    Vector3 next = Vector3::Lerp(cur, popupTargetPos, std::min(1.0f, popupSpeed * delta));
    miniGameParent->SetPos(next);

    // 도착 판정
    Vector3 diff = popupTargetPos - next;
    if (diff.Length() < 0.5f)
    {
        miniGameParent->SetPos(popupTargetPos);

        // pause controll
        // 팝업이 완전히 다 내려간 뒤, 게임 일시정지 가능
        if (isPopupHiding)
        {
            isPopupHiding = false;
            isPopupHidden = true;
        }
    }
}

void MiniGameManager::ShowPopup()
{
    popupTargetPos = popupShownPos;
    isPopupHiding = false;
    isPopupHidden = false;
}

void MiniGameManager::HidePopup()
{
    popupTargetPos = popupHiddenPos;
    isPopupHiding = true;
}
