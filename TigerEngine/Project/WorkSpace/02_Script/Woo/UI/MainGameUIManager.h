#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class RectTransform;
class Image;
class TextUI;

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

    // --- [Screen UI] -----------------
    // player life
    Image* life_1 = nullptr;
    Image* life_2 = nullptr;
    Image* life_3 = nullptr;
    Image* life_4 = nullptr;
    Image* life_5 = nullptr;

    // quest
    RectTransform* questParent = nullptr;       // x : 1420.0 ~ 1920
    TextUI* questTitle = nullptr;
    TextUI* questLable = nullptr;
    Image* cheakbox = nullptr;
    Image* line = nullptr;

    // quest panel animation state
    bool questPanelAnimating_ = false;
    float questPanelTimer_ = 0.0f;
    float questPanelDuration_ = 1.0f;
    float questPanelFromX_ = 1420.0f;
    float questPanelToX_ = 1420.0f;

public:
    // rttr
    std::string lifeOnImagePath = "";  
    std::string lifeOffImagePath = "";

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // image path
    const std::string& GetPressImagePath() const;
    void SetPressImagePath(const std::string& path);

public:
    // 외부 call fucns..
    static MainGameUIManager* Instance() { return s_instance; }

    // Life
    void UpdateLifeUI(int currentLife);

    // Quest
    void UpdateQuestTitle(const wchar_t* s);
    void UpdateQuestLable(const wchar_t* s);

    void SetQuestTitleOn(bool flag);
    void SetQuestLableOn(bool flag);
    void SetQuestCheakboxOn(bool flag);
    void SetQuestLineOn(bool flag);

    void QuestPannelOpen(float durationSec);
    void QuestPannelClose(float durationSec);
    void TickQuestPanel(float dt); // 퀘스트창 슬라이드 연출
};

