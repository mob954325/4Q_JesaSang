#include "QuestManager.h"

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
    if (curStep < 1) curStep = 1;
    if (curStep > 4) curStep = 4;

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

void QuestManager::StepComplete(int compleateIndex)
{
    if (compleateIndex < 1 || compleateIndex > 4)
    {
        cout << "[QuestManager] Invalid compleateIndex : " << compleateIndex << endl;
        return;
    }

    if (compleateIndex != curStep)
        return;

    auto ui = MainGameUIManager::Instance();
    if (!ui)
    {
        cout << "[QuestManager] MainGameUIManager is null!" << endl;
        return;
    }

    // TODO :: 여기서 성공 연출 딜레이
    ui->SetQuestCheakboxOn(true);
    ui->SetQuestLineOn(true);

    // 여기 슬라이드가 접혔다 다시 켜져야함


    // 다음 Step으로 Update
    if (curStep < 4) curStep++;
    ApplyStepUI();
}
