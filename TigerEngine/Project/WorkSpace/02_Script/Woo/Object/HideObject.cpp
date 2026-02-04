#include "HideObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "../Player/PlayerController.h"

REGISTER_COMPONENT(HideObject)

RTTR_REGISTRATION
{
    rttr::registration::class_<HideObject>("HideObject")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void HideObject::OnStart()
{
    hideDurationTimer = hideDurationTime;
    hideCoolTimer = 0.0f;
}

void HideObject::OnUpdate(float delta)
{
    // 은신 중일 때 (10초)
    if (isHiding)
    {
        hideDurationTimer -= delta;
        if (hideDurationTimer <= 0.0f)
        {
            isHiding = false;
            isCoolTime = true;
            hideCoolTimer = hideCoolTime;
            player->ChangeState(PlayerState::Idle);
        }
    }

    // 쿨타임 중일 때
    if (isCoolTime)
    {
        hideCoolTimer -= delta;
        if (hideCoolTimer <= 0.0f)
        {
            isCoolTime = false;
        }
    }
}

nlohmann::json HideObject::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void HideObject::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

bool HideObject::IsPossibleHide()
{
    return !isAILooking && !isHiding && !isCoolTime;
}

void HideObject::StartHide(PlayerController* p)
{
    isHiding = true;
    player = p;
    hideDurationTimer = hideDurationTime;
}

void HideObject::SetAILook(bool isLook)
{
    isAILooking = isLook;
}
