#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"
#include "Components/CharacterControllerComponent.h"

#include <directxtk/Keyboard.h>

class IPlayerState;
class InteractionZone;
class InteractionSensor;
class SearchObject;
class Inventory;

// Player State Enum
enum class PlayerState
{
    Idle, Walk, Run, Sit, SitWalk, Hide, Hit, Die, None
};

/*
     [ Player Controller Script Component ]

     1) FSM
     2) Input
     3) Movement
     4) TODO :: Interaction...
*/
class PlayerController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // --- [ Component ] ---------------------------
    Transform* transform = nullptr;
    FBXRenderer* fbxRenderer = nullptr;
    CharacterControllerComponent* cct = nullptr;
    Inventory* inventory = nullptr;

    Transform* camTransform = nullptr;

    // --- [ State ] ---------------------------
    PlayerState state = PlayerState::None;
    IPlayerState* curState;
    IPlayerState* fsmStates[8];

    // --- [ Stat ] --------------------------------
    // values (inspector)
    float walkSpeed = 2.5f;
    float runSpeed = 4.0f;
    float sitSpeed = 1.0f;
        

    // --- [ Controll ] ----------------------------
    // cur stat
    float curSpeed = 0.0f;
    Vector3 moveDir = Vector3::Zero;

    // interaction
    bool  isPossibleInteraction = false; // 기획자분이 한번에 하나만 가능한 사이즈라고 하심. 중첩된다면 추가 처리필요.
    SearchObject* curSerachObject;       // 현재 interaction가능한 오브젝트
    float interactionTime  = 3.0f;
    float interactionTimer = 0.0f;
    


    // --- [ Key ] ---------------------------------
    // key input flags
    bool isMoveLKey;
    bool isMoveRKey;
    bool isMoveFKey;
    bool isMoveBKey;
    bool isSitKey;
    bool isRunKey;
    bool isInteractionKey;

    // key buindings (inspector)
    Keyboard::Keys moveL_Key = Keyboard::Left;
    Keyboard::Keys moveR_Key = Keyboard::Right;
    Keyboard::Keys moveF_Key = Keyboard::Up;
    Keyboard::Keys moveB_Key = Keyboard::Down;

    Keyboard::Keys sit_Key = Keyboard::LeftControl;
    Keyboard::Keys run_Key = Keyboard::LeftShift;
    Keyboard::Keys interaction_Key = Keyboard::F;


public:
    // Component process
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnFixedUpdate(float delta) override;
    void OnDestory() override;

    // Collsion event
    void OnTriggerEnter(PhysicsComponent*) override;
    void OnTriggerStay(PhysicsComponent*) override;
    void OnTriggerExit(PhysicsComponent*) override;

    void OnCollisionEnter(PhysicsComponent*) override;
    void OnCollisionStay(PhysicsComponent*) override;
    void OnCollisionExit(PhysicsComponent*) override;

    // Json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // FSM
    void InitFSMStates();
    void ChangeState(PlayerState state);

    // Init
    void InitStat();

    // Input
    void InputProcess();

    // Movement
    void Move(float delta);
    void Rotation(float delta);

    // Interaction
    void InteractionCheak(float delta);
    void SerachObjectInteraction();

public:
    // 외부 Funcs.. TODO
    // Current Interaction Zone Search Object Set
    void SetCurSearchObject(SearchObject* object);






    //---------------------------------------------------------------
    // friend
    friend class IPlayerState;
    friend class Player_Idle;
    friend class Player_Walk;
    friend class Player_Run;
    friend class Player_Sit;
    friend class Player_SitWalk;
    friend class Player_Hide;
    friend class Player_Hit;
    friend class Player_Die;
    friend class InteractionZone;
    friend class InteractionSensor;
};

