#include "HappyEndingPanel.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/EngineSystem/SceneSystem.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"

static bool IsAnyKeyDown_Win32()
{
    // 1~254 범위(VK) 스캔. (0은 reserved, 255는 없음)
    for (int vk = 1; vk < 255; ++vk)
    {
        // 최상위 비트가 1이면 현재 눌림 상태
        if (GetAsyncKeyState(vk) & 0x8000)
            return true;
    }
    return false;
}

REGISTER_COMPONENT(HappyEndingPanel);

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<HappyEndingPanel>("HappyEndingPanel")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void HappyEndingPanel::OnInitialize()
{
    effectsPath.resize(8);
    for (int i = 0; i < 8; ++i)
    {
        effectsPath[i] = "..\\Assets\\Resource\\UI\\CutScenes\\happy_ending_0" + std::to_string(i) + ".png";
    }
}

void HappyEndingPanel::OnStart()
{
    image = GetOwner()->GetComponent<Image>();
    if (image)
        image->GetTextureByPath(effectsPath[0]);
}

void HappyEndingPanel::OnUpdate(float dt)
{
    timer += dt;

    if (timer > maxTimer && index < effectsPath.size())
    {        
        image->GetTextureByPath(effectsPath[index]);
        index++;
        timer = 0.0f;
    }

    if (!waitingAnyKey && index == effectsPath.size())
    {
        waitingAnyKey = true;

        // TODO:
        // 1) "돌아가기" 텍스트 표시(예: UI Text 컴포넌트 생성/활성화)
        // 2) "아무 키나 누르세요" 텍스트 표시
    }

    // maxTime 이후: 아무 키 누르면 한 번만 처리
    if (waitingAnyKey && !handled)
    {
        if (IsAnyKeyDown_Win32())
        {
            handled = true;

            // TODO: 돌아가기 처리
            // 예) 씬 전환, 패널 비활성화, 메인 메뉴로 이동 등
            auto s = SceneSystem::Instance().GetCurrentScene();
            s->LoadToJson("..\\Assets\\Scenes\\01_StartScene.json");
                //GetOwner()->SetActive(false);
        }
    }
}

nlohmann::json HappyEndingPanel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void HappyEndingPanel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

