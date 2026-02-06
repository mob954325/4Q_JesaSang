#include "AdultGhost_Chase.h"
#include "EngineSystem/SceneSystem.h"


// 3초 동안은 무조건 플레이어 위치를 향해서 이동하고, (그리드에서 걸을 수 있는 영역만을 통해서 최단거리 이동)
// 3초 이후에, 여전히 플레이어가 시야 범위 내에 있으면 여전히 플레이어 향해서 이동하지만, 
// 3초 이후에, 플레이어가 범위 내에 없으면 탐색 Search 상태로 넘어간다. 

void AdultGhost_Chase::Enter()
{
    cout << "[AdultGhost_Chase] Enter Chase State" << endl;

    agent = adultGhost->agent;

    aiTarget = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("AITarget");
    if (aiTarget)
    {
        auto wp = aiTarget->GetTransform()->GetLocalPosition();
        // cout << "[Chase Enter] AITarget Local = (" << wp.x << ", " << wp.y << ", " << wp.z << ")" << endl;
    }

    chaseTimer = 0.0f;
    repathTimer = 0.0f;

    // 추격 속도 세팅 
    agent->patrolSpeed = 7.0f;

    // Agent 초기화 
    agent->externalControl = true;  // 자율 AI 봉인
    agent->isWaiting = false;
    agent->waitTimer = 0.0f;
    agent->hasTarget = true;        // 항상 true 유지
    agent->path.clear();
}

void AdultGhost_Chase::ChangeStateLogic()
{
    if (!aiTarget) return;

    // 3초 이후에만 상태 전환 가능 
    if (chaseTimer >= forceChaseTime)
    {
        if (!adultGhost->vision->CheckVision(aiTarget, 90, 400))
        {
            cout << "[AdultGhost_Chase] ChangeState -> AdultGhostState::Search" << endl;
            adultGhost->ChangeState(AdultGhostState::Search);
        }
    }
}

void AdultGhost_Chase::Update(float deltaTime)
{
    chaseTimer += deltaTime;

    if (!aiTarget) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;


    // [ 플레이어 위치 -> Grid ]
    int px, py;
    auto pwp = aiTarget->GetTransform()->GetLocalPosition();
    grid->WorldToGridFromCenter(pwp, px, py);
    cout << "[Chase Repath] Player Grid = (" << px << "," << py << ")\n";

    // [ AI 위치 -> Grid ]
    auto myPos = adultGhost->GetOwner()->GetTransform()->GetWorldPosition();
    int cx, cy;
    grid->WorldToGridFromCenter(myPos, cx, cy);
    cout << "[Agent] Self Grid = (" << cx << "," << cy << ")\n";


    // [ 어느정도 이동했을 때 경로 갱신 ]
    int dist = abs(px - agent->targetCX) + abs(py - agent->targetCY);

    const int repathThreshold = 2; // 2칸 이상 차이날 때만
    if (dist >= repathThreshold)
    {
        agent->targetCX = px;
        agent->targetCY = py;

        // 진행중인 경로가 거의 끝났을 때만 리셋
        if (agent->path.size() <= 2)
            agent->path.clear();

        agent->hasTarget = true;

        cout << "[Chase] ReTarget -> (" << px << "," << py << ")\n";
    }
}

void AdultGhost_Chase::FixedUpdate(float deltaTime)
{
    if (!agent || !agent->hasTarget) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    // 경로 없으면 생성
    if (agent->path.empty())
    {
        agent->path = grid->FindPath(agent->cx, agent->cy, agent->targetCX, agent->targetCY);
        if (agent->path.empty()) return;
    }

    // 다음 칸 이동 
    auto next = agent->path.front();
    Vector3 targetPos = grid->GridToWorldFromCenter(next.first, next.second);

    Vector3 pos = agent->GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;
    dir.y = 0;

    // 해당 칸 도착 
    if (dir.Length() < agent->reachDist)
    {
        agent->cx = next.first;
        agent->cy = next.second;
        agent->path.erase(agent->path.begin());
        return; // 이 칸은 끝까지 가도록 
    }

    dir.Normalize();
    agent->MoveAgent(dir, agent->patrolSpeed, deltaTime);
}

void AdultGhost_Chase::Exit()
{
    agent->hasTarget = false;
    agent->isWaiting = false;
    agent->waitTimer = 0.f;
    agent->path.clear();
    agent->externalControl = false;

    cout << "[AdultGhost_Chase] Exit" << endl;
}