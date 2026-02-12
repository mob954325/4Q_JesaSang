#pragma once

#include "Components/ScriptComponent.h"
#include <directxtk/Keyboard.h>

class Image;
class RectTransform;
class AudioSourceComponent;

class StartSceneController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    StartSceneController() { SetName("StartSceneController"); }
    ~StartSceneController() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    std::string imageObjectName = "UI_Start_Image";
    std::string nextScenePath = "..\\Assets\\Scenes\\02_IntroScene.json";

private:
    void ApplyLayout();
    void EnsureAudioSource();
    void PlayTitleBgm();
    void LoadNextScene();
    static bool IsAdvancePressed(bool down, bool& prevDown);

    Image* m_Image = nullptr;
    RectTransform* m_ImageRect = nullptr;
    AudioSourceComponent* m_AudioSource = nullptr;

    bool m_LayoutApplied = false;
    bool m_PrevSpace = false;
    bool m_PrevEnter = false;
};
