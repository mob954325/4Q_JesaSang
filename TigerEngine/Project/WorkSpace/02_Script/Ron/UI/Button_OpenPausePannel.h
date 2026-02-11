#pragma once
#include "Components/ScriptComponent.h"

class Image;

class Button_OpenPausePannel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    Button_OpenPausePannel() { SetName("Button_OpenPausePannel"); }

    void OnStart() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    const std::string& GetPressImagePath() const;
    void SetPressImagePath(const std::string& path);

    std::string normalImagePath = "";
    std::string pressedImagePath = "";

private:
    Image* image{};
};
