#include "OcclusionFader.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "Components/Camera.h"

REGISTER_COMPONENT(OcclusionFader)

RTTR_REGISTRATION
{
    rttr::registration::class_<OcclusionFader>("OcclusionFader")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void OcclusionFader::OnInitialize()
{
}

void OcclusionFader::OnStart()
{
}

void OcclusionFader::OnUpdate(float delta)
{
}

void OcclusionFader::OnDestory()
{
}

nlohmann::json OcclusionFader::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void OcclusionFader::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}