#include "DialogueUIController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Components/UI/TextUI.h"

REGISTER_COMPONENT(DialogueUIController)

RTTR_REGISTRATION
{
    rttr::registration::class_<DialogueUIController>("DialogueUIController")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void DialogueUIController::OnStart()
{
    targetTr = GetOwner()->GetChildByName("DialoguePoint");

    auto curScene = SceneSystem::Instance().GetCurrentScene();
    dialogueParent = curScene->GetGameObjectByName("UI_PlayerDialogue")->GetTransform();
    image_dialogue = curScene->GetGameObjectByName("Image_Dialogue")->GetComponent<Image>();
    text_dialogue = curScene->GetGameObjectByName("Text_Dialoaue")->GetComponent<TextUI>();

    if (!targetTr || !dialogueParent || !image_dialogue || !text_dialogue)
    {
        cout << "[DialogueUIController] Missing Components!" << endl;
    }
}

void DialogueUIController::OnUpdate(float delta)
{
    if (!targetTr || !dialogueParent || !image_dialogue || !text_dialogue)
        return;

    // position trace
    dialogueParent->SetPosition(targetTr->GetWorldPosition() + offset);
}

void DialogueUIController::OnDestory()
{

}

nlohmann::json DialogueUIController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void DialogueUIController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}