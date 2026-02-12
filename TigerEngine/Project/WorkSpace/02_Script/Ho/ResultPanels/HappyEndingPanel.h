#pragma once
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/UI/Image.h"

/// <summary>
/// 승리 시 활성화 되는 오브젝트에 붙을 컴포넌트
/// </summary>
class HappyEndingPanel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    float timer = 0.0f;     // 이미지 시퀀스 시간
    float maxTimer = 0.25f;  // 이미지 시퀀스 딜레이 시간
    int index = 1;          // 이미지 시퀀스 인덱스
    bool waitingAnyKey = false;    // maxTime 이후 입력 대기 상태
    bool handled = false;          // 한 번만 처리
private:
    Image* image = nullptr;
    std::string cutPath = "..\\Assets\\Resource\\CutScene\\happy_ending_00.png";

    // 이미지 패스 모음
    std::vector<std::string> effectsPath{};
};
