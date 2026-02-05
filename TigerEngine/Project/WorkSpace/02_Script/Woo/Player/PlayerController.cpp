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
#include "FSM/Player_Cook.h"
#include "FSM/Player_Die.h"

#include "../Object/SearchObject.h"
#include "../Object/HideObject.h"
#include "../Inventory/Inventory.h"
#include "../Item/Item.h"
#include "../Camera/CameraController.h"
#include "../MiniGame/MiniGameManager.h"
#include "../JesaSang/JesaSangManager.h"
#include "../Altar/AltarManager.h"
#include "PlayerItemVisualizer.h"
#include "../Manager/GameManager.h"


REGISTER_COMPONENT(PlayerController)

RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerController>("PlayerController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

/*-------[ Component Process ]-------------------------------------*/
void PlayerController::OnStart()
{
    // get components
    transform = GetOwner()->GetComponent<Transform>();
    fbxRenderer = GetOwner()->GetComponent<FBXRenderer>();
    cct = GetOwner()->GetComponent<CharacterControllerComponent>();
    inventory = GetOwner()->GetComponent<Inventory>();
    visualizer = GetOwner()->GetComponent<PlayerItemVisualizer>();
    
    camController = CameraSystem::Instance().GetCurrCamera()->GetOwner()->GetComponent<CameraController>();

    // debug
    if (!fbxRenderer || !cct || !inventory || !camController)
    {
        cout << "[Player] Missing COmponet!" << endl;
    }

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

    // ----- test --------------
    // ai attack test
    if (Input::GetKeyDown(Keyboard::Q))
    {
        TakeAttack();
    }
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
    fsmStates[(int)PlayerState::Cook] = new Player_Cook(this);
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
    curLife = life;
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
}

/*-------[ Movement ]----------------------------------*/
void PlayerController::Move(float delta)
{
    if (!cct) return;

    // cct->m_MoveSpeed = curSpeed;
    cct->MovePlayer(lookDir, curSpeed, delta);
    // cct->MoveCharacter(lookDir, delta);
}

static float WrapAngleRad(float a)      // util
{
    while (a > DirectX::XM_PI)  a -= DirectX::XM_2PI;
    while (a < -DirectX::XM_PI) a += DirectX::XM_2PI;
    return a;
}

void PlayerController::Rotation(float delta)
{
    if (lookDir.LengthSquared() <= 0.0001f)
        return;

    Vector3 rotationDir = -lookDir;         // 너 왜 반전이닝?

    float targetYaw = atan2f(rotationDir.x, rotationDir.z);
    float currentYaw = transform->GetYaw();
    float deltaYaw = WrapAngleRad(targetYaw - currentYaw);

    float turnSpeed = 10.0f;
    float newYaw = currentYaw + deltaYaw * turnSpeed * delta;

    transform->SetEuler(Vector3(0.0f, newYaw, 0.0f));
}

/*-------[ Interaction ]----------------------------------*/
void PlayerController::InteractionCheak(float delta)
{
    SerachObjectInteraction(delta);     // 수색 오브젝트 수색 Interaction
    HideObjectInteraction(delta);       // 은신 오브젝트 은신 Interaction
    CookingInteraction(delta);          // 조리대 미니게임 시작 Interaction
    PutFoodJesaSangInteraction(delta);  // 제사상 음식 올리기 Interaction
    GetItemAltarInteraction(delta);     // 제단에서 아이템 가져오기 Interaction
}

void PlayerController::SerachObjectInteraction(float dt)
{
    // interaction x -> reset
    if (!isInteractionKey || !isPossibleSearch || curSerachObject == nullptr)
    {
        searchTimer = 0.0f;
        return;
    }

    // inventory full
    if (inventory->HasItem())
    {
        cout << "[Player] Inventory Full! Can't interaction" << endl;
        return;
    }

    // holding
    searchTimer += dt;
    float progress = searchTimer / searchTime;
    if (progress > 1.0f) progress = 1.0f;

    // search object interaction
    if (searchTimer >= searchTime)
    {
        // search object interaction
        std::unique_ptr<IItem> item = curSerachObject->Interaction();

        // item get or fail
        if (item)
        {
            visualizer->VisualOnItem(item->itemId);
            inventory->AddItem(std::move(item));
        }
        else
            std::cout << "[Player] Fail Get Item... " << std::endl;

        // clear
        isPossibleSearch = false;
        curSerachObject = nullptr;
        searchTimer = 0.0f;
    }
}

void PlayerController::HideObjectInteraction(float dt)
{
    // interaction x
    if (!isPossibleHide || !curHideObject)
        return;

    // hide
    if (Input::GetKeyDown(interaction_Key) && curHideObject->IsPossibleHide())
    {
        ChangeState(PlayerState::Hide);
        curHideObject->StartHide(this);
    }
}

void PlayerController::CookingInteraction(float dt)
{
    // interaction x -> reset
    if (!isInteractionKey || !isPossibleCooking || !inventory->HasItem())
    {
        cookInteractionTimer = 0.0f;
        return;
    }

    // ingredient
    if (inventory->GetCurItemType() != ItemType::Ingredient)
    {
        cookInteractionTimer = 0.0f;
        return;
    }

    // holding
    cookInteractionTimer += dt;
    float progress = cookInteractionTimer / cookInteractionTime;
    if (progress > 1.0f) progress = 1.0f;

    // cooking interaction
    if (cookInteractionTimer >= cookInteractionTime)
    {
        // mini game start
        visualizer->VisualOffItem();
        MiniGameManager::Instance()->StartMiniGame(std::move(inventory->TakeCurItem()));
        ChangeState(PlayerState::Cook);
        cookInteractionTimer = 0.0f;
    }
}

void PlayerController::PutFoodJesaSangInteraction(float dt)
{
    // interaction x -> reset
    if (!isInteractionKey || !isPossiblePutFood ||
        !inventory->HasItem() || inventory->GetCurItemType() != ItemType::Food)
    {
        putFoodTimer = 0.0f;
        return;
    }

    // holding
    putFoodTimer += dt;
    float progress = putFoodTimer / putFoodTime;
    if (progress > 1.0f) progress = 1.0f;

    // 제사상에 음식 올리기 interaction
    if (putFoodTimer >= putFoodTime)
    {
        std::unique_ptr<IItem> food = inventory->TakeCurItem();
        JesaSangManager::Instance()->ReceiveFood(std::move(food));
        visualizer->VisualOffItem();

        // clear
        putFoodTimer = 0.0f;
    }
}

void PlayerController::GetItemAltarInteraction(float dt)
{
    // interaction x -> reset
    if (!isInteractionKey || !isPossibleGetFood || !AltarManager::Instance()->HasItem())
    {
        getItemAltarTimer = 0.0f;
        return;
    }

    // inventory full
    if (inventory->HasItem())
    {
        cout << "[Player] Inventory Full! Can't interaction" << endl;
        return;
    }

    // holding
    getItemAltarTimer += dt;
    float progress = getItemAltarTimer / getItemAltarTime;
    if (progress > 1.0f) progress = 1.0f;

    // 제단 아이엠(재료/음식) 가져오기 interaction
    if (getItemAltarTimer >= getItemAltarTime)
    {
        std::unique_ptr<IItem> item = AltarManager::Instance()->GetItem();
        visualizer->VisualOnItem(item->itemId);
        inventory->AddItem(std::move(item));

        // clear
        getItemAltarTimer = 0.0f;
    }
}
    

PlayerState PlayerController::GetPlayerState()
{
    return state;
}

/*----------- 외부 호출 Funcs.. -------------------------*/
void PlayerController::SetCurSearchObject(SearchObject* object)
{
    // interaction zone에서 search object를 넘겨줌
    if (object)
    {
        isPossibleSearch = true;
        curSerachObject = object;
    }
    else
    {
        isPossibleSearch = false;
        curSerachObject = nullptr;
    }
}

void PlayerController::SetCurHideObject(HideObject* object)
{
    // interaction zone에서 hide object를 넘겨줌
    if (object)
    {
        isPossibleHide = true;
        curHideObject = object;
    }
    else
    {
        isPossibleHide = false;
        curHideObject = nullptr;
    }
}

void PlayerController::ReceiveMiniGameResult(unique_ptr<IItem> ingredient, bool isSuccess)
{
    if (isSuccess)
    {
        string ingreID = ingredient->itemId;

        // 성공시 음식 생성
        std::unique_ptr<IItem> food;
        if (ingreID == "Ingredient_Apple") food = make_unique<Food>("Apple");
        else if (ingreID == "Ingredient_Pear") food = make_unique<Food>("Pear");
        else if (ingreID == "Ingredient_Batter") food = make_unique<Food>("Batter");
        else if (ingreID == "Ingredient_Tofu") food = make_unique<Food>("Tofu");
        else if (ingreID == "Ingredient_Sanjeok") food = make_unique<Food>("Sanjeok");
        else if (ingreID == "Ingredient_Donggeurangttaeng") food = make_unique<Food>("Donggeurangttaeng");
        else cout << "NO!!!!!!!!!!!!!!!! ingredent id isanham." << endl;

        cout << "[Player] Cooking Success! Get Food : " << food->itemId << endl;

        // 재료 사용
        ingredient.reset();

        // 인벤토리에 완성된 음식 추가
        visualizer->VisualOnItem(food->itemId);
        inventory->AddItem(std::move(food));
    }
    else
    {
        cout << "[Player] Cooking Fail.. Get Ingredient : " << ingredient->itemId << endl;

        // 실패시 재료 다시 돌려받음
        visualizer->VisualOnItem(ingredient->itemId);
        inventory->AddItem(std::move(ingredient));

        // TODO :: 소음, AI 트리거 발생
    }
}

void PlayerController::ReceiveMiniGameItem(unique_ptr<IItem> ingredient)
{
    // ESC로 미니게임 강제종료시 재료반 다시 돌려받음
    visualizer->VisualOnItem(ingredient->itemId);
    inventory->AddItem(std::move(ingredient));
}

void PlayerController::TakeAttack()
{
    // 이미 죽은상태 return
    if (state == PlayerState::Die) return;

    // 무적상태 return
    if (isPlayerInvincible)
    {
        cout << "[Player] TakeAttack Fail! isPlayerInvincible" << endl;
        return;
    }
    
    cout << "[Player] Take Damage! " << endl;

    // 아이템이 있다면 제단에 올라감
    if (inventory->HasItem())
    {
        std::unique_ptr<IItem> item = inventory->TakeCurItem();
        visualizer->VisualOffItem();
        AltarManager::Instance()->ReceiveItem(std::move(item));
        cout << "[Player] Drop Item... " << endl;
    }

    // Die
    curLife--;
    
    if (curLife <= 0)
    {
        curLife = 0;
        ChangeState(PlayerState::Die);
        GameManager::Instance()->GameOver();

        return;
    }

    // Hit (패닉)
    if(state != PlayerState::Hit)
        ChangeState(PlayerState::Hit);
    else
    {
        // 이미 패닉상태였을경우 재시작 (무적상태는 위에서 return)
        ChangeState(PlayerState::Idle);
        ChangeState(PlayerState::Hit);
    }
}

float PlayerController::GetCurSenseRadiuse() const
{
    return curSenseRadius;
}
