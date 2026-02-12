#include "SettingsUIController.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "Components/UI/Image.h"
#include "Components/RectTransform.h"
#include "Object/GameObject.h"
#include "../../../../Base/System/InputSystem.h"
#include "Manager/AudioManager.h"

#include <directXTK/Mouse.h>

REGISTER_COMPONENT(SettingsUIController);

RTTR_REGISTRATION
{
    rttr::registration::class_<SettingsUIController>("SettingsUIController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("SoundPanelObjectName", &SettingsUIController::soundPanelObjectName)
        .property("CreditPanelObjectName", &SettingsUIController::creditPanelObjectName)
        .property("EscImageObjectName", &SettingsUIController::escImageObjectName)
        .property("SoundButtonObjectName", &SettingsUIController::soundButtonObjectName)
        .property("CreditButtonObjectName", &SettingsUIController::creditButtonObjectName)
        .property("CloseButtonObjectName", &SettingsUIController::closeButtonObjectName)
        .property("ExitButtonObjectName", &SettingsUIController::exitButtonObjectName)
        .property("MenuRootObjectName", &SettingsUIController::menuRootObjectName)
        .property("SoundNormalPath", &SettingsUIController::soundNormalPath)
        .property("SoundPressedPath", &SettingsUIController::soundPressedPath)
        .property("CreditNormalPath", &SettingsUIController::creditNormalPath)
        .property("CreditPressedPath", &SettingsUIController::creditPressedPath)
        .property("CloseNormalPath", &SettingsUIController::closeNormalPath)
        .property("ClosePressedPath", &SettingsUIController::closePressedPath)
        .property("ExitNormalPath", &SettingsUIController::exitNormalPath)
        .property("ExitPressedPath", &SettingsUIController::exitPressedPath)
        .property("AllowEscClose", &SettingsUIController::allowEscClose)
        .property("StartOpen", &SettingsUIController::startOpen);
}

SettingsUIController* SettingsUIController::s_instance = nullptr;

void SettingsUIController::OnInitialize()
{
    if (!s_instance)
    {
        s_instance = this;
    }
}

void SettingsUIController::OnStart()
{
    m_Root = GetOwner();
    ResolveRefs();
    CaptureSoundBarBase();
    UpdateSoundBarVisuals();
    SetOpen(startOpen);
}

void SettingsUIController::OnDestory()
{
    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

void SettingsUIController::OnUpdate(float delta)
{
    (void)delta;

    if (allowEscClose)
    {
        const bool isEscDown = Input::GetKeyDown(DirectX::Keyboard::Keys::Escape);
        if (isEscDown && !m_WasEscDown)
        {
            if (m_IsOpen)
            {
                SetOpen(false);
            }
        }
        m_WasEscDown = isEscDown;
    }

    if (!m_IsOpen)
    {
        m_DraggingBar1 = false;
        m_DraggingBar2 = false;
        m_WasLeftDown = false;
        return;
    }

    ResolveRefs();
    CaptureSoundBarBase();

    auto ms = DirectX::Mouse::Get().GetState();
    bool leftDown = (ms.leftButton != 0);

    if (leftDown && !m_WasLeftDown)
    {
        if (IsMouseOverRect(m_SoundBar1PointRect) || IsMouseOverRect(m_SoundBar1BgRect) || IsMouseOverRect(m_SoundBar1GaugeRect))
        {
            m_DraggingBar1 = true;
            m_DraggingBar2 = false;
        }
        else if (IsMouseOverRect(m_SoundBar2PointRect) || IsMouseOverRect(m_SoundBar2BgRect) || IsMouseOverRect(m_SoundBar2GaugeRect))
        {
            m_DraggingBar2 = true;
            m_DraggingBar1 = false;
        }
    }

    if (!leftDown)
    {
        m_DraggingBar1 = false;
        m_DraggingBar2 = false;
    }

    if (m_DraggingBar1 && m_SoundBar1BgRect)
    {
        float v = GetMouseBarValue(m_SoundBar1BgRect);
        m_SoundBar1Value = v;
        ApplyVolume(1, v);
        UpdateSoundBarVisual(1, v);
    }
    if (m_DraggingBar2 && m_SoundBar2BgRect)
    {
        float v = GetMouseBarValue(m_SoundBar2BgRect);
        m_SoundBar2Value = v;
        ApplyVolume(2, v);
        UpdateSoundBarVisual(2, v);
    }

    m_WasLeftDown = leftDown;
}

nlohmann::json SettingsUIController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void SettingsUIController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void SettingsUIController::ToggleRoot()
{
    ResolveRefs();
    SetOpen(!m_IsOpen);
}

void SettingsUIController::SetOpen(bool open)
{
    ResolveRefs();
    m_IsOpen = open;
    if (m_SoundPanel) m_SoundPanel->SetActive(open);
    if (m_CreditPanel) m_CreditPanel->SetActive(false);
    if (m_EscImageObj && !open) m_EscImageObj->SetActive(false);
    if (m_SoundButtonObj) m_SoundButtonObj->SetActive(open);
    if (m_CreditButtonObj) m_CreditButtonObj->SetActive(open);
    if (m_CloseButtonObj) m_CloseButtonObj->SetActive(open);
    if (m_ExitButtonObj) m_ExitButtonObj->SetActive(open);
    if (m_SoundBar1Bg) m_SoundBar1Bg->SetActive(open);
    if (m_SoundBar1Gauge) m_SoundBar1Gauge->SetActive(open);
    if (m_SoundBar1Point) m_SoundBar1Point->SetActive(open);
    if (m_SoundBar2Bg) m_SoundBar2Bg->SetActive(open);
    if (m_SoundBar2Gauge) m_SoundBar2Gauge->SetActive(open);
    if (m_SoundBar2Point) m_SoundBar2Point->SetActive(open);

    if (m_MenuRootObj)
    {
        m_MenuRootObj->SetActive(!open);
    }

    if (open)
    {
        ShowSoundTab();
        UpdateSoundBarVisuals();
    }
}

void SettingsUIController::ShowSoundTab()
{
    ResolveRefs();
    if (m_SoundPanel) m_SoundPanel->SetActive(true);
    if (m_CreditPanel) m_CreditPanel->SetActive(false);
    if (m_EscImageObj) m_EscImageObj->SetActive(true);
    if (m_SoundBar1Bg) m_SoundBar1Bg->SetActive(true);
    if (m_SoundBar1Gauge) m_SoundBar1Gauge->SetActive(true);
    if (m_SoundBar1Point) m_SoundBar1Point->SetActive(true);
    if (m_SoundBar2Bg) m_SoundBar2Bg->SetActive(true);
    if (m_SoundBar2Gauge) m_SoundBar2Gauge->SetActive(true);
    if (m_SoundBar2Point) m_SoundBar2Point->SetActive(true);
    if (m_ExitButtonObj) m_ExitButtonObj->SetActive(true);

    if (m_SoundButtonImage)
    {
        m_SoundButtonImage->ChangeData(soundPressedPath);
    }
    if (m_CreditButtonImage)
    {
        m_CreditButtonImage->ChangeData(creditNormalPath);
    }

    if (m_SoundButtonRect && m_CreditButtonRect && m_ButtonBaseCaptured)
    {
        const float scale = 1.5f;
        Vector2 soundSize = m_SoundButtonBaseSize * scale;
        Vector2 creditSize = m_CreditButtonBaseSize;
        Vector3 soundPos = m_SoundButtonBasePos - Vector3((scale - 1.0f) * m_SoundButtonBaseSize.x * 0.5f,
                                                          (scale - 1.0f) * m_SoundButtonBaseSize.y * 0.5f,
                                                          0.0f);
        Vector3 creditPos = m_CreditButtonBasePos;
        m_SoundButtonRect->SetSize(soundSize);
        m_SoundButtonRect->SetPos(soundPos);
        m_CreditButtonRect->SetSize(creditSize);
        m_CreditButtonRect->SetPos(creditPos);
    }

    UpdateSoundBarVisuals();
}

void SettingsUIController::ShowCreditTab()
{
    ResolveRefs();
    if (m_SoundPanel) m_SoundPanel->SetActive(false);
    if (m_CreditPanel) m_CreditPanel->SetActive(true);
    if (m_EscImageObj) m_EscImageObj->SetActive(false);
    if (m_SoundBar1Bg) m_SoundBar1Bg->SetActive(false);
    if (m_SoundBar1Gauge) m_SoundBar1Gauge->SetActive(false);
    if (m_SoundBar1Point) m_SoundBar1Point->SetActive(false);
    if (m_SoundBar2Bg) m_SoundBar2Bg->SetActive(false);
    if (m_SoundBar2Gauge) m_SoundBar2Gauge->SetActive(false);
    if (m_SoundBar2Point) m_SoundBar2Point->SetActive(false);
    if (m_ExitButtonObj) m_ExitButtonObj->SetActive(true);

    if (m_SoundButtonImage)
    {
        m_SoundButtonImage->ChangeData(soundNormalPath);
    }
    if (m_CreditButtonImage)
    {
        m_CreditButtonImage->ChangeData(creditPressedPath);
    }

    if (m_SoundButtonRect && m_CreditButtonRect && m_ButtonBaseCaptured)
    {
        const float scale = 1.5f;
        Vector2 soundSize = m_SoundButtonBaseSize;
        Vector2 creditSize = m_CreditButtonBaseSize * scale;
        Vector3 soundPos = m_SoundButtonBasePos;
        soundPos.y = 755.0f;
        Vector3 creditPos = m_CreditButtonBasePos - Vector3((scale - 1.0f) * m_CreditButtonBaseSize.x * 0.5f,
                                                            (scale - 1.0f) * m_CreditButtonBaseSize.y * 0.5f,
                                                            0.0f);
        m_SoundButtonRect->SetSize(soundSize);
        m_SoundButtonRect->SetPos(soundPos);
        m_CreditButtonRect->SetSize(creditSize);
        m_CreditButtonRect->SetPos(creditPos);
    }
}

void SettingsUIController::ResolveRefs()
{
    if (!m_Root)
    {
        m_Root = GetOwner();
    }

    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (!m_SoundPanel)
    {
        m_SoundPanel = scene->GetGameObjectByName(soundPanelObjectName);
    }
    if (!m_CreditPanel)
    {
        m_CreditPanel = scene->GetGameObjectByName(creditPanelObjectName);
    }
    if (!m_EscImageObj)
    {
        m_EscImageObj = scene->GetGameObjectByName(escImageObjectName);
    }
    if (!m_SoundButtonObj)
    {
        m_SoundButtonObj = scene->GetGameObjectByName(soundButtonObjectName);
        if (m_SoundButtonObj)
        {
            m_SoundButtonImage = m_SoundButtonObj->GetComponent<Image>();
            m_SoundButtonRect = m_SoundButtonObj->GetComponent<RectTransform>();
            if (m_SoundButtonImage)
            {
                m_SoundButtonImage->ChangeData(soundNormalPath);
                m_SoundButtonImage->OnPressed.AddListener(m_SoundButtonImage, [this]()
                {
                    m_SoundButtonImage->ChangeData(soundPressedPath);
                });
                m_SoundButtonImage->OnPressOut.AddListener(m_SoundButtonImage, [this]()
                {
                    ShowSoundTab();
                });
            }
        }
    }
    if (!m_CreditButtonObj)
    {
        m_CreditButtonObj = scene->GetGameObjectByName(creditButtonObjectName);
        if (m_CreditButtonObj)
        {
            m_CreditButtonImage = m_CreditButtonObj->GetComponent<Image>();
            m_CreditButtonRect = m_CreditButtonObj->GetComponent<RectTransform>();
            if (m_CreditButtonImage)
            {
                m_CreditButtonImage->ChangeData(creditNormalPath);
                m_CreditButtonImage->OnPressed.AddListener(m_CreditButtonImage, [this]()
                {
                    m_CreditButtonImage->ChangeData(creditPressedPath);
                });
                m_CreditButtonImage->OnPressOut.AddListener(m_CreditButtonImage, [this]()
                {
                    ShowCreditTab();
                });
            }
        }
    }

    if (!m_CloseButtonObj)
    {
        if (!closeButtonObjectName.empty())
        {
            m_CloseButtonObj = scene->GetGameObjectByName(closeButtonObjectName);
            if (m_CloseButtonObj)
            {
                m_CloseButtonImage = m_CloseButtonObj->GetComponent<Image>();
                if (m_CloseButtonImage)
                {
                    m_CloseButtonImage->ChangeData(closeNormalPath);
                    m_CloseButtonImage->OnPressed.AddListener(m_CloseButtonImage, [this]()
                    {
                        m_CloseButtonImage->ChangeData(closePressedPath);
                    });
                    m_CloseButtonImage->OnPressOut.AddListener(m_CloseButtonImage, [this]()
                    {
                        if (m_CloseButtonImage)
                        {
                            m_CloseButtonImage->ChangeData(closeNormalPath);
                        }
                        SetOpen(false);
                    });
                }
            }
        }
    }

    if (!m_ExitButtonObj)
    {
        if (!exitButtonObjectName.empty())
        {
            m_ExitButtonObj = scene->GetGameObjectByName(exitButtonObjectName);
            if (m_ExitButtonObj)
            {
                m_ExitButtonImage = m_ExitButtonObj->GetComponent<Image>();
                if (m_ExitButtonImage)
                {
                    m_ExitButtonImage->ChangeData(exitNormalPath);
                    m_ExitButtonImage->OnPressed.AddListener(m_ExitButtonImage, [this]()
                    {
                        m_ExitButtonImage->ChangeData(exitPressedPath);
                    });
                    m_ExitButtonImage->OnPressOut.AddListener(m_ExitButtonImage, [this]()
                    {
                        if (m_ExitButtonImage)
                        {
                            m_ExitButtonImage->ChangeData(exitNormalPath);
                        }
                    });
                }
            }
        }
    }

    if (!m_MenuRootObj)
    {
        m_MenuRootObj = scene->GetGameObjectByName(menuRootObjectName);
    }

    if (!m_ButtonBaseCaptured && m_SoundButtonRect && m_CreditButtonRect)
    {
        m_SoundButtonBasePos = m_SoundButtonRect->GetPos();
        m_CreditButtonBasePos = m_CreditButtonRect->GetPos();
        m_SoundButtonBaseSize = m_SoundButtonRect->GetSize();
        m_CreditButtonBaseSize = m_CreditButtonRect->GetSize();
        m_ButtonBaseCaptured = true;
    }

    if (!m_SoundBar1Bg) m_SoundBar1Bg = scene->GetGameObjectByName("UI_Settings_SoundBar1_BG");
    if (!m_SoundBar1Gauge) m_SoundBar1Gauge = scene->GetGameObjectByName("UI_Settings_SoundBar1_Gauge");
    if (!m_SoundBar1Point) m_SoundBar1Point = scene->GetGameObjectByName("UI_Settings_SoundBar1_Point");
    if (!m_SoundBar2Bg) m_SoundBar2Bg = scene->GetGameObjectByName("UI_Settings_SoundBar2_BG");
    if (!m_SoundBar2Gauge) m_SoundBar2Gauge = scene->GetGameObjectByName("UI_Settings_SoundBar2_Gauge");
    if (!m_SoundBar2Point) m_SoundBar2Point = scene->GetGameObjectByName("UI_Settings_SoundBar2_Point");

    if (!m_SoundBar1BgRect && m_SoundBar1Bg) m_SoundBar1BgRect = m_SoundBar1Bg->GetComponent<RectTransform>();
    if (!m_SoundBar1GaugeRect && m_SoundBar1Gauge) m_SoundBar1GaugeRect = m_SoundBar1Gauge->GetComponent<RectTransform>();
    if (!m_SoundBar1PointRect && m_SoundBar1Point) m_SoundBar1PointRect = m_SoundBar1Point->GetComponent<RectTransform>();
    if (!m_SoundBar2BgRect && m_SoundBar2Bg) m_SoundBar2BgRect = m_SoundBar2Bg->GetComponent<RectTransform>();
    if (!m_SoundBar2GaugeRect && m_SoundBar2Gauge) m_SoundBar2GaugeRect = m_SoundBar2Gauge->GetComponent<RectTransform>();
    if (!m_SoundBar2PointRect && m_SoundBar2Point) m_SoundBar2PointRect = m_SoundBar2Point->GetComponent<RectTransform>();
}

void SettingsUIController::CaptureSoundBarBase()
{
    if (m_SoundBarBaseCaptured)
    {
        return;
    }

    if (m_SoundBar1GaugeRect && m_SoundBar1PointRect)
    {
        m_SoundBar1GaugeBaseSize = m_SoundBar1GaugeRect->GetSize();
        m_SoundBar1GaugeBasePos = m_SoundBar1GaugeRect->GetPos();
        m_SoundBar1PointBasePos = m_SoundBar1PointRect->GetPos();
    }

    if (m_SoundBar2GaugeRect && m_SoundBar2PointRect)
    {
        m_SoundBar2GaugeBaseSize = m_SoundBar2GaugeRect->GetSize();
        m_SoundBar2GaugeBasePos = m_SoundBar2GaugeRect->GetPos();
        m_SoundBar2PointBasePos = m_SoundBar2PointRect->GetPos();
    }

    if (m_SoundBar1GaugeRect && m_SoundBar1PointRect && m_SoundBar2GaugeRect && m_SoundBar2PointRect)
    {
        m_SoundBarBaseCaptured = true;
    }
}

void SettingsUIController::UpdateSoundBarVisuals()
{
    if (!m_SoundBarBaseCaptured)
    {
        return;
    }

    auto& system = AudioManager::Instance().GetSystem();
    // Bar1: SFX (top), Bar2: BGM (bottom)
    m_SoundBar1Value = std::clamp(system.GetChannelGroupVolume("SFX"), 0.0f, 1.0f);
    m_SoundBar2Value = std::clamp(system.GetChannelGroupVolume("BGM"), 0.0f, 1.0f);

    UpdateSoundBarVisual(1, m_SoundBar1Value);
    UpdateSoundBarVisual(2, m_SoundBar2Value);
}

void SettingsUIController::UpdateSoundBarVisual(int barIndex, float value)
{
    value = std::clamp(value, 0.0f, 1.0f);

    RectTransform* gaugeRect = (barIndex == 1) ? m_SoundBar1GaugeRect : m_SoundBar2GaugeRect;
    RectTransform* pointRect = (barIndex == 1) ? m_SoundBar1PointRect : m_SoundBar2PointRect;
    Vector2 baseSize = (barIndex == 1) ? m_SoundBar1GaugeBaseSize : m_SoundBar2GaugeBaseSize;
    Vector3 basePos = (barIndex == 1) ? m_SoundBar1GaugeBasePos : m_SoundBar2GaugeBasePos;
    Vector3 pointBasePos = (barIndex == 1) ? m_SoundBar1PointBasePos : m_SoundBar2PointBasePos;

    if (!gaugeRect || !pointRect)
    {
        return;
    }

    float width = baseSize.x * value;
    Vector2 size = baseSize;
    size.x = width;
    gaugeRect->SetSize(size);

    const Vector2 pivot = gaugeRect->GetPivot();
    float left = basePos.x - pivot.x * baseSize.x;
    Vector3 gaugePos = basePos;
    gaugePos.x = left + pivot.x * width;
    gaugeRect->SetPos(gaugePos);

    Vector3 pointPos = pointBasePos;
    pointPos.x = left + width;
    pointRect->SetPos(pointPos);
}

float SettingsUIController::GetMouseBarValue(RectTransform* barRect) const
{
    if (!barRect)
    {
        return 1.0f;
    }

    auto ms = DirectX::Mouse::Get().GetState();
    Matrix invWorld = barRect->GetWorldMatrix().Invert();
    Vector3 mouseWorld((float)ms.x, (float)ms.y, 0.0f);
    Vector3 local = Vector3::Transform(mouseWorld, invWorld);
    return std::clamp(local.x, 0.0f, 1.0f);
}

bool SettingsUIController::IsMouseOverRect(RectTransform* rect) const
{
    if (!rect)
    {
        return false;
    }

    auto ms = DirectX::Mouse::Get().GetState();
    Matrix invWorld = rect->GetWorldMatrix().Invert();
    Vector3 mouseWorld((float)ms.x, (float)ms.y, 0.0f);
    Vector3 local = Vector3::Transform(mouseWorld, invWorld);
    return (local.x >= 0.0f && local.x <= 1.0f && local.y >= 0.0f && local.y <= 1.0f);
}

void SettingsUIController::ApplyVolume(int barIndex, float value)
{
    value = std::clamp(value, 0.0f, 1.0f);
    auto& system = AudioManager::Instance().GetSystem();

    if (barIndex == 1)
    {
        system.SetChannelGroupVolume("SFX", value);
    }
    else
    {
        system.SetChannelGroupVolume("BGM", value);
    }
}
