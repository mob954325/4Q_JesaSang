#include "PlayerThreatMonitor.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"
#include "RenderPass/ParticleSource/Effect.h"
#include "../../Moon/AI/AdultGhostController.h"
#include "PlayerController.h"

REGISTER_COMPONENT(PlayerThreatMonitor)

RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerThreatMonitor>("PlayerThreatMonitor")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}


void PlayerThreatMonitor::OnStart()
{
    pc = GetOwner()->GetComponent<PlayerController>();

    // get effect
    auto smokeChild = GetOwner()->GetChildByName("Player_SmokeEffect");
    if (!smokeChild)
    {
        std::cout << "[PlayerThreatMonitor] Missing child: Player_SmokeEffect" << std::endl;
        return;
    }

    smokeEffect = smokeChild->GetOwner()->GetComponent<Effect>();
    if (!smokeEffect)
    {
        std::cout << "[PlayerThreatMonitor] Missing Effect component on Player_SmokeEffect" << std::endl;
        return;
    }

    // 적 찾기
    auto enemyObjects = SceneUtil::GetObjectsByName("Ghost_Adult");
    enemyControllers.clear();
    enemyControllers.reserve(enemyObjects.size());

    for (auto obj : enemyObjects)
    {
        if (!obj) continue;
        auto* controller = obj->GetComponent<AdultGhostController>();
        if (controller) enemyControllers.push_back(controller);
    }

    // init
    isSmokeActive = false;
    smokeEffect->Stop();
}

void PlayerThreatMonitor::OnUpdate(float delta)
{
    if (!smokeEffect) return;

    const bool shouldEnable = ShouldEnableThreatEffect();

    if (shouldEnable && !isSmokeActive)
    {
        StartSmokeEffect();
        isSmokeActive = true;
    }
    else if (!shouldEnable && isSmokeActive)
    {
        StopSmokeEffect();
        isSmokeActive = false;
    }
}

nlohmann::json PlayerThreatMonitor::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PlayerThreatMonitor::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}


bool PlayerThreatMonitor::ShouldEnableThreatEffect() const
{
    // 플레이어 Hit 상태면 무조건 OFF
    if (IsPlayerInHitState())
        return false;

    // 적이 하나라도 위협 상태면 ON
    return IsAnyEnemyThreatening();
}

bool PlayerThreatMonitor::IsAnyEnemyThreatening() const
{
    if (enemyControllers.empty())
        return false;

    for (auto* enemy : enemyControllers)
    {
        if (!enemy) continue;

        const AdultGhostState state = enemy->GetState();
        if (IsEnemyThreateningState(state))
            return true;
    }
    return false;
}

bool PlayerThreatMonitor::IsEnemyThreateningState(AdultGhostState state) const
{
    switch (state)
    {
    case AdultGhostState::Chase:
    case AdultGhostState::Search:
        return true;
    default:
        return false;
    }
}

void PlayerThreatMonitor::StartSmokeEffect()
{
    if (!smokeEffect) return;
    if (smokeEffect->emitters.empty()) return;

    smokeEffect->Stop();
    smokeEffect->emitters[0].looping = true;
    smokeEffect->Play();
}

void PlayerThreatMonitor::StopSmokeEffect()
{
    if (!smokeEffect) return;
    if (smokeEffect->emitters.empty()) return;

    smokeEffect->emitters[0].looping = false;       // 자연스럽게 소멸
}

bool PlayerThreatMonitor::IsPlayerInHitState() const
{
    return  (pc->GetPlayerState() == PlayerState::Hit);
}