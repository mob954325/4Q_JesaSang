#include "TutorialController.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Step/ITutorialStep.h"
#include "Step/TutorialStep_Step1.h"


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

void TutorialController::OnUpdate(float dt)
{
    if (!currentStep) return;

    if (currentStep->IsComplete())
    {
        currentStep->Exit();
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
}

void TutorialController::ChangeStep(TutorialStep next)
{
    if (currentStep == fsmSteps[(int)next]) return;
    if (currentStep) currentStep->Exit();
    currentStep = fsmSteps[(int)next];
    state = next;
    if (currentStep) currentStep->Enter();
}