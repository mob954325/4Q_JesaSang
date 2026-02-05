#include "Player_Hide.h"
#include "../../Player/PlayerItemVisualizer.h"
#include "System/InputSystem.h"
#include "../../Camera/CameraController.h"
#include "../../Object/HideObject.h"
#include "../../Inventory/Inventory.h"

void Player_Hide::Enter()
{
    // player render off
    player->fbxRenderer->SetActive(false);
    player->visualizer->VisualRenderOff();

    // camera view mode change
    player->camController->SetTargetTransform(player->curHideObject->GetOwner()->GetTransform());
    player->camController->SetViewMode(CameraController::ViewMode::Top);

    // set speed
    player->curSpeed = 0;

    // set sense radius
    player->curSenseRadius = 0;

    //cout << "[Player] Enter Hide State" << endl;
}

void Player_Hide::ChangeStateLogic()
{
    if(Input::GetKeyDown(player->interaction_Key))
    {
        player->curHideObject->StopHide();      // 중단할때만 종료 로직 추가
        player->ChangeState(PlayerState::Idle);
    }
}

void Player_Hide::Update(float deltaTime)
{

}

void Player_Hide::FixedUpdate(float deltaTime)
{

}

void Player_Hide::Exit()
{
    // player render on
    player->fbxRenderer->SetActive(true);
    player->visualizer->VisualRenderOn();

    // camera view mode change
    player->camController->SetTargetTransform(player->transform);
    player->camController->SetViewMode(CameraController::ViewMode::Quarter);

    //cout << "[Player] Exit Hide State" << endl;
}
