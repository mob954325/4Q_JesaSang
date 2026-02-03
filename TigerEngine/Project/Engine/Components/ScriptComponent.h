#pragma once
#include "../Object/Component.h"
#include "PhysicsComponent.h"

/// <summary>
/// 사용자 정의 컴포넌트가 상속 받은 컴포넌트 클래스
/// 단순히 Engine부에서의 컴포넌트와 사용자가 만든 컴포넌트 분리용 클래스
/// </summary>
/// 
class ScriptComponent : public Component
{
    RTTR_ENABLE(Component)
public:
    ScriptComponent() = default;
    ~ScriptComponent() = default;

    bool IsPlayModeOnly() override { return true; }

protected:
    void Enable() override
    {
        auto ptr = ObjectSystem::Instance().Get<Component>(handle);
        ScriptSystem::Instance().RegisterScript(ptr);
        OnEnable();
    }

    void Disable() override
    {
        auto ptr = ObjectSystem::Instance().Get<Component>(handle);
        ScriptSystem::Instance().UnRegisterScript(ptr);
        OnDisable();
    }
public:
    virtual void OnCollisionEnter(PhysicsComponent*) {}
    virtual void OnCollisionStay(PhysicsComponent*) {}
    virtual void OnCollisionExit(PhysicsComponent*) {}

    virtual void OnTriggerEnter(PhysicsComponent*) {}
    virtual void OnTriggerStay(PhysicsComponent*) {}
    virtual void OnTriggerExit(PhysicsComponent*) {}

    virtual void OnCCTTriggerEnter(CharacterControllerComponent*) {}
    virtual void OnCCTTriggerStay(CharacterControllerComponent*) {}
    virtual void OnCCTTriggerExit(CharacterControllerComponent*) {}

    virtual void OnCCTCollisionEnter(CharacterControllerComponent*) {}
    virtual void OnCCTCollisionStay(CharacterControllerComponent*) {}
    virtual void OnCCTCollisionExit(CharacterControllerComponent*) {}
};