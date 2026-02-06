#pragma once
#include "../AdultGhostController.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Object/Component.h"

class IAdultGhostState
{
protected:
    AdultGhostController* adultGhost;

public:
    IAdultGhostState(AdultGhostController* _adultGhost) : adultGhost(_adultGhost) {}
    virtual ~IAdultGhostState() = default;

    virtual void Enter() = 0;
    virtual void ChangeStateLogic() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void FixedUpdate(float deltaTime) = 0;
    virtual void Exit() = 0;
};
