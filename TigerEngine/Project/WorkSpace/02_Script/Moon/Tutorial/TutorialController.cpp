#include "TutorialController.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "System/InputSystem.h"

#include "Step/ITutorialStep.h"
#include "Step/TutorialStep_Step1.h"
#include "Step/TutorialStep_Step2.h"
#include "Step/TutorialStep_Step3.h"
#include "Step/TutorialStep_Step4.h"
#include "Step/TutorialStep_Step5.h"
#include "Step/TutorialStep_Step6.h"
#include "Step/TutorialStep_Step7.h"
#include "Step/TutorialStep_Step8.h"
#include "Step/TutorialStep_Step9.h"


REGISTER_COMPONENT(TutorialController)

RTTR_REGISTRATION
{
    rttr::registration::class_<TutorialController>("TutorialController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

nlohmann::json TutorialController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void TutorialController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}


// --------------------------------------------------------


void TutorialController::OnStart() 
{
    InitFSMSteps();

    ChangeStep(TutorialStep::Step1);
}


// IsComplete() 가 true가 되면 -> 자동으로 Step 전환 
void TutorialController::OnUpdate(float dt)
{
    if (!currentStep) return;

    if (currentStep->IsComplete())
    {
        ChangeStep((TutorialStep)((int)state + 1));
        return;
    }

    currentStep->Update(dt);
}


// -----------------------------------------------------------
// [ FSM ]
// -----------------------------------------------------------

void TutorialController::InitFSMSteps()
{
    fsmSteps[(int)TutorialStep::Step1] = new TutorialStep_Step1(this);
    fsmSteps[(int)TutorialStep::Step2] = new TutorialStep_Step2(this);
    fsmSteps[(int)TutorialStep::Step3] = new TutorialStep_Step3(this);
    fsmSteps[(int)TutorialStep::Step4] = new TutorialStep_Step4(this);
    fsmSteps[(int)TutorialStep::Step5] = new TutorialStep_Step5(this);
    fsmSteps[(int)TutorialStep::Step6] = new TutorialStep_Step6(this);
    fsmSteps[(int)TutorialStep::Step7] = new TutorialStep_Step7(this);
    fsmSteps[(int)TutorialStep::Step8] = new TutorialStep_Step8(this);
    fsmSteps[(int)TutorialStep::Step9] = new TutorialStep_Step9(this);
}

void TutorialController::ChangeStep(TutorialStep next)
{
    // None은 튜토리얼 종료
    if (next == TutorialStep::None)
    {
        if (currentStep)
        {
            currentStep->Exit();
            currentStep = nullptr;
        }

        state = TutorialStep::None;
        std::cout << "=== Tutorial FINISH ===" << std::endl;
        return;
    }

    if (currentStep == fsmSteps[(int)next]) return;

    if (currentStep) currentStep->Exit();
    currentStep = fsmSteps[(int)next];
    state = next;

    if (currentStep) currentStep->Enter();
}


// -----------------------------------------------------------
// [ Util ]
// -----------------------------------------------------------
