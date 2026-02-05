#include "Player_Hit.h"

void Player_Hit::Enter()
{
    cout << "[Player] Enter Hit State (life : " << player->curLife << ")" << endl;

    // set speed
    player->curSpeed = player->walkSpeed * player->hitSpeedUpRate;

    // timer init
    timer = 0.0f;
}

void Player_Hit::ChangeStateLogic()
{
    // hit duration 후 자동 change
    if (timer >= player->hitDuration)
    {
        const bool isMove =
            player->isMoveLKey || player->isMoveRKey ||
            player->isMoveFKey || player->isMoveBKey;

        // sit, sit walk
        if (player->isSitKey)
        {
            player->ChangeState(isMove ? PlayerState::SitWalk : PlayerState::Sit);
            return;
        }

        // idle
        if (!isMove)
        {
            player->ChangeState(PlayerState::Idle);
            return;
        }

        // run
        if (player->isRunKey)
        {
            player->ChangeState(PlayerState::Run);
            return;
        }
    }
}

void Player_Hit::Update(float deltaTime)
{
    // timer
    timer += deltaTime;

    // look dir
    Vector3 input(0, 0, 0);

    if (player->isMoveLKey) input.x -= 1;
    if (player->isMoveRKey) input.x += 1;
    if (player->isMoveFKey) input.z += 1;
    if (player->isMoveBKey) input.z -= 1;

    if (input.LengthSquared() > 0)
        input.Normalize();

    player->lookDir = -input;       // 이동 반대 (패닉)
}

void Player_Hit::FixedUpdate(float deltaTime)
{

}

void Player_Hit::Exit()
{
    cout << "[Player] Exit Hit State" << endl;
}
