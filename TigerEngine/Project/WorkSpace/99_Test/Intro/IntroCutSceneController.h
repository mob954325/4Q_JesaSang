#pragma once

#include "Components/ScriptComponent.h"
#include <directxtk/Keyboard.h>

class Image;
class RectTransform;

class IntroCutSceneController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    IntroCutSceneController() { SetName("IntroCutSceneController"); }
    ~IntroCutSceneController() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    std::string imageObjectName = "UI_Intro_Image";
    std::string frameObjectName = "UI_Intro_Frame";
    std::string nextScenePath = "..\\Assets\\Scenes\\01_StartScene.json";

    bool allowSkip = true;

private:
    void LoadFrameList();
    void ApplyFrame(int index);
    void ApplyLayout();
    void LoadNextScene();

    Image* m_Image = nullptr;
    RectTransform* m_ImageRect = nullptr;
    Image* m_FrameImage = nullptr;
    RectTransform* m_FrameRect = nullptr;

    std::vector<std::string> m_Frames;
    int m_CurrentIndex = 0;

    bool m_LayoutApplied = false;
    bool m_PrevSpace = false;
    bool m_PrevEnter = false;
    bool m_PrevEsc = false;
};
