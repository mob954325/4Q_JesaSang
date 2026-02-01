#include "PlayerController.h"
#include "Util/DebugDraw.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include <Util/PhysXUtils.h>

#include "Object/GameObject.h"
#include "System/InputSystem.h"
#include "EngineSystem/PhysicsSystem.h"
#include "EngineSystem/CameraSystem.h"

#include "FSM/IPlayerState.h"
#include "FSM/Player_Idle.h"
#include "FSM/Player_Walk.h"
#include "FSM/Player_Run.h"
#include "FSM/Player_Sit.h"
#include "FSM/Player_SitWalk.h"
#include "FSM/Player_Hide.h"
#include "FSM/Player_Hit.h"
#include "FSM/Player_Die.h"

#include "../Object/SearchObject.h"
#include "../Inventory/Inventory.h"
#include "../Camera/CameraController.h"


REGISTER_COMPONENT(PlayerController)

RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerController>("PlayerController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

/*-------[ Component Process ]-------------------------------------*/
void PlayerController::OnInitialize()
{
    
}

void PlayerController::OnStart()
{
    // get components
    transform = GetOwner()->GetComponent<Transform>();
    fbxRenderer = GetOwner()->GetComponent<FBXRenderer>();
    cct = GetOwner()->GetComponent<CharacterControllerComponent>();
    inventory = GetOwner()->GetComponent<Inventory>();
    
    camController = CameraSystem::Instance().GetCurrCamera()->GetOwner()->GetComponent<CameraController>();

    // debug
    if (!fbxRenderer || !cct || !inventory || !camController)
    {
        cout << "[Player] Missing COmponet!" << endl;
    }

    // transform -> physics init
    //transform->SetPosition(Vector3::Zero);

    // init fsm
    InitFSMStates();
    ChangeState(PlayerState::Idle);

    // init stat
    InitStat();
}

void PlayerController::OnUpdate(float delta)
{
    // input
    InputProcess();
    
    // fsm
    if(curState)
    {
        curState->ChangeStateLogic();
        curState->Update(delta);
    }

    // interaction cheak
    InteractionCheak(delta);

    // view mode change test (Hide State)
    if (Input::GetKeyDown(interaction_Key))
    {
        camController->ToggleViewMode();
    }

    // Debug----
    //cout << "[Player] State : " << (int)state << endl;
    //cout << "[Player] Walk MoveDir : (" << moveDir.x << ", " << moveDir.y << ", " << moveDir.z << ")" << endl;
    //cout << "[Player] Current Speed : " << curSpeed << endl;

    //cout << "L:" << isMoveLKey << " R:" << isMoveRKey << " F:" << isMoveFKey << " B:" << isMoveBKey
    //    << " Sit:" << isSitKey << " Run:" << isRunKey << " Interact:" << isInteractionKey << endl;
}

void PlayerController::OnFixedUpdate(float delta)
{
    // fsm
    if (curState)
    {
        curState->FixedUpdate(delta);
    }

    // movement
    Move(delta);
    Rotation(delta);
}

void PlayerController::OnDestory()
{

}


/*-------[ Collision Event ]-------------------------------------*/
void PlayerController::OnTriggerEnter(PhysicsComponent*)
{
}

void PlayerController::OnTriggerStay(PhysicsComponent*)
{
}

void PlayerController::OnTriggerExit(PhysicsComponent*)
{
}

void PlayerController::OnCollisionEnter(PhysicsComponent*)
{
}

void PlayerController::OnCollisionStay(PhysicsComponent*)
{
}

void PlayerController::OnCollisionExit(PhysicsComponent*)
{
}


/*-------[ JSON ]-------------------------------------*/
nlohmann::json PlayerController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PlayerController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

/*----------------------------------------------------------------*/
/*-------[ FSM ]--------------------------------------------------*/
/*----------------------------------------------------------------*/
void PlayerController::InitFSMStates()
{
    fsmStates[(int)PlayerState::Idle] = new Player_Idle(this);
    fsmStates[(int)PlayerState::Walk] = new Player_Walk(this);
    fsmStates[(int)PlayerState::Run] = new Player_Run(this);
    fsmStates[(int)PlayerState::Sit] = new Player_Sit(this);
    fsmStates[(int)PlayerState::SitWalk] = new Player_SitWalk(this);
    fsmStates[(int)PlayerState::Hide] = new Player_Hide(this);
    fsmStates[(int)PlayerState::Hit] = new Player_Hit(this);
    fsmStates[(int)PlayerState::Die] = new Player_Die(this);
}

void PlayerController::ChangeState(PlayerState nextState)
{
    if(curState == fsmStates[(int)nextState])
        return;

    if(curState)
        curState->Exit();

    curState = fsmStates[(int)nextState];
    this->state = nextState;

    if(curState)
        curState->Enter();
}

/*-------[ Init ]-------------------------------------*/
void PlayerController::InitStat()
{

}

/*-------[ Input ]-------------------------------------*/
void PlayerController::InputProcess()
{
    // key
    isMoveLKey = Input::GetKey(moveL_Key);
    isMoveRKey = Input::GetKey(moveR_Key);
    isMoveFKey = Input::GetKey(moveF_Key);
    isMoveBKey = Input::GetKey(moveB_Key);
    isSitKey = Input::GetKey(sit_Key);
    isRunKey = Input::GetKey(run_Key);
    isInteractionKey = Input::GetKey(interaction_Key);

    // move dir
    Vector3 input(0, 0, 0);

    if (isMoveLKey) input.x -= 1;
    if (isMoveRKey) input.x += 1;
    if (isMoveFKey) input.z += 1;
    if (isMoveBKey) input.z -= 1;

    if (input.LengthSquared() > 0)
        input.Normalize();

    this->moveDir = input;
}

/*-------[ Movement ]----------------------------------*/
void PlayerController::Move(float delta)
{
    if (!cct) return;

    cct->m_MoveSpeed = curSpeed;
    cct->MoveCharacter(moveDir, delta);
}

static float WrapAngleRad(float a)      // util
{
    while (a > DirectX::XM_PI)  a -= DirectX::XM_2PI;
    while (a < -DirectX::XM_PI) a += DirectX::XM_2PI;
    return a;
}

void PlayerController::Rotation(float delta)
{
    if (moveDir.LengthSquared() <= 0.0001f)
        return;

    Vector3 rotationDir = -moveDir;         // 너 왜 반전이닝?

    float targetYaw = atan2f(rotationDir.x, rotationDir.z);
    float currentYaw = transform->GetYaw();
    float deltaYaw = WrapAngleRad(targetYaw - currentYaw);

    float turnSpeed = 10.0f;
    float newYaw = currentYaw + deltaYaw * turnSpeed * delta;

    transform->SetEuler(Vector3(0.0f, newYaw, 0.0f));
}

void PlayerController::InteractionCheak(float delta)
{
    // interaction x -> reset
    if (!isInteractionKey || !isPossibleInteraction || curSerachObject == nullptr)
    {
        interactionTimer = 0.0f;
        return;
    }

    // ingredient inventory full
    if (curSerachObject->itemType == ItemType::Ingredient && 
        inventory->IsFull())
    {
        return;
    }

    // holding
    interactionTimer += delta;
    float progress = interactionTimer / interactionTime;
    if (progress > 1.0f) progress = 1.0f;

    // completion -> interaction
    if (interactionTimer >= interactionTime)
    {
        SerachObjectInteraction();
        interactionTimer = 0.0f;
    }
}

void PlayerController::SerachObjectInteraction()
{
    // search object interaction
    std::unique_ptr<IItem> item = curSerachObject->Interaction();
    
    // item get or fail
    if (item)
        inventory->AddItem(std::move(item));
    else
        std::cout << "[Player] Fail Get Item... " << std::endl;

    // clear
    isPossibleInteraction = false;
    curSerachObject = nullptr;
}
    

void PlayerController::SetCurSearchObject(SearchObject* object)
{
    if (object)
    {
        isPossibleInteraction = true;
        curSerachObject = object;
    }
    else
    {
        isPossibleInteraction = false;
        curSerachObject = nullptr;
    }
}
