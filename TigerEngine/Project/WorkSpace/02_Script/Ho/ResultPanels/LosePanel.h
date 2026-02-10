#pragma once
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/UI/Image.h"

/// <summary>
/// 패배 시 활성화되는 오브젝트에 붙을 컴포넌트
/// </summary>
class LosePanel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    void Play(); // notified = true

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    bool notified = false;  // 외부 호출을 했는가? -> Manager에서 요청
    bool isPlayed = false;  // 한 번 플레이했는가? -> 중복 플레이 방지

    float timer = 0.0f;     // 이미지 시퀀스 시간
    float maxTimer = 0.5f;  // 이미지 시퀀스 딜레이 시간
    int index = 0;          // 이미지 시퀀스 인덱스

private:
    Image* cutImg = nullptr;
    std::string cutPath = "..\\Assets\\Resource\\CutScene\\intro_00.png";
    // particleUI -> // img가 있는 게임오브젝트를 복제 생성해서 여기저기 뿌린다. -> 오브젝트 제거 시에 모두 제거

    // 이미지 패스 모음
    std::vector<std::string> effectsPath{};
};