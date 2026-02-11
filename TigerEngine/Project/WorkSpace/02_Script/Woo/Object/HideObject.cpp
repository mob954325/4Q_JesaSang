#include "HideObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Components/UI/Image.h"
#include "../Player/PlayerController.h"
#include "../Engine/Object/GameObject.h"

REGISTER_COMPONENT(HideObject)

RTTR_REGISTRATION
{
    rttr::registration::class_<HideObject>("HideObject")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void HideObject::UpdateJagaejangModel()
{
    if (!isJagaejang) return;
    if (!openModel || !closeModel) return;

    // 플레이어가 안 숨어있으면 open 활성화, 숨어있으면 close 활성화
    openModel->SetActive(!isHiding);
    closeModel->SetActive(isHiding);
}

void HideObject::OnStart()
{
    // get child compoennts
    auto t1 = GetOwner()->GetChildByName("Image_SensorOn");
    auto t2 = GetOwner()->GetChildByName("Image_InteractionOn");
    auto t3 = GetOwner()->GetChildByName("Image_InteractionGauge");

    if (!t1 || !t2 || !t3)
    {
        cout << "[SearchObject] Missing child transforms!" << endl;
        return;
    }

    image_sensorOn = t1->GetOwner()->GetComponent<Image>();
    image_interactionOn = t2->GetOwner()->GetComponent<Image>();
    image_interactionGauge = t3->GetOwner()->GetComponent<Image>();

    // 자개장
    auto openTr = GetOwner()->GetChildByName("OpenModel");
    auto closeTr = GetOwner()->GetChildByName("CloseModel");

    openModel = openTr ? openTr->GetOwner() : nullptr;
    closeModel = closeTr ? closeTr->GetOwner() : nullptr;

    isJagaejang = (openModel != nullptr && closeModel != nullptr);

    // init
    hideDurationTimer = 0.0f;
    isHiding = false;
    reHideReady = true;
    reHideCoolTimer = reHideCoolTime;

    // UI
    UIGaugeUpate(0.0f);
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

        // UI
        float progress = 1.0f - (reHideCoolTimer / reHideCoolTime);
        progress = std::clamp(progress, 0.0f, 1.0f);
        UIGaugeUpate(progress);

        if (reHideCoolTimer >= reHideCoolTime)
        {
            reHideReady = true;
            reHideCoolTimer = reHideCoolTime;
            UIGaugeUpate(0.0f);
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

    UpdateJagaejangModel();
    UISensorOnOff(false);
    UIInteractionOnOff(false);
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

    player = nullptr;

    UpdateJagaejangModel();

    // UI
    UIGaugeUpate(1.0f);
    UISensorOnOff(true);
    UIInteractionOnOff(true);

    cout << "[HideObject] Stop Hide Interrupted" << endl;
}

void HideObject::UISensorOnOff(bool flag)
{
    if (!image_sensorOn) return;
    if (flag && !IsPossibleHide()) return;
    image_sensorOn->SetActive(flag);
}

void HideObject::UIInteractionOnOff(bool flag)
{
    if (!image_interactionOn) return;
    image_interactionOn->SetActive(flag);
    image_interactionGauge->SetActive(flag);
}

void HideObject::UIGaugeUpate(float progress)
{
    if (!image_interactionGauge) return;
    image_interactionGauge->SetFillAmount(progress);
}
