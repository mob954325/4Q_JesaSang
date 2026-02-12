#pragma once
#include "../Engine/Components/ScriptComponent.h"
#include "../Engine/Components/UI/Image.h"

class MenuUI_StartButton : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    MenuUI_StartButton() { SetName("MenuUI_StartButton"); }

    void OnInitialize() override;
    void OnEnable() override;
    void OnStart() override;
    void OnDisable() override;
    void OnDestory() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    const std::string& GetPressImagePath() const;
    void SetPressImagePath(const std::string& path);

    /// 눌렀을 때 사용할 이미지 경로 
    std::string pressedImagePath = "";
    std::string normalImagePath = "";    // image가 처음 가지고 있는 이미지 경로 ( 복구 하기 위함 )    
    std::string targetScenePath = "";

private:
    void EnsureClickAudioSource();
    void PlayClickSound();

    Image* image{};                 // 해당 게임 오브젝트가 가지고 있는 이미지 컴포넌트
    class AudioSourceComponent* m_ClickAudioSource = nullptr;
};
