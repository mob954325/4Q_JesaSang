#pragma once
#include "Components/ScriptComponent.h"
#include <iostream>
#include <string>

using namespace std;

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

private:
    void ApplyStepUI();     // 현재 퀘스트 step에 맞는 UI 업데이트

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    static QuestManager* Instance() { return s_instance; }

    int GetCurStep() const { return curStep; }
    void StepComplete(int compleateIndex);
};
