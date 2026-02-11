#pragma once
#include "Components/ScriptComponent.h"
#include "Components/AnimationController.h"
#include "Components/FBXRenderer.h"
#include "Components/FBXData.h"

class ITutorialStep;
class TutorialStep_Step1;
class TutorialStep_Step2;
class TutorialStep_Step3;
class TutorialStep_Step4;
class TutorialStep_Step5;
class TutorialStep_Step6;
class TutorialStep_Step7;
class TutorialStep_Step8;
class TutorialStep_Step9;


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

    // [ Player ]
    AnimationController* Player_animController = nullptr;
    FBXRenderer* Player_fbxRenderer = nullptr;
    FBXData* Player_fbxData = nullptr;

public:
    void OnStart() override;
    void OnUpdate(float dt) override;

    void InitFSMSteps();
    void ChangeStep(TutorialStep next);


    // [ Player ]
    GameObject* player_Obj = nullptr;
    void LoadPlayerAnimation();


public:
    void PlayAnim(const std::string& name, float blend = 0.2f)
    {
        if (!Player_animController) return;
        Player_animController->ChangeState(name, blend);
    }


public:
    // friend
    friend class ITutorialStep;
    friend class TutorialStep_Step1;
    friend class TutorialStep_Step2;
    friend class TutorialStep_Step3;
    friend class TutorialStep_Step4;
    friend class TutorialStep_Step5;
    friend class TutorialStep_Step6;
    friend class TutorialStep_Step7;
    friend class TutorialStep_Step8;
    friend class TutorialStep_Step9;

};
