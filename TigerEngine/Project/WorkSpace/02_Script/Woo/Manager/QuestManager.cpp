#include "QuestManager.h"
#include <iostream>

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"
#include "../UI/MainGameUIManager.h"
#include "../JesaSang/JesaSangManager.h"


REGISTER_COMPONENT(QuestManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<QuestManager>("QuestManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}


namespace
{
    constexpr int kMinStep = 1;
    constexpr int kMaxStep = 4;
    constexpr int kStep4FoodTotal = 6;

    constexpr float kSuccessHoldSec = 2.0f;  // 체크/라인 켠 후 유지 시간
    constexpr float kCloseSec = 1.0f;        // 오른쪽으로 밀리는 시간(닫힘)
    constexpr float kOpenSec = 1.0f;         // 왼쪽으로 밀리는 시간(열림)

    int ClampStep(int s)
    {
        if (s < kMinStep) return kMinStep;
        if (s > kMaxStep) return kMaxStep;
        return s;
    }
}



void QuestManager::OnInitialize()
{
    s_instance = this;
}

void QuestManager::OnStart()
{
    // step 1 퀘스트 시작
    curStep = 1;
    ApplyStepUI();
}

void QuestManager::OnUpdate(float delta)
{
    TickStepTransition(delta);

    // UI 매니저의 패널 슬라이드(보간) 매 프레임 갱신
    if (auto ui = MainGameUIManager::Instance())
        ui->TickQuestPanel(delta);
}

void QuestManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
}

nlohmann::json QuestManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void QuestManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void QuestManager::ApplyStepUI()
{
    auto ui = MainGameUIManager::Instance();
    if (!ui)
    {
        cout << "[QuestManager] MainGameUIManager is null!" << endl;
        return;
    }

    // curStep 범위 보정
    curStep = ClampStep(curStep);

    // 현재 퀘스트 step에 맞는 UI 업데이트
    switch (curStep)
    {
    case 1:
        ui->UpdateQuestTitle(L"[탐색] 제사 준비");
        ui->UpdateQuestLable(L"음식 재료를 획득하세요.");
        break;
    
    case 2:
        ui->UpdateQuestTitle(L"[조리] 정성을 담아");
        ui->UpdateQuestLable(L"미니게임을 성공하세요.");
        break;
    
    case 3:
        ui->UpdateQuestTitle(L"[운반] 차려지는 상");
        ui->UpdateQuestLable(L"제사상에 음식을 올리세요.");
        break;
    
    case 4:
    {
        int step4FoodCount = JesaSangManager::Instance()->GetHasFoodCount();

        std::wstring lable =
            L"제사상에 모든 음식을 올리세요. (" +
            std::to_wstring(step4FoodCount) +
            L"/6)";

        ui->UpdateQuestTitle(L"[완성] 제사상 차림");
        ui->UpdateQuestLable(lable.c_str());
        break;
    }
    
    default:
        break;
    }

     // UI
    ui->SetQuestTitleOn(true);
    ui->SetQuestLableOn(true);
    ui->SetQuestCheakboxOn(false);
    ui->SetQuestLineOn(false);
}


void QuestManager::StartStepTransition()
{
    if (phase_ != AnimPhase::None)
        return;

    pendingNextStep_ = (curStep < kMaxStep) ? (curStep + 1) : curStep;

    phase_ = AnimPhase::ShowSuccess;
    phaseTimer_ = kSuccessHoldSec;
}

void QuestManager::TickStepTransition(float dt)
{
    if (phase_ == AnimPhase::None)
        return;

    phaseTimer_ -= dt;
    if (phaseTimer_ > 0.0f)
        return;

    auto ui = MainGameUIManager::Instance();

    switch (phase_)
    {
    case AnimPhase::ShowSuccess:
        // 2초 유지 후 패널 닫기 시작
        if (ui) ui->QuestPannelClose(kCloseSec);   // ✅ 닫기 슬라이드 시작
        phase_ = AnimPhase::Closing;
        phaseTimer_ = kCloseSec;
        break;

    case AnimPhase::Closing:
        // 닫힌 상태에서 스텝 업데이트 (UI 내용 변경)
        curStep = ClampStep(pendingNextStep_);
        ApplyStepUI();

        // 그 다음 패널 열기
        if (ui) ui->QuestPannelOpen(kOpenSec);     // ✅ 열기 슬라이드 시작
        phase_ = AnimPhase::Opening;
        phaseTimer_ = kOpenSec;
        break;

    case AnimPhase::Opening:
        // 연출 종료
        phase_ = AnimPhase::None;
        phaseTimer_ = 0.0f;
        pendingNextStep_ = 0;
        break;

    default:
        phase_ = AnimPhase::None;
        break;
    }
}

void QuestManager::StepComplete(int compleateIndex)
{
    if (compleateIndex < kMinStep || compleateIndex > kMaxStep)
    {
        std::cout << "[QuestManager] Invalid compleateIndex : " << compleateIndex << std::endl;
        return;
    }

    if (compleateIndex != curStep)
        return;

    auto ui = MainGameUIManager::Instance();
    if (!ui)
    {
        std::cout << "[QuestManager] MainGameUIManager is null!" << std::endl;
        return;
    }

    // 성공 연출
    ui->SetQuestCheakboxOn(true);
    ui->SetQuestLineOn(true);

    // 슬라이드 연출
    // 2초 후 Close → 퀘스트 업데이트 → Open
    StartStepTransition();
}
