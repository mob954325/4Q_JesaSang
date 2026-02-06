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
    hideDurationTimer = 0.0f;
    reHideCoolTimer = reHideCoolTime;
    isHiding = false;
}

void HideObject::OnUpdate(float delta)
{
    // 은신 중일 때 최대 은신 시간 controll
    if (isHiding)
    {
        hideDurationTimer += delta;

        // 최대 은신시간 넘기면 플레이거 나가~!
        if (hideDurationTimer >= hideDurationTime)
        {
            player->ChangeState(PlayerState::Idle);
            StopHide();
            cout << "[HideObject] Max Hide Time... Player Get Out!" << endl;
        }

        return;
    }

    // 재은신 쿨타임 controll
    if (!reHideReady)
    {
        reHideCoolTimer += delta;
        if (reHideCoolTimer >= reHideCoolTime)
        {
            reHideReady = true;
            reHideCoolTimer = reHideCoolTime;
            cout << "[HideObject] Re Hide Possible!" << endl;
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
    return !isAILooking && !isHiding && reHideReady;
}

void HideObject::StartHide(PlayerController* p)
{
    player = p;

    isHiding = true;
    hideDurationTimer = 0.0f;
    reHideReady = false;
    reHideCoolTimer = 0.0f;
}

void HideObject::StopHide()
{
    // 이미 숨고 있지 않으면 무시
    if (!isHiding)
        return;

    // HideObject 관점에서 은신 종료 처리
    isHiding = false;
    hideDurationTimer = 0.0f;

    // 재은신 쿨타임 시작
    reHideReady = false;
    reHideCoolTimer = 0.0f;

    // 안전 처리
    player = nullptr;

    cout << "[HideObject] Stop Hide Interrupted" << endl;
}

void HideObject::SetAILook(bool isLook)
{
    // isAILooking = isLook; // 바라보고 있는 AI 수를 체크해서 Se 선민 | 02.07 

    if (isLook)
        lookingAICount++;
    else
        lookingAICount--;

    if (lookingAICount < 0)
        lookingAICount = 0;

    isAILooking = (lookingAICount > 0);
    cout << "[HideObject] isAILooking = " << isAILooking << endl;
}
