#include "PlayerSoundSource.h"
#include "Object/GameObject.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"
#include <random>

REGISTER_COMPONENT(PlayerSoundSource)

RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerSoundSource>("PlayerSoundSource")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void PlayerSoundSource::OnStart()
{
    sound = GetOwner()->GetComponent<AudioSourceComponent>();
    std::cout << "[Player Sound Source] Getcomponent AudioSOurce" << endl;

    if (!sound) // 없으면 생성후 연결
    {
        sound = GetOwner()->AddComponent<AudioSourceComponent>();
        std::cout << "[Player Sound Source] Addcomponent AudioSOurce" << endl;
    }
}

nlohmann::json PlayerSoundSource::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PlayerSoundSource::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void PlayerSoundSource::PlaySound(PlayerSoundType type, bool loop)
{
    if (!sound) return;
    std::string id = "";

    // NOTE id는 csv에서 정해짐
    switch (type)
    {
    case PlayerSoundType::Sit:
        id = "Player_Sit_Sound";
        break;
    case PlayerSoundType::Walk:
        id = "Player_Walk_Sound";
        break;
    case PlayerSoundType::Run:
        id = "Player_Run_Sound";
        break;
    case PlayerSoundType::Hit:
        id = "Player_Hit_Sound";
        break;
    case PlayerSoundType::Confused:
        id = "Player_Confused_Sound";
        break;
    case PlayerSoundType::HpDown:
        id = "Player_HpDown_Sound";
        break;
    case PlayerSoundType::Typing:
        id = "Player_Typing_Sound";
        break;
    default:
        break;
    }
    sound->SetLoop(loop);
    sound->SetClipId(id);
    sound->Play();

    std::cout << "Play Sound : " << id << std::endl;
}

void PlayerSoundSource::StopSound()
{
    if (!sound) return;
    sound->Stop();
}

void PlayerSoundSource::PlayRandomFootStep()
{
    if (!sound) return;

    // RNG (한 번만 초기화)
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // enum 범위: 0 ~ 4 (현재 5개라고 가정)
    std::uniform_int_distribution<int> dist(
        0,
        static_cast<int>(PlayerFootStepType::Player_Walk_Sound5)
    );

    PlayerFootStepType type = static_cast<PlayerFootStepType>(dist(gen));

    std::string id;

    // NOTE id는 csv에서 정해짐 (여기 문자열을 csv id와 일치시키세요)
    switch (type)
    {
    case PlayerFootStepType::Player_Walk_Sound1:
        id = "Player_FootStep_Sound1";
        break;
    case PlayerFootStepType::Player_Walk_Sound2:
        id = "Player_FootStep_Sound2";
        break;
    case PlayerFootStepType::Player_Walk_Sound3:
        id = "Player_FootStep_Sound3";
        break;
    case PlayerFootStepType::Player_Walk_Sound4:
        id = "Player_FootStep_Sound4";
        break;
    case PlayerFootStepType::Player_Walk_Sound5:
        id = "Player_FootStep_Sound5";
        break;
    default:
        return;
    }

    sound->SetLoop(false);
    sound->SetClipId(id);
    sound->Play();
}
