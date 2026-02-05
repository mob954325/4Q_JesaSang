#include "TrapObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

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
}

nlohmann::json TrapObject::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void TrapObject::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}