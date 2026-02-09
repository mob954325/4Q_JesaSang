#pragma once
#include "../Engine/Components/ScriptComponent.h"

class Image;

/*
    [ Button_PausePannelOff Script Component ]

    인게임 일시정지 패널 끄기 버튼
*/

class Button_PausePannelOff : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    Image* image = nullptr;

public:
    // component process
    void OnStart() override;

    // json
    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

public:
    // image path
    const std::string& GetPressImagePath() const;
    void SetPressImagePath(const std::string& path);

    std::string normalImagePath = "";    // 기본 이미지 path
    std::string pressedImagePath = "";   // 눌렸을때 이미지 path
};


