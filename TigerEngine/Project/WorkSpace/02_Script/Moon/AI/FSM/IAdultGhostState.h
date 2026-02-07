#pragma once
#include "../AdultGhostController.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Object/Component.h"

class IAdultGhostState
{
protected:
    AdultGhostController* adultGhost;

public:
    AdultGhostState type;

public:
    explicit IAdultGhostState(AdultGhostController* _adultGhost, AdultGhostState _type) : adultGhost(_adultGhost), type(_type) {}
    virtual ~IAdultGhostState() = default;

    virtual void Enter() = 0;
    virtual void ChangeStateLogic() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void FixedUpdate(float deltaTime) = 0;
    virtual void Exit() = 0;
};
