#include "HideObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(HideObject)

RTTR_REGISTRATION
{
    rttr::registration::class_<HideObject>("HideObject")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void HideObject::OnStart()
{

}

void HideObject::OnUpdate(float delta)
{
    
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
    return true;
}

void HideObject::SetAILook(bool isLook)
{
    isPossibleHide = isLook;
}
