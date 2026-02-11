#include "TutorialController.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "System/InputSystem.h"

#include "EngineSystem/SceneSystem.h"


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
    // -------------------------------------------------
    // 튜토리얼 세팅
    // -------------------------------------------------
    InitFSMSteps();

    Blink_Top = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Blink_Top"); // rectTransform 축 -700 
    Blink_Bottom = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Blink_Bottom"); // rectTransform 축 700 



    // -------------------------------------------------
    // 플레이어 세팅
    // -------------------------------------------------
    player_Obj = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player_Tutorial");
    Player_animController = player_Obj->GetComponent<AnimationController>();
    Player_fbxRenderer = player_Obj->GetComponent<FBXRenderer>();
    Player_fbxData = player_Obj->GetComponent<FBXData>();

    if (!Player_animController || !Player_fbxRenderer || !Player_fbxData)
    {
        std::cout << "[TutorialController] Player_Tutorial Component Missing" << std::endl;
        return;
    }

    LoadPlayerAnimation();




    // -------------------------------------------------
    // 튜토리얼 
    // -------------------------------------------------
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
// [ Tutorial FSM ]
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
// [ 플레이어 ]
// -----------------------------------------------------------

void TutorialController::LoadPlayerAnimation()
{
    // 애니메이션 파일 로드
    FBXResourceManager::Instance().LoadAnimationByPath(Player_fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\FuckingAssimp\\ani_idle_character.fbx", "Idle");
    FBXResourceManager::Instance().LoadAnimationByPath(Player_fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\FuckingAssimp\\ani_walk_character.fbx", "Walk");
    FBXResourceManager::Instance().LoadAnimationByPath(Player_fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\FuckingAssimp\\ani_run_character.fbx", "Run");
    FBXResourceManager::Instance().LoadAnimationByPath(Player_fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\FuckingAssimp\\ani_sit_character.fbx", "Sit");
    FBXResourceManager::Instance().LoadAnimationByPath(Player_fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\FuckingAssimp\\ani_cry_character.fbx", "Hit");

    // 클립 생성
    auto idleClip = Player_animController->FindClip("Idle");
    auto walkClip = Player_animController->FindClip("Walk");
    auto runClip = Player_animController->FindClip("Run");
    auto sitClip = Player_animController->FindClip("Sit");
    auto hitClip = Player_animController->FindClip("Hit");

    if (!idleClip || !walkClip || !runClip || !sitClip || !hitClip)
    {
        cout << "[Player Animation] Clip not found!\n" << endl;
        return;
    }

    // 상태 등록
    Player_animController->AddState(std::make_unique<AnimationState>("Idle", idleClip, Player_animController));
    Player_animController->AddState(std::make_unique<AnimationState>("Walk", walkClip, Player_animController));
    Player_animController->AddState(std::make_unique<AnimationState>("Run", runClip, Player_animController));
    Player_animController->AddState(std::make_unique<AnimationState>("Sit", sitClip, Player_animController));
    Player_animController->AddState(std::make_unique<AnimationState>("Hit", hitClip, Player_animController));

    // 시작 상태
    Player_animController->ChangeState("Idle");
}


// -----------------------------------------------------------
// [ Util ]
// -----------------------------------------------------------
