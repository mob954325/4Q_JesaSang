#pragma once
#include "Components/ScriptComponent.h"


class ITutorialStep;
class TutorialStep_Step1;

enum class TutorialStep
{
    Step1, Step2, Step3, Step4, Step5,
    Step6, Step7, Step8, Step9, None
};


class TutorialController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // Step 
    TutorialStep state = TutorialStep::None;
    ITutorialStep* currentStep = nullptr;
    ITutorialStep* fsmSteps[9];


public:
    void OnStart() override;
    void OnUpdate(float dt) override;

    void InitFSMSteps();
    void ChangeStep(TutorialStep next);



public:
    // friend
    friend class ITutorialStep;
    friend class TutorialStep_Step1;
};
