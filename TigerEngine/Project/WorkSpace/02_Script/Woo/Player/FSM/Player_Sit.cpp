#include "Player_Sit.h"

void Player_Sit::Enter()
{
    //cout << "[Player] Enter Sit State" << endl;

    // set speed
    player->curSpeed = 0;

    // set moved dir
    player->lookDir = Vector3::Zero;
}

void Player_Sit::ChangeStateLogic()
{
    // idle
    if (!player->isSitKey)
    {
        player->ChangeState(PlayerState::Idle);
        return;
    }

    const bool isMove =
        player->isMoveLKey || player->isMoveRKey ||
        player->isMoveFKey || player->isMoveBKey;

    // sit walk
    if (isMove)
    {
        player->ChangeState(PlayerState::SitWalk);
        return;
    }
}

void Player_Sit::Update(float deltaTime)
{
    // look dir
    Vector3 input(0, 0, 0);

    if (player->isMoveLKey) input.x -= 1;
    if (player->isMoveRKey) input.x += 1;
    if (player->isMoveFKey) input.z += 1;
    if (player->isMoveBKey) input.z -= 1;

    if (input.LengthSquared() > 0)
        input.Normalize();

    player->lookDir = input;
}

void Player_Sit::FixedUpdate(float deltaTime)
{
    
}

void Player_Sit::Exit()
{
    //cout << "[Player] Exit Sit State" << endl;
}
