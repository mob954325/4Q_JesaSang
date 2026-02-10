#pragma once
// #include "../TutorialController.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Object/Component.h"


class TutorialController;

class ITutorialStep
{
protected:
    TutorialController* tutorialController;

public:
    explicit ITutorialStep(TutorialController* _tutorialController) : tutorialController(_tutorialController) {}
    virtual ~ITutorialStep() = default;

    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual bool IsComplete() = 0;
    virtual void Exit() = 0;
};
