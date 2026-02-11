#pragma once
#include "Components/ScriptComponent.h"
#include <string>

/*

    [ QuestManager Component ] <Singleton>

    메인 게임 퀘스트 매니저입니다.
    사실상 툴팁같은 느낌이기도 함

    - 퀘스트 Step -
     1) [탐색] 제사준비 : 최조로 음식 재료 획득시 달성
     2) [조리] 정성을 담아 : 최초로 미니게임 성공시 달성
     3) [운반] 차려지는 상 : 최조로 제사상에 음식을 올렸을시 달성
     4) [완성] 제사상 완성 : 제사상에 올라온 음식 카운팅 (1/6)
*/

class QuestManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    inline static QuestManager* s_instance = nullptr;

    // 현재 퀘스트 index (1~4)
    int curStep = 0;

    // 연출 상태
    enum class AnimPhase
    {
        None,
        ShowSuccess,   // 체크/라인 ON 상태 유지
        Closing,       // 패널 닫히는 중(오른쪽으로)
        Opening        // 패널 열리는 중(왼쪽으로)
    };

    AnimPhase phase_ = AnimPhase::None;
    float phaseTimer_ = 0.0f;
    int pendingNextStep_ = 0;

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // Quset UI Update Setting
    void ApplyStepUI();     

    // 연출
    void StartStepTransition();           // 연출 시작
    void TickStepTransition(float dt);    // 연출 Update

public:
    // 외부 call funcs..
    static QuestManager* Instance() { return s_instance; }

    int GetCurStep() const { return curStep; }
    void StepComplete(int compleateIndex);  // compleateIndex번 퀘스트 달성 알림
};
