#include "OcclusionFadeObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "System/InputSystem.h"

#include "Object/GameObject.h"
#include "Components/Camera.h"
#include <algorithm> 


REGISTER_COMPONENT(OcclusionFadeObject)

RTTR_REGISTRATION
{
    rttr::registration::class_<OcclusionFadeObject>("OcclusionFadeObject")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void OcclusionFadeObject::OnStart()
{
    fbxRenderer = GetOwner()->GetComponent<FBXRenderer>();
    if(!fbxRenderer)
    {
        cout << "[OcclusionFadeObject] OnStart Missing 'fbxRenderer Component'" << endl;
        return;
    }

    // init
    currentOpacity = 1.0f;
    startOpacity = currentOpacity;
    ApplyOpacity(currentOpacity);
}

void OcclusionFadeObject::OnUpdate(float delta)
{
    if (!fbxRenderer) return;

    // 투명화 / 복구
    if (isFading)
        OcclusionFadeProcess(delta);
}

nlohmann::json OcclusionFadeObject::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void OcclusionFadeObject::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void OcclusionFadeObject::OcclusionFadeProcess(float delta)
{
    if (fadeDuration <= 0.0001f)
    {
        currentOpacity = endOpacity;
        ApplyOpacity(currentOpacity);
        OcclusionFadeReset();
        return;
    }

    currentFadeTime += delta;

    float t = std::clamp(currentFadeTime / fadeDuration, 0.0f, 1.0f);
    currentOpacity = startOpacity + (endOpacity - startOpacity) * t;

    ApplyOpacity(currentOpacity);

    if (t >= 1.0f)
        OcclusionFadeReset();
}

void OcclusionFadeObject::OcclusionFadeReset()
{
    isFading = false;
    currentFadeTime = 0.0f;

    //  RenderType : Opaque
    if (currentOpacity >= 0.999f)
    {
        currentOpacity = 1.0f;
        ApplyOpacity(currentOpacity);
        fbxRenderer->renderBlendType = RenderBlendType::Opaque;
    }
}

void OcclusionFadeObject::BeginFade(float newTargetOpacity)
{
    newTargetOpacity = std::clamp(newTargetOpacity, 0.0f, 1.0f);

    // 값이 같거나 이미 투명화/복구중이면 무시
    const float eps = 0.001f;
    if (isFading && std::fabs(endOpacity - newTargetOpacity) <= eps)
        return;

    // start, target
    startOpacity = currentOpacity;
    endOpacity = newTargetOpacity;

    currentFadeTime = 0.0f;
    isFading = true;

    // RenderType : Transparent
    if (endOpacity < 0.999f)
        fbxRenderer->renderBlendType = RenderBlendType::Transparent;
}

void OcclusionFadeObject::ApplyOpacity(float opacity)
{
    fbxRenderer->SetAlpha(opacity);
}

void OcclusionFadeObject::StartFadeIn()
{
    if (!fbxRenderer) {
        cout << "[OcclusionFadeObject] StartFadeIn Missing 'fbxRenderer Component'" << endl;
        return;
    }

    BeginFade(fadeInOpacity);
    cout << "[OcclusionFadeObject] StartFadeIn" << endl;
}

void OcclusionFadeObject::StartFadeOut()
{
    if (!fbxRenderer) {
        cout << "[OcclusionFadeObject] StartFadeOut Missing 'fbxRenderer Component'" << endl;
        return;
    }

    BeginFade(1.0f);
    cout << "[OcclusionFadeObject] StartFadeOut" << endl;
}
