#include "PlayerSoundSource.h"
#include "Object/GameObject.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"

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

    if (!sound) // 없으면 생성후 연결
    {
        sound = GetOwner()->AddComponent<AudioSourceComponent>();
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
        id = "Player_Hpdown_Sound";
        break;
    case PlayerSoundType::HpDown:
        id = "Player_Sit_Sound";
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
}