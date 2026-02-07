#include "TestButton.h"
#include "Object/GameObject.h"

void TestButton::OnInitialize()
{
    imageEnter = GetOwner()->AddComponent<Image>();
    imageNormal = GetOwner()->AddComponent<Image>();
    imagePressed = GetOwner()->AddComponent<Image>();

    imageEnter->OnEnter.AddListener(imageEnter, [&]() { imageEnter->SetActive(true); });
    imageEnter->OnClick.AddListener(imageEnter, [&]() { imageEnter->SetActive(false); imagePressed->SetActive(true); });
    imageEnter->OnEnter.AddListener(imageEnter, [&]() { imageEnter->SetActive(false); });

    imageNormal->OnEnter.AddListener(imageNormal, [&]() { imageNormal->SetActive(false); });
    imageNormal->OnClick.AddListener(imageNormal, [&]() { imageNormal->SetActive(false); });
    imageNormal->OnEnter.AddListener(imageNormal, [&]() { imageNormal->SetActive(true); });

    imagePressed->OnEnter.AddListener(imagePressed, [&]() { imagePressed->SetActive(false); });
    imagePressed->OnClick.AddListener(imagePressed, [&]() { imagePressed->SetActive(true); });
    imagePressed->OnEnter.AddListener(imagePressed, [&]() { imagePressed->SetActive(false); });
}

void TestButton::OnEnable()
{
}

void TestButton::OnStart()
{
}

void TestButton::OnDisable()
{
}

void TestButton::OnDestory()
{
}

void TestButton::OnUpdate(float delta)
{
}

nlohmann::json TestButton::Serialize()
{
    return nlohmann::json();
}

void TestButton::Deserialize(nlohmann::json data)
{
}
