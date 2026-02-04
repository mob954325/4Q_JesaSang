#include "AgentComponent.h"
#include "../Components/Transform.h"
#include "../Components/GridComponent.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"
#include "../EngineSystem/AgentSystem.h"
#include <random>

RTTR_REGISTRATION
{
    rttr::registration::class_<AgentComponent>("AgentComponent")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)

        .property("reachDist", &AgentComponent::reachDist)
        .property("patrolSpeed", &AgentComponent::patrolSpeed);
}

nlohmann::json AgentComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AgentComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

AgentComponent::~AgentComponent()
{
    
}


void AgentComponent::OnInitialize()
{
    
}

void AgentComponent::OnStart()
{
    cct = GetOwner()->GetComponent<CharacterControllerComponent>();

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    auto tr = GetOwner()->GetTransform();
    grid->WorldToGridFromCenter(tr->GetWorldPosition(), cx, cy);
}

// 무작위 목표(Grid 좌표)를 선택하는 함수
void AgentComponent::PickRandomTarget()
{
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;
      
    int range = 10; // 현재 위치(cx, cy)를 기준으로 목표를 선택할 범위
     
    hasTarget = false;

    static std::random_device rd; 
    static std::mt19937 gen(rd()); // 메르센 트위스터 난수 생성기
    std::uniform_int_distribution<> dis(-range, range); // -range ~ +range 범위의 균등 정수 분포

    for (int i = 0; i < 20; ++i) // 최대 20번 
    {
        // 현재 좌표(cx, cy) 기준으로 랜덤한 좌표(rx, ry) 생성
        int rx = cx + dis(gen);
        int ry = cy + dis(gen);

        // 생성된 좌표가 이동 가능한지 체크
        if (grid->IsWalkableFromCenter(rx, ry))
        {
            targetCX = rx; // 목표 X 좌표 설정
            targetCY = ry; // 목표 Y 좌표 설정
            hasTarget = true;
            return;
        }
    }

    std::cout << "[AgentComponent] No valid target found\n";
}


void AgentComponent::OnFixedUpdate(float dt)
{
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid || !hasTarget) return;

    // 경로 없으면 새 경로 생성
    if (path.empty())
    {
        path = grid->FindPath(cx, cy, targetCX, targetCY);
    }

    if (path.empty()) return; // 경로가 없으면 이동 안함

    auto next = path.front();
    Vector3 targetPos = grid->GridToWorldFromCenter(next.first, next.second);

    Vector3 pos = GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;
    dir.y = 0;

    if (dir.Length() < reachDist)
    {
        cx = next.first;
        cy = next.second;
        path.erase(path.begin());
    }
    else
    {
        dir.Normalize();
        MoveAgent(dir, patrolSpeed, dt);
    }

    // 디버그 출력
    std::cout << "[DEBUG] Current: (" << cx << "," << cy << ") "
        << "Next: (" << next.first << "," << next.second << ") "
        << "Path size: " << path.size() << std::endl;
}

void AgentComponent::MoveAgent(const Vector3& dir, float speed, float dt)
{
    if (cct)
        cct->MoveAI(dir, speed, dt);
}

void AgentComponent::OnFixedUpdate(float dt)
{

}

void AgentComponent::Enable_Inner()
{
    AgentSystem::Instance().Register(this);
}

void AgentComponent::Disable_Inner()
{
    AgentSystem::Instance().UnRegister(this);
}
