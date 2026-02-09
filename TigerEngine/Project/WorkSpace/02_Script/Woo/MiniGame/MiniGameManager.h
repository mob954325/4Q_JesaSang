#pragma once
#include "Components/ScriptComponent.h"
#include "Game/IMiniGame.h"
#include <directxtk/Keyboard.h>

class IItem;
class RectTransform;
class Image;

/*
    [ MiniGameManager Script Component ] <Singleton>

    음식에 맞는 미니게임을 실행시키고, 미니게임 라이프 사이클을 관리하는 매니저

    Singleton Script Component로,
    플레이어가 조리대와 인터랙션 할 때 Instance에 접근하여 게임을 시작합니다.
    Minigame은 총 3가지로, Player가 소지한 음식 재료에 따라 다른 미니 게임이 진행됩니다.

    - UI -
    Backgournd : 1500(아래) <-> 550
*/

class MiniGameManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static MiniGameManager* s_instance = nullptr;

    // game
    std::unique_ptr<IMiniGame> currentMiniGame;
    std::unique_ptr<IItem> curIngredient;

    // UI
    RectTransform* miniGameParent;      // 시작/끝날때 밑에서 올라오고 내려가는 연출
    GameObject* success;                // 게임 성공
    GameObject* fail;                   // 게임 실패
    GameObject* miniGame1_Parent;       // on, off (Cutting : 스타포스)
    GameObject* miniGame2_Parent;       // on, off (Fire : 스듀낚시)
    GameObject* miniGame3_Parent;       // on, off (Assembling : 키입력)
    // 각 miniGame 안의 UI controll은 각 IMinigame 안에서 선언하고 제어하기.

    // popup 연출
    Vector3 popupHiddenPos;
    Vector3 popupShownPos;
    Vector3 popupTargetPos;
    float popupSpeed = 12.0f;
    bool isPopupMoving = false;

    // pause esc 우선순위 제어
    bool isPopupHiding = false;
    bool isPopupHidden = true;

    // result pause (미니게임 결과보라는 느낌스)
    bool  isResultWaiting = false;
    float resultWaitTimer = 0.0f;
    float resultWaitDuration = 2.0f;     // 멈춤 타임
    bool  cachedResultSuccess = false;   // 결과 저장(연출용)

    // result end pending (팝업 완전히 내려간 뒤 EndMiniGame 호출하기 위함)
    bool  pendingEnd = false;

public:
    // data
    Keyboard::Keys stop_key = Keyboard::Escape;

    // controll
    bool isPlaying = false;

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs..
    // start/end
    std::unique_ptr<IMiniGame> CreateMinigameForIngredientId(const std::string& foodId);
    void EndMiniGame(bool isSuccess);   // 미니게임 종료시 (성공/실패+패널티)
    void StopMiniGame();                // 미니게임 강제 종료시 (재료만 반환)
    void CleanupMinigame();

    // update
    void StopChecking();

    // 연출
    void UpdatePopup(float delta);
    void ShowPopup();
    void HidePopup();

public:
    // 외부 call fucns..
    static MiniGameManager* Instance() { return s_instance; }

    void StartMiniGame(std::unique_ptr<IItem> ingredient);         // 플레이어 조리대 인터랙션

    bool IsPopupHiding() const { return isPopupHiding; }
};
