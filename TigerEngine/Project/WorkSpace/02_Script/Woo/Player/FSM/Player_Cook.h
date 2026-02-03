#pragma once
#include "IPlayerState.h"

// Player FSM - [Cook STATE]
class Player_Cook : public IPlayerState
{
public:
    Player_Cook(PlayerController* _player)
        : IPlayerState(_player) {
    }
    ~Player_Cook() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};
