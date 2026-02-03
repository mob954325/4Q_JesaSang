#include "GameObject.h"
#include "DirectXCollision.h"
#include "../Manager/ComponentFactory.h"
#include "../EngineSystem/ScriptSystem.h"
#include "../EngineSystem/RenderSystem.h"
#include "System/InputSystem.h"
#include "../EngineSystem/PlayModeSystem.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Enableable>("Enableable")
        .property("Active", &Enableable::GetActiveSelf, &Enableable::SetActive);

    rttr::registration::class_<GameObject>("GameObject")
        
        .property("Name", &GameObject::name);
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string str)
{
    name = str;
}

void GameObject::RemoveComponent(Component* comp)
{
    // 찾기
    for(auto it = components.begin(); it != components.end(); it++)
    {       
        if(*it == comp)
        {
            components.erase(it);
            break;
        }
    }   

    // handle 찾기 - 실제 객체 파괴하는 단계
    for (auto it = handles.begin(); it != handles.end(); it++)
    {
        auto objPtr = ObjectSystem::Instance().Get<Component>(*it);
        if (objPtr == comp)
        {
            if (auto renderComp = dynamic_cast<RenderComponent*>(objPtr))
            {
                RenderSystem::Instance().UnRegister(renderComp);
            }
            else
            {
                ScriptSystem::Instance().UnRegister(objPtr);
            }

            objPtr->OnDestory();
            ObjectSystem::Instance().Destory(*it);
            handles.erase(it);
            break;
        }
    }
}

Transform* GameObject::GetTransform() const
{
	return transform;
}

std::vector<Component*> GameObject::GetComponents()
{
    return components;
}

bool GameObject::IsDestory()
{
    return isDestory;
}

void GameObject::Destory()
{
	isDestory = true; // 가지고 있는 모든 컴포넌트 파괴하기
}

Scene *GameObject::GetScene()
{
    return currentScene;
}

void GameObject::SetScene(Scene* scene)
{
    currentScene = scene;
}

nlohmann::json GameObject::Serialize() const
{
    // gameObject
    //  properties
    //      name : gmaeName
    //      components 
    //          component_1
    //              component1_element_1
    //              component1_element_2
    //              ....
    //          component_2
    //              ....

    nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();    
    datas["properties"] = nlohmann::json::object(); // 객체 생성

    // ID 저장 : GameObject의 ID로 기억한다. ( 모든 컴포넌트와 게임 오브젝트는 Object를 상속받기 때문에 )
    int parentID = -1; // 기본 값 : 루트 오브젝트
    if (transform != nullptr) 
    {
        Transform* parent = transform->GetParent();
        if (parent != nullptr) 
        {
            GameObject* parentObj = parent->GetOwner();
            if (parentObj != nullptr) 
            {
                parentID = static_cast<int>(parentObj->GetId());
            }
        }
    }
    datas["properties"]["ParentID"] = parentID;
    datas["properties"]["ID"] = static_cast<int>(GetId());

    // 오브젝트 내용 직렬화화
    for(auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);

        if(value.is_type<std::string>())
        {
            datas["properties"][propName] = value.get_value<std::string>();
        }
    }

    // 컴포넌트 내용 직렬화
    datas["properties"]["components"] = nlohmann::json::array();
    auto& comps = datas["properties"]["components"];

    for (auto& comp : components)
    {
        comps.push_back(comp->Serialize());
    }
    
    return datas;
}

void GameObject::Deserialize(const nlohmann::json objData)
{
    // objData : data["objects"]["properties"]

    rttr::type t = rttr::type::get(*this);
    if(!objData.contains("components")) return;

    const auto& registered = ComponentFactory::Instance().GetRegisteredComponents();

    for(auto& prop : objData["components"])
    {
        if(!prop.contains("type")) continue;

        std::string compName = prop["type"];
        if(compName == "Transform") // Transform은 게임 오브젝트가 생성 시에 추가된다.
        {
            auto trans = this->GetComponent<Transform>();
            trans->Deserialize(prop);
            continue;
        }

        auto it = registered.find(compName);
        if (it != registered.end())
        {
            Component* createdComp = it->second.creator(this);
            createdComp->Deserialize(prop);
        }          
    }
}

void GameObject::UpdateAABB()
{
    Transform* trans = transform;
    Vector3 updatedExtent = aabbBoxExtent * trans->GetScale();
    aabbBox.Center = trans->GetWorldPosition() + aabbCenter;
    aabbBox.Extents = updatedExtent;
}

void GameObject::Initialize()
{
    aabbBoxExtent = { 10.0f, 10.0f, 10.0f };
    aabbBox = { {0.0f, 0.0f, 0.0f}, aabbBoxExtent };
    transform = AddComponent<Transform>();
}

void GameObject::SetAABB(BoundingBox aabb)
{
    aabbBox = aabb;
}

void GameObject::SetAABB(Vector3 min, Vector3 max, Vector3 centor)
{
    auto tran = transform;

    aabbBox.Center = tran->GetLocalPosition();
    aabbBoxExtent = (max - min) / 2.0f;
    aabbCenter = centor;
}

void GameObject::ClearAll()
{
    for (auto it = components.begin(); it != components.end();)
    {
        it = components.erase(it);
    }

    // handle 찾기 - 실제 객체 파괴하는 단계
    for (auto it = handles.begin(); it != handles.end();)
    {
        auto objPtr = ObjectSystem::Instance().Get<Component>(*it);
        if (auto renderComp = dynamic_cast<RenderComponent*>(objPtr))
        {
            RenderSystem::Instance().UnRegister(renderComp);
        }
        else
        {
            ScriptSystem::Instance().UnRegister(objPtr);
        }

        objPtr->OnDestory();
        ObjectSystem::Instance().Destory(*it);
        it = handles.erase(it);
    }

    components.clear();
}

std::vector<Transform*> GameObject::GetChildern()
{
    return transform->GetChildren();
}

Transform* GameObject::GetChildByIndex(int index)
{
    if (index >= transform->GetChildren().size()) return nullptr;
    if (index < 0) return nullptr;

    return transform->GetChildByIndex(index);
}

Transform* GameObject::GetChildByName(std::string name)
{
    return transform->GetChildByName(name);
}

Transform* GameObject::GetParent()
{
    return transform->GetParent();
}


// -----------------------------
// Physics 충돌 이벤트 
// -----------------------------

// Rigid
void GameObject::BroadcastCollisionEnter(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCollisionEnter(other);
}

void GameObject::BroadcastCollisionStay(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCollisionStay(other);
}

void GameObject::BroadcastCollisionExit(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCollisionExit(other);
}


// Trigger
void GameObject::BroadcastTriggerEnter(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnTriggerEnter(other);
}

void GameObject::BroadcastTriggerStay(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnTriggerStay(other);
}

void GameObject::BroadcastTriggerExit(PhysicsComponent* other)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnTriggerExit(other);
}


// CCT Trigger
void GameObject::BroadcastCCTTriggerEnter(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTTriggerEnter(cct);
}

void GameObject::BroadcastCCTTriggerStay(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTTriggerStay(cct);
}

void GameObject::BroadcastCCTTriggerExit(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTTriggerExit(cct);
}


// CCT Collision
void GameObject::BroadcastCCTCollisionEnter(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTCollisionEnter(cct);
}

void GameObject::BroadcastCCTCollisionStay(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTCollisionStay(cct);
}

void GameObject::BroadcastCCTCollisionExit(CharacterControllerComponent* cct)
{
    // 우정 0203 | 에디터모드 충돌 방지코드 추가
    if (!PlayModeSystem::Instance().IsPlaying()) return;

    for (auto c : components)
        if (auto s = dynamic_cast<ScriptComponent*>(c))
            s->OnCCTCollisionExit(cct);
}
