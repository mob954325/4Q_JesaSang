#include "IntroCutSceneController.h"

#include <algorithm>
#include <filesystem>

#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "EngineSystem/SceneSystem.h"
#include "Manager/UIManager.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"
#include "Util/PathHelper.h"
#include "System/InputSystem.h"

REGISTER_COMPONENT(IntroCutSceneController);

RTTR_REGISTRATION
{
    rttr::registration::class_<IntroCutSceneController>("IntroCutSceneController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("ImageObjectName", &IntroCutSceneController::imageObjectName)
        .property("FrameObjectName", &IntroCutSceneController::frameObjectName)
        .property("NextScenePath", &IntroCutSceneController::nextScenePath)
        .property("AllowSkip", &IntroCutSceneController::allowSkip);
}

namespace
{
    struct FrameEntry
    {
        int major = 0;
        int minor = 0;
        std::string filename;
    };

    bool ParseFrameName(const std::string& filename, FrameEntry& out)
    {
        const std::string prefixEn = "intro_";
        const std::string prefixKr = "인트로_";
        std::string usedPrefix;

        if (filename.rfind(prefixEn, 0) == 0)
        {
            usedPrefix = prefixEn;
        }
        else if (filename.rfind(prefixKr, 0) == 0)
        {
            usedPrefix = prefixKr;
        }
        else
        {
            return false;
        }

        std::string stem = filename;
        const auto dotPos = stem.find_last_of('.');
        if (dotPos != std::string::npos)
        {
            stem = stem.substr(0, dotPos);
        }

        std::string numberPart = stem.substr(usedPrefix.size());
        int major = 0;
        int minor = 0;
        const auto splitPos = numberPart.find('.');
        try
        {
            if (splitPos == std::string::npos)
            {
                major = std::stoi(numberPart);
            }
            else
            {
                major = std::stoi(numberPart.substr(0, splitPos));
                minor = std::stoi(numberPart.substr(splitPos + 1));
            }
        }
        catch (...)
        {
            return false;
        }

        out.major = major;
        out.minor = minor;
        out.filename = filename;
        return true;
    }

    bool IsAdvancePressed(bool current, bool& prev)
    {
        const bool pressed = current && !prev;
        prev = current;
        return pressed;
    }
}

void IntroCutSceneController::OnInitialize()
{
}

void IntroCutSceneController::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (auto imageObj = scene->GetGameObjectByName(imageObjectName))
    {
        m_Image = imageObj->GetComponent<Image>();
        m_ImageRect = imageObj->GetComponent<RectTransform>();
    }

    if (auto frameObj = scene->GetGameObjectByName(frameObjectName))
    {
        m_FrameImage = frameObj->GetComponent<Image>();
        m_FrameRect = frameObj->GetComponent<RectTransform>();
    }

    LoadFrameList();
    ApplyLayout();
    if (!m_Frames.empty())
    {
        ApplyFrame(0);
    }
}

void IntroCutSceneController::OnUpdate(float delta)
{
    (void)delta;

    if (!m_LayoutApplied)
    {
        ApplyLayout();
    }

    const bool spaceDown = Input::GetKey(DirectX::Keyboard::Keys::Space);
    const bool enterDown = Input::GetKey(DirectX::Keyboard::Keys::Enter);
    const bool escDown = Input::GetKey(DirectX::Keyboard::Keys::Escape);

    const bool advance = IsAdvancePressed(spaceDown, m_PrevSpace)
        || IsAdvancePressed(enterDown, m_PrevEnter);

    if (advance)
    {
        const int nextIndex = m_CurrentIndex + 1;
        if (nextIndex < static_cast<int>(m_Frames.size()))
        {
            ApplyFrame(nextIndex);
        }
        else
        {
            LoadNextScene();
        }
    }

    if (allowSkip && IsAdvancePressed(escDown, m_PrevEsc))
    {
        LoadNextScene();
    }
}

nlohmann::json IntroCutSceneController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void IntroCutSceneController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void IntroCutSceneController::LoadFrameList()
{
    m_Frames.clear();

    auto dir = PathHelper::FindDirectory("Assets/Resource/CutScene");
    if (!dir)
    {
        return;
    }

    std::vector<FrameEntry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(*dir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const auto filename = entry.path().filename().string();
        if (filename == "타이틀.png"
            || filename == "컷씬-프레임.png"
            || filename == "title.png"
            || filename == "cutscene-frame.png")
        {
            continue;
        }

        FrameEntry frame;
        if (ParseFrameName(filename, frame))
        {
            entries.push_back(frame);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const FrameEntry& a, const FrameEntry& b)
    {
        if (a.major != b.major) return a.major < b.major;
        return a.minor < b.minor;
    });

    const std::string base = "..\\Assets\\Resource\\CutScene\\";
    for (const auto& entry : entries)
    {
        m_Frames.push_back(base + entry.filename);
    }
}

void IntroCutSceneController::ApplyFrame(int index)
{
    if (!m_Image || index < 0 || index >= static_cast<int>(m_Frames.size()))
    {
        return;
    }

    m_CurrentIndex = index;
    m_Image->ChangeData(m_Frames[index]);
}

void IntroCutSceneController::ApplyLayout()
{
    Vector2 screen = UIManager::Instance().GetSize();
    if (screen.x <= 0.0f || screen.y <= 0.0f)
    {
        return;
    }

    if (m_ImageRect)
    {
        m_ImageRect->SetPivot({ 0.0f, 0.0f });
        m_ImageRect->SetPos({ 0.0f, 0.0f, 0.0f });
        m_ImageRect->SetSize(screen);
    }

    if (m_FrameRect)
    {
        m_FrameRect->SetPivot({ 0.0f, 0.0f });
        m_FrameRect->SetPos({ 0.0f, 0.0f, 0.0f });
        m_FrameRect->SetSize(screen);
    }

    if (m_FrameImage)
    {
        m_FrameImage->ChangeData("..\\Assets\\Resource\\CutScene\\cutscene-frame.png");
    }

    m_LayoutApplied = true;
}

void IntroCutSceneController::LoadNextScene()
{
    if (nextScenePath.empty())
    {
        return;
    }

    if (auto scene = SceneSystem::Instance().GetCurrentScene())
    {
        scene->LoadToJson(nextScenePath);
    }
}
