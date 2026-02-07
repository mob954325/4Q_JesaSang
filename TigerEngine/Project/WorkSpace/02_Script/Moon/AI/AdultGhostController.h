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

public:
    template<typename T>
    T* GetState(AdultGhostState id)
    {
        auto* base = fsmStates[(int)id];
        if (!base) return nullptr;

#ifdef _DEBUG
        if (base->type != id)
        {
            std::cerr << "[FSM] State type mismatch\n";
            return nullptr;
        }
#endif

        return static_cast<T*>(base);
    }

private:
    // Component 
    AgentComponent* agent = nullptr;
    // GridComponent* grid = nullptr;
    VisionComponent* vision = nullptr;


    // State
    AdultGhostState state = AdultGhostState::None;
    IAdultGhostState* currentState;
    IAdultGhostState* fsmStates[5];

    // HideObject tracking
    GameObject* curSeeingHideObject = nullptr;
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


public:
    // friend
    friend class IAdultGhostState;
    friend class AdultGhost_Patrol;
    friend class AdultGhost_Chase;
    friend class AdultGhost_Search;
    friend class AdultGhost_Return;
    friend class AdultGhost_Attack;
};
