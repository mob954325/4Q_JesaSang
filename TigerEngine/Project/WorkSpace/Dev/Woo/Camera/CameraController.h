#pragma once
#include "Components/ScriptComponent.h"
#include <directxtk/SimpleMath.h>

class Transform;
class Camera;

/*
     [ Camera Controller Script Component ]
     
     dead zone 안에 있을때는 look focusing만, 
     dead zone 밖으로 나가면 position tracking (dead zone까지만)

     1) Target Tracking (deadzone + clamp follow)
     2) Target Look Focus
     3) 시점 변경 연출 (Quarter <-> Top)
*/

class CameraController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

public:
    enum class ViewMode
    {
        Quarter   // TODO
    };

    ViewMode currentMode = ViewMode::Quarter;

private:
    Transform* transform = nullptr;       
    Transform* targetTr = nullptr;    

    // offset
    Vector3 quarterOffset = { 0, 330.0f, -230.0f };
    Vector3 quarterEuler = { -100.0f, 0.0f, 0.0f };

    // pivot point (campos = pivot + modeOffset)
    Vector3 pivotPos = Vector3::Zero;

    // --- tunables ---
    float groundY = 0.0f;          // pivot이 붙어있는 평면 높이
    float deadRadius = 120.0f;     // 월드 단위 dead zone 반경
    float followLambda = 12.0f;    // 따라오는 속도


public:
    void OnInitialize() override;
    void OnStart() override;
    void OnLateUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    static float Clamp(float v, float a, float b) { return std::max(a, std::min(v, b)); }
    void UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt);
};
