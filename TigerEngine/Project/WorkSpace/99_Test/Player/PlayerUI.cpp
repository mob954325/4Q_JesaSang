#include "PlayerUI.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"


REGISTER_COMPONENT(PlayerUI)

void PlayerUI::OnInitialize()
{
    image = GetOwner()->AddComponent<Image>();
    auto rect = GetOwner()->AddComponent<RectTransform>();    
    rect->SetSize({ 100, 100 });
    image->SetDrawSpace(DrawSpaceType::World);
}

void PlayerUI::OnEnable()
{
}

void PlayerUI::OnStart()
{
}

void PlayerUI::OnDisable()
{
}

void PlayerUI::OnDestory()
{
}

void PlayerUI::OnUpdate(float delta)
{
}

nlohmann::json PlayerUI::Serialize()
{
    return nlohmann::json();
}

void PlayerUI::Deserialize(nlohmann::json data)
{
}
