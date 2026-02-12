#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class Image;
class RectTransform;
class AudioSourceComponent;

class SettingsUIController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    enum class SettingsTab
    {
        Sound,
        Credit
    };

    static SettingsUIController* s_instance;

public:
    SettingsUIController() { SetName("SettingsUIController"); }
    ~SettingsUIController() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    static SettingsUIController* Instance() { return s_instance; }

    void ToggleRoot();
    void SetOpen(bool open);
    void ShowSoundTab();
    void ShowCreditTab();

private:
    void EnsureClickAudioSource();
    void PlayClickSound();
    void BindSoundBarInputOnce();
    void BeginSoundBarDrag(int barIndex);
    void EndSoundBarDrag(int barIndex);
    void ResolveRefs();
    void CaptureSoundBarBase();
    void UpdateSoundBarVisuals();
    void UpdateSoundBarVisual(int barIndex, float value);
    float GetMouseBarValue(RectTransform* barRect) const;
    bool IsMouseOverRect(RectTransform* rect) const;
    void ApplyVolume(int barIndex, float value);

public:
    std::string soundPanelObjectName = "UI_Settings_SoundPanel";
    std::string creditPanelObjectName = "UI_Settings_CreditPanel";
    std::string escImageObjectName = "UI_Settings_ESC_Image";
    std::string soundButtonObjectName = "UI_Settings_SoundButton";
    std::string creditButtonObjectName = "UI_Settings_CreditButton";
    std::string closeButtonObjectName = "UI_Settings_CloseButton";
    std::string exitButtonObjectName = "";
    std::string menuRootObjectName = "Main Button Manager";

    std::string soundNormalPath = "..\\Assets\\Resource\\UI\\sound.png";
    std::string soundPressedPath = "..\\Assets\\Resource\\UI\\sound_click.png";
    std::string creditNormalPath = "..\\Assets\\Resource\\UI\\credit.png";
    std::string creditPressedPath = "..\\Assets\\Resource\\UI\\credit_click.png";
    std::string closeNormalPath = "..\\Assets\\Resource\\UI\\yes.png";
    std::string closePressedPath = "..\\Assets\\Resource\\UI\\yes_click.png";
    std::string exitNormalPath = "..\\Assets\\Resource\\UI\\yes.png";
    std::string exitPressedPath = "..\\Assets\\Resource\\UI\\yes_click.png";

    bool allowEscClose = false;
    bool startOpen = false;

private:
    GameObject* m_Root = nullptr;
    GameObject* m_SoundPanel = nullptr;
    GameObject* m_CreditPanel = nullptr;
    GameObject* m_EscImageObj = nullptr;
    GameObject* m_SoundButtonObj = nullptr;
    GameObject* m_CreditButtonObj = nullptr;
    GameObject* m_CloseButtonObj = nullptr;
    GameObject* m_ExitButtonObj = nullptr;
    GameObject* m_MenuRootObj = nullptr;
    GameObject* m_SoundBar1Bg = nullptr;
    GameObject* m_SoundBar1Gauge = nullptr;
    GameObject* m_SoundBar1Point = nullptr;
    GameObject* m_SoundBar2Bg = nullptr;
    GameObject* m_SoundBar2Gauge = nullptr;
    GameObject* m_SoundBar2Point = nullptr;
    AudioSourceComponent* m_ClickAudioSource = nullptr;
    Image* m_SoundButtonImage = nullptr;
    Image* m_CreditButtonImage = nullptr;
    Image* m_CloseButtonImage = nullptr;
    Image* m_ExitButtonImage = nullptr;
    Image* m_SoundBar1BgImage = nullptr;
    Image* m_SoundBar1GaugeImage = nullptr;
    Image* m_SoundBar1PointImage = nullptr;
    Image* m_SoundBar2BgImage = nullptr;
    Image* m_SoundBar2GaugeImage = nullptr;
    Image* m_SoundBar2PointImage = nullptr;
    RectTransform* m_SoundButtonRect = nullptr;
    RectTransform* m_CreditButtonRect = nullptr;
    bool m_ButtonBaseCaptured = false;
    Vector3 m_SoundButtonBasePos{};
    Vector3 m_CreditButtonBasePos{};
    Vector2 m_SoundButtonBaseSize{};
    Vector2 m_CreditButtonBaseSize{};

    RectTransform* m_SoundBar1BgRect = nullptr;
    RectTransform* m_SoundBar1GaugeRect = nullptr;
    RectTransform* m_SoundBar1PointRect = nullptr;
    RectTransform* m_SoundBar2BgRect = nullptr;
    RectTransform* m_SoundBar2GaugeRect = nullptr;
    RectTransform* m_SoundBar2PointRect = nullptr;
    Vector2 m_SoundBar1GaugeBaseSize{};
    Vector3 m_SoundBar1GaugeBasePos{};
    Vector3 m_SoundBar1PointBasePos{};
    Vector2 m_SoundBar2GaugeBaseSize{};
    Vector3 m_SoundBar2GaugeBasePos{};
    Vector3 m_SoundBar2PointBasePos{};
    bool m_SoundBarBaseCaptured = false;
    float m_SoundBar1Value = 1.0f;
    float m_SoundBar2Value = 1.0f;
    bool m_DraggingBar1 = false;
    bool m_DraggingBar2 = false;
    bool m_WasLeftDown = false;

    bool m_WasEscDown = false;
    bool m_IsOpen = false;
    bool m_SoundBarInputBound = false;
    SettingsTab m_CurrentTab = SettingsTab::Sound;
    bool m_TabSwitchInProgress = false;
};
