#pragma once
#include "../../../Engine/Components/Light.h"
#include "../../../Engine/Components/Camera.h"
#include "../../../Engine/Components/ScriptComponent.h"

/// <summary>
/// 패배 연출용 스크립트 
/// 카메라를 찾아서 점점 뒤로 가는 연출을 실행하고 LosePanel에게 다음 연출을 요청한다.
/// </summary>
class LoseCinemachine : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnStart() override;
    void OnUpdate(float dt) override;

    void Play();

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    Camera* cam{}; // 카메라 이름은 LoseCinemachineCamera
    std::string camName = "LoseCinemachineCamera";
    Light* spotLight{};

    float cameraSpeed = 25.0f;
    float maxAccelspeed = 3.0f;

    float camTimer = 0.0f;
    float camMaxTime = 3.0f;

public:
    bool notified = false;
    bool isPlayed = false;
};
