#include "TrapObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "../Player/PlayerController.h"
#include "EngineSystem/SceneSystem.h"

REGISTER_COMPONENT(TrapObject)

RTTR_REGISTRATION
{
    rttr::registration::class_<TrapObject>("TrapObject")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void TrapObject::OnStart()
{
}

void TrapObject::OnUpdate(float delta)
{
    // wave 파동중일때 re wave cool time udapte
    if (!isWaveing)
        return;

    coolTimer += delta;
    if (coolTimer >= coolTime)
    {
        // 재발동 가능
        isPossibleWave = true;
        isWaveing = false;

        // clear
        coolTimer = 0.0f;
        waveRadius = 0.0f;
    }
}

nlohmann::json TrapObject::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void TrapObject::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void TrapObject::OnCCTTriggerEnter(CharacterControllerComponent* other)
{
    if (!isPossibleWave) return;        // 최초1회, re wave 가능할때

    GameObject* otherOwner = other->GetOwner();
    if (otherOwner->GetName() != "Player") return;

    // 플레이어 상태에 따른 파장 범위 설정
    auto pc = otherOwner->GetComponent<PlayerController>();
    if (!pc) return;

    auto state = pc->GetPlayerState();

    switch (state)
    {
    case PlayerState::Walk:
        waveRadius = walkWaveRadius;
        break;
    case PlayerState::Run:
    case PlayerState::Hit:
        waveRadius = runWaveRadius;
        break;
    default:
        return; // 파장 발생 안 함
    }

    // 파장 웨이브 시작
    StartTriggerWave();     
}


void TrapObject::StartTriggerWave()
{
    // TODO :: 파장 연출

    // AI
    NotifyAIInRange();
    
    // re wave cool time start
    isWaveing = true;
    isPossibleWave = false;
    coolTimer = 0.0f;

    cout << "[TrapObject] : Start Trap Trigger Wave. Range : " << waveRadius << endl;
}

void TrapObject::NotifyAIInRange()
{
    // TODO :: 선민이 AI 찾아서 Call 하는 함수 호출
    /*
        파동 발생한 즉시 월드의 AI를 모두 찾아서 dist > waveLength보다 크다면 AI Called
    */

    //auto baby_AIs = SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName("baby_AIs");
    //auto ancestor_AIs = SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName("ancestor_AIs");


    // test

    auto searchObect = SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName("SearchObject");
    if (searchObect.empty()) cout << "searchObects : empty!" << endl;
    else
    {
        cout << "searchObect cout : " << searchObect.size() << endl;
    }
}
