#pragma once
#include <memory>
#include <deque>
#include <string>
#include <unordered_map>
#include "Components/ScriptComponent.h"

class GameObject;
class Camera;
class Image;
class IItem;

/*
    [ AltarManager Script Component ] <Singleton>

    플레이어가 떨어뜨린 아이템(음식재료/완성된 음식)이 올라오게 됩니다.
    플레이어는 제단에 있는 아이템을 다시 회수해갈 수 있습니다.

    제단은 초기 비활성화 되어있으며, 제단에 아이템이 처음 올라간 순간 활성화 됩니다.
    

    0209 삭제 | 제단에는 한번에 1개의 아이템만 올라와있을 수 있습니다. (퀘스트 시스템 제어)
    0209 갱신 | 제단에는 여러개의 아이템이 올라올 수 있습니다.
                플레이어는 여전히 하나의 아이템만 회수 가능하며 FIFO 입니다.
*/

class AltarManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static AltarManager* s_instance = nullptr;

    // item queue (FIFO)
    std::deque<std::unique_ptr<IItem>> itemQueue;

    // visual gameobjects
    GameObject* ingre_apple = nullptr;
    GameObject* ingre_pear = nullptr;
    GameObject* ingre_batter = nullptr;
    GameObject* ingre_tofu = nullptr;
    GameObject* ingre_sanjeok = nullptr;
    GameObject* ingre_dong = nullptr;

    GameObject* food_apple = nullptr;
    GameObject* food_pear = nullptr;
    GameObject* food_batter = nullptr;
    GameObject* food_tofu = nullptr;
    GameObject* food_sanjeok = nullptr;
    GameObject* food_dong = nullptr;

    // child UI
    Image* image_sensorOn = nullptr;
    Image* image_interactionOn = nullptr;
    Image* image_interactionGauge = nullptr;


private:
    // 제단 최초 활성화 연출
    GameObject* altar = nullptr;       // 활성화/비활성화할 제단 덩어리 부모 게임오브젝트
    bool isFirstReceiveItem = false;   // 최초 트리거
   
    Transform* altarDirectCam = nullptr;          // 연출 카메라
    std::string camName = "AltarDirectCamera";    // 연출 카메라 이름
    
    
    enum class DirectPhase
    {
        None,
        FadeOut_1,
        SwitchToDirectCam,
        FadeIn_1_And_ZoomIn,
        VisualOn,
        FadeOut_2,
        SwitchToMainCam,
        FadeIn_2,
        Done
    };

    bool isDirecting = false;
    DirectPhase directPhase = DirectPhase::None;
    float phaseTimer = 0.0f;

    // 연출용 파라미터
    float fadeOutTime_1 = 3.0f;
    float fadeInTime_1 = 1.8f;
    float zoomInTime = 5.0f;      // 카메라 이동 시간
    float visualHoldTime = 3.0f;  // 비주얼 on 후 잠깐 홀드

    float fadeOutTime_2 = 1.8f;
    float fadeInTime_2 = 3.0f;

    // 연출 비주얼 대상 아이템
    std::string directingItemId;

    // 연출 중 카메라 줌(Transform 이동) 보정
    Vector3 directCamStartPos = { 0,0,0 };
    Vector3 directCamTargetPos = { 0,0,0 };
    bool hasDirectCamPosCached = false;

    // --- vignette fade helpers ---
    struct VignetteBackup
    {
        bool valid = false;
        BOOL useVignette = false;
        float intensity = 1.0f;
        float smoothness = 0.0f;
        Vector2 center = { 0.5f, 0.5f };
        Vector3 color = { 0,0,0 };
    };

    VignetteBackup vignetteBackup;
    float vignetteFrom = 0.0f;
    float vignetteTo = 0.0f;
    float vignetteDuration = 0.0f;

    // 연출 유틸
    void BackupPostProcess();                 // 기존 PP 백업
    void RestorePostProcess();                // 연출 끝나면 원복
    void StartVignetteFade(float from, float to, float duration);
    void UpdateVignetteFade(float dt);
    static float EaseInOut(float t);          // 부드러운 페이드 커브
    static float Clamp01(float v);
    
    // 연출 함수
    void FirstReceiveDirect(std::string itemId);  // 최초 제단 활성화 연출
    void SetAllVisualOff();                       // 모든 제단 비주얼 OFF
    void BeginDirectSequence(std::string itemId);
    void UpdateDirectSequence(float dt);



public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);


public:
    // 외부 call fucns..
    static AltarManager* Instance() { return s_instance; }

    // Item
    bool HasItem() const { return (!itemQueue.empty()); }
    size_t GetItemCount() const { return itemQueue.size(); }

    // visual on/off
    void VisualItem(std::string itemID, bool isActive);

    void ReceiveItem(std::unique_ptr<IItem> item);  // 제단에 아이템 올리기
    std::unique_ptr<IItem> GetItem();               // 제단 아아템 회수하기 (FIFO)

    // Directing
    bool IsAltarFirstDirecting() { return isDirecting; }

    // UI
    void UISensorOnOff(bool flag);              // 플레이어 감지영역 UI
    void UIInteractionOnOff(bool flag);         // 플레이어 상호작용 영역 UI
    void UIGaugeUpate(float progress);
};

