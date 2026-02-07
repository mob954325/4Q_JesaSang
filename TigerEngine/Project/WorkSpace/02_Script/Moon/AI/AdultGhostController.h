#pragma once
#include "Components/ScriptComponent.h"
#include "Components/AgentComponent.h"
#include "Components/GridComponent.h"
#include "Components/VisionComponent.h"
#include "Util/CollisionLayer.h"


class IAdultGhostState;
class AdultGhost_Patrol;
class AdultGhost_Chase;
class AdultGhost_Search;
class AdultGhost_Return;
class AdultGhost_Attack;


enum class AdultGhostState
{
 // 순찰,   추격,   탐색,   복귀,   공격,    None
    Patrol, Chase, Search, Return, Attack, None
};


class AdultGhostController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // Component 
    AgentComponent* agent = nullptr;
    GridComponent* grid = nullptr;
    VisionComponent* vision = nullptr;

    GameObject* curSeeingHideObject = nullptr;

    // State
    AdultGhostState state = AdultGhostState::None;
    IAdultGhostState* curState;
    IAdultGhostState* fsmStates[5];

private:
    // FSM
    void InitFSMStates();
    void ChangeState(AdultGhostState state);

    // Animation
    void LoadAnimation();

    // Init
    // void InitStat();

    // Interaction
    void InteractionCheak(float delta);


public:
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnFixedUpdate(float dt) override;


public:
    // To. 우정 : 플레이어가 호출할 메소드
    // 이 함수 호출하면 AI는 Search 상태로 전환 되면서 -> 그 함수가 호출되었을 때의 플레이어 위치를 향해서 이동
    void OnPlayerNoise(const Vector3& noiseWorldPos);

    bool isSeeingHideObject = false;    // 시야 내에 HideObject가 감지 되는가? 
    bool hideLookRegistered = false;    // HideObject에 카운트 올렸는가?

public:
    // friend
    friend class IAdultGhostState;
    friend class AdultGhost_Patrol;
    friend class AdultGhost_Chase;
    friend class AdultGhost_Search;
    friend class AdultGhost_Return;
    friend class AdultGhost_Attack;
};
