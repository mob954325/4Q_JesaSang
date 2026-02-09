#include "AdultGhost_Attack.h"

#include "EngineSystem/SceneSystem.h"
#include "../../../Woo/Player/PlayerController.h"


void AdultGhost_Attack::Enter()
{
    cout << "[AdultGhost_Attack] Enter Attack State" << endl;

    attackTimer = 0.0f;
    didDamage = false;

    // adultGhost->animController->ChangeState("Attack");

    // 이동 완전 정지
    adultGhost->agent->externalControl = true;
    adultGhost->agent->path.clear();
}

void AdultGhost_Attack::ChangeStateLogic()
{
    // 딜레이 이후 전환 로직 실행 
    if (attackTimer < attackDelay) return;

    auto* player = adultGhost->GetPlayer();

    if (player && adultGhost->IsSeeing(player))
    {
        cout << "[AdultGhost_Attack] Player still in sight -> Chase\n";
        adultGhost->ChangeState(AdultGhostState::Chase);
    }
    else
    {
        cout << "[AdultGhost_Attack] Lost player -> Search\n";

        auto grid = GridSystem::Instance().GetMainGrid();
        if (grid)
        {
            int px, py;
            auto wp = adultGhost->GetPlayer()->GetTransform()->GetLocalPosition();
            if (grid->WorldToGridFromCenter(wp, px, py))
            {
                adultGhost->lastPlayerGrid = { px, py, true };
            }
        }
        adultGhost->searchReason = SearchReason::FromAttack;
        adultGhost->ChangeState(AdultGhostState::Search);
    }
}

void AdultGhost_Attack::Update(float deltaTime)
{
    attackTimer += deltaTime;

    // 공격 애니메이션 끝나고 타격 입는 듯한 연출 위해서 딜레이 
    if (!didDamage && attackTimer >= 1.0f /*attackTimer < attackDelay*/)
    {
        auto* player = adultGhost->GetPlayer();
        if (player)
            player->GetComponent<PlayerController>()->TakeAttack(); // AI에게 공격 당했을 때 

        didDamage = true;
    }
}

void AdultGhost_Attack::FixedUpdate(float deltaTime)
{
}

void AdultGhost_Attack::Exit()
{
    attackTimer = 0.0f;
    adultGhost->agent->externalControl = false;
}