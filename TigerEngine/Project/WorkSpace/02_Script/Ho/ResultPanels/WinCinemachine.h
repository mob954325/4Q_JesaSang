#pragma once
#include "../../../Engine/Components/Light.h"
#include "../../../Engine/Components/Camera.h"
#include "../../../Engine/Components/ScriptComponent.h"

/// <summary>
/// 승리 연출용 스크립트 
/// 자식으로 카메라랑 스폿라이트를 가지고 연출한다.
/// </summary>
class WinCinemachine : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnStart() override;
    void OnUpdate(float dt) override;

    void Play();

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    Camera* cam{}; // 카메라 이름은 WinCinemachineCamera
    std::string camName = "WinCinemachineCamera";
    Light* spotLight{};

    float cameraSpeed = 20.0f;

    float camTimer = 0.0f;
    float camMaxTime = 4.0f;

    float spotTimer = 0.0f;
    float spotMaxTime = 1.3f; // 뜸들이는 시간

public:
    bool notified = false;
    bool isPlayed = false;
};
