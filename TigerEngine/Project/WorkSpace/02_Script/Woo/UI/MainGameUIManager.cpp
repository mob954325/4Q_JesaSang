#include "MainGameUIManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Components/UI/TextUI.h"


REGISTER_COMPONENT(MainGameUIManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<MainGameUIManager>("MainGameUIManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr)
    .property("lifeOnImagePath", &MainGameUIManager::lifeOnImagePath)  (metadata(META_BROWSE, ""))
    .property("lifeOffImagePath", &MainGameUIManager::lifeOffImagePath) (metadata(META_BROWSE, ""));
}

namespace
{
    constexpr float kQuestOpenX = 1420.0f;
    constexpr float kQuestCloseX = 1920.0f;

    float Clamp01(float v)
    {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    // 보기 좋은 슬라이드용 이징(부드럽게)
    float EaseOutCubic(float t)
    {
        t = Clamp01(t);
        float u = 1.0f - t;
        return 1.0f - (u * u * u);
    }
}       // quest 연출


void MainGameUIManager::OnInitialize()
{
    // 중복 생성 방지
    if (s_instance != nullptr && s_instance != this)
    {
        assert(false && "Duplicate GameManager instance detected.");
        return;
    }

    s_instance = this;
}

void MainGameUIManager::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        cout << "[MainGameUIManager] Current scene is null!" << endl;
        return;
    }

    auto SafeGetGO = [&](const std::string& objName) -> GameObject*
        {
            auto go = scene->GetGameObjectByName(objName);
            if (!go)
                cout << "[MainGameUIManager] Missing GameObject: " << objName << endl;
            return go;
        };

    auto SafeGetImage = [&](const std::string& objName) -> Image*
        {
            auto go = SafeGetGO(objName);
            if (!go) return nullptr;

            auto img = go->GetComponent<Image>();
            if (!img)
                cout << "[MainGameUIManager] Missing Image Component on: " << objName << endl;

            return img;
        };

    auto SafeGetRect = [&](const std::string& objName) -> RectTransform*
        {
            auto go = SafeGetGO(objName);
            if (!go) return nullptr;

            auto rt = go->GetComponent<RectTransform>();
            if (!rt)
                cout << "[MainGameUIManager] Missing RectTransform Component on: " << objName << endl;

            return rt;
        };

    auto SafeGetText = [&](const std::string& objName) -> TextUI*
        {
            auto go = SafeGetGO(objName);
            if (!go) return nullptr;

            auto txt = go->GetComponent<TextUI>();
            if (!txt)
                cout << "[MainGameUIManager] Missing Text Component on: " << objName << endl;

            return txt;
        };


    // life image component
    life_1 = SafeGetImage("Image_Life1");
    life_2 = SafeGetImage("Image_Life2");
    life_3 = SafeGetImage("Image_Life3");
    life_4 = SafeGetImage("Image_Life4");
    life_5 = SafeGetImage("Image_Life5");

    if (!life_1 || !life_2 || !life_3 || !life_4 || !life_5)
    {
        cout << "[MainGameUIManager] Missing life UI objects!" << endl;
    }

    // quest
    questParent = SafeGetRect("Quest");
    questTitle = SafeGetText("Text_QuestTitle");
    questLable = SafeGetText("Text_QuestLabel");
    cheakbox = SafeGetImage("Image_QuestCheckbox");
    line = SafeGetImage("Image_QuestLine");


    if (!questParent || !questTitle || !questLable || !cheakbox || !line)
    {
        cout << "[MainGameUIManager] Missing quest UI objects!" << endl;
    }
}

void MainGameUIManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
}

nlohmann::json MainGameUIManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MainGameUIManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void MainGameUIManager::UpdateLifeUI(int currentLife)
{
    if (!life_1 || !life_2 || !life_3 || !life_4 || !life_5)
    {
        cout << "[MainGameUIManager] UpdateLifeUI failed: life images not initialized." << endl;
        return;
    }

    if (currentLife < 0) currentLife = 0;
    if (currentLife > 5) currentLife = 5;

    Image* lifes[5] = { life_1, life_2, life_3, life_4, life_5 };

    for (int i = 0; i < 5; i++)
    {
        if (i < currentLife)
            lifes[i]->ChangeData(lifeOnImagePath);
        else
            lifes[i]->ChangeData(lifeOffImagePath);
    }
}

void MainGameUIManager::UpdateQuestTitle(const wchar_t* s)
{
    if (!questTitle || !s) return;
    questTitle->SetText(std::wstring(s));
}

void MainGameUIManager::UpdateQuestLable(const wchar_t* s)
{
    if (!questLable || !s) return; // questLabel 변수명은 실제 멤버에 맞춰
    questLable->SetText(std::wstring(s));
}

void MainGameUIManager::SetQuestTitleOn(bool flag)
{
    if (!questTitle) return;
    questTitle->SetActive(flag);
}

void MainGameUIManager::SetQuestLableOn(bool flag)
{
    if (!questLable) return;
    questLable->SetActive(flag);
}

void MainGameUIManager::SetQuestCheakboxOn(bool flag)
{
    if (!cheakbox) return;
    cheakbox->SetActive(flag);
}

void MainGameUIManager::SetQuestLineOn(bool flag)
{
    if (!line) return;
    line->SetActive(flag);
}

void MainGameUIManager::QuestPannelOpen(float durationSec)
{
    if (!questParent) return;

    questPanelAnimating_ = true;
    questPanelTimer_ = 0.0f;
    questPanelDuration_ = (durationSec <= 0.0f) ? 0.0001f : durationSec;

    questPanelFromX_ = questParent->GetPos().x;
    questPanelToX_ = kQuestOpenX;
}

void MainGameUIManager::QuestPannelClose(float durationSec)
{
    if (!questParent) return;

    questPanelAnimating_ = true;
    questPanelTimer_ = 0.0f;
    questPanelDuration_ = (durationSec <= 0.0f) ? 0.0001f : durationSec;

    questPanelFromX_ = questParent->GetPos().x;
    questPanelToX_ = kQuestCloseX;
}

void MainGameUIManager::TickQuestPanel(float dt)
{
    if (!questParent || !questPanelAnimating_)
        return;

    questPanelTimer_ += dt;
    float t = questPanelTimer_ / questPanelDuration_;
    float eased = EaseOutCubic(t);

    Vector3 pos = questParent->GetPos();
    pos.x = Lerp(questPanelFromX_, questPanelToX_, eased);
    questParent->SetPos(pos);

    if (t >= 1.0f)
    {
        // 애니메이션 종료
        pos.x = questPanelToX_;
        questParent->SetPos(pos);

        questPanelAnimating_ = false;
    }
}
