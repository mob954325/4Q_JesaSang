#pragma once
#include "../BabyGhostController.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Object/Component.h"

class IBabyGhostState
{
protected:
    BabyGhostController* babyGhost;

public:
    BabyGhostState type;

public:
    explicit IBabyGhostState(BabyGhostController* _babyGhost, BabyGhostState _type) : babyGhost(_babyGhost), type(_type) {}
    virtual ~IBabyGhostState() = default;

    virtual void Enter() = 0;
    virtual void ChangeStateLogic() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void FixedUpdate(float deltaTime) = 0;
    virtual void Exit() = 0;
};
