#pragma once
#include "IPlayerState.h"

// Player FSM - [HIT STATE]
class Player_Hit : public IPlayerState
{
private:
    float hitTimer = 0.0f;
    float invincibleTimer = 0.0f;

    // 플레이어 깜빡거림
    float renderDirectorTime = 0.2f;
    float renderDirectorTimer = 0.0f;

public:
    Player_Hit(PlayerController* _player)
        : IPlayerState(_player) {
    }
    ~Player_Hit() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};


