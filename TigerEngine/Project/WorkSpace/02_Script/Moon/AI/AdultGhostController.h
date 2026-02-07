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

struct GridPos
{
    int x = -1;
    int y = -1;
    bool valid = false;
};

class AdultGhostController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // Component 
    AgentComponent* agent = nullptr;
    VisionComponent* vision = nullptr;


    // State
    AdultGhostState state = AdultGhostState::None;
    IAdultGhostState* currentState;
    IAdultGhostState* fsmStates[5];


    // HideObject tracking
    GameObject* curSeeingHideObject = nullptr;
    std::vector<GameObject*> hideObjects;
    bool hideLookRegistered = false;

private:
    // FSM
    void InitFSMStates();
    void ChangeState(AdultGhostState state);

    // Animation
    void LoadAnimation();

    // Movement (공통)
    bool MoveToTarget(float delta);

public:
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnFixedUpdate(float dt) override;
    void OnDestory() override;

    // Interaction
    void OnPlayerNoise(const Vector3& noiseWorldPos); // 플레이어에서 호출 

    // Helper
    void ResetAgentForMove(float speed);
    bool IsSeeing(GameObject* target) const;
    bool IsPlayerInSenseRange();

    GameObject* GetAITarget() const;
    GameObject* GetPlayer() const;


    // 플레이어 발견 마지막 위치 (그리드 좌표) 
    GridPos lastPlayerGrid;

public:
    // friend
    friend class IAdultGhostState;
    friend class AdultGhost_Patrol;
    friend class AdultGhost_Chase;
    friend class AdultGhost_Search;
    friend class AdultGhost_Return;
    friend class AdultGhost_Attack;
};
