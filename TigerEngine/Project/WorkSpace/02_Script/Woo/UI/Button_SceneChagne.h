#pragma once
#include "../Engine/Components/ScriptComponent.h"

class Image;

/*
    [ Button_SceneChagne Script Component ]

    씬 전환용 버튼 script component
    어차피 RTTR로 패스 저장하니까 하나 돌려쓰기
*/


class Button_SceneChagne : public ScriptComponent
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

    // change scene path
    std::string targetScenePath = "";    // 씬 전환 path
};