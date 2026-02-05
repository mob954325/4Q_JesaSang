#include "Player_Cook.h"
#include "../../Inventory/Inventory.h"

void Player_Cook::Enter()
{
    cout << "[Player] Enter Cook State" << endl;

    // set speed
    player->curSpeed = 0;

    // set sense radius
    player->curSenseRadius = 0;
}

void Player_Cook::ChangeStateLogic()
{

}

void Player_Cook::Update(float deltaTime)
{

}

void Player_Cook::FixedUpdate(float deltaTime)
{

}

void Player_Cook::Exit()
{
    cout << "[Player] Exit Cook State" << endl;
}
