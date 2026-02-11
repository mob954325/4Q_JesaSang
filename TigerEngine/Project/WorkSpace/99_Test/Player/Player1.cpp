#include "Player1.h"
#include "Object/GameObject.h"
#include "System/InputSystem.h"
#include "Object/Component.h"
#include "Components/FBXRenderer.h"
#include "Util/ComponentAutoRegister.h"
#include "../Engine/EngineSystem/SceneSystem.h"
#include "Util/PrefabUtil.h"
#include "../Engine//EngineSystem/CameraSystem.h"

REGISTER_COMPONENT(Player1);

RTTR_REGISTRATION
{
    rttr::registration::class_<Player1>("Player1")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
    .property("r", &Player1::r)
    .property("g", &Player1::g)
    .property("b", &Player1::b);
}

void Player1::OnInitialize()
{
    // cout << "== Player1 init ==\n";
    // weapon = GetOwner()->AddComponent<Weapon>();

    // cout << "GameObject add\n";
    // auto weaponObj = SceneUtil::CreateGameObject("TestWeapon");

    //GetOwner()->GetTransform()->AddChild(weaponObj->GetTransform());
    // auto objIndex = GetOwner()->GetChildByIndex(0);
    // cout << "Find Obj index" << objIndex << " : " << objIndex->GetName() << "\n";

    //auto objName = GetOwner()->GetChildByName("TestWeapon");
    // cout << "Find Obj name " << objName->GetName() << " : " << objIndex->GetName() << "\n";

    // cout << "[Player1 | PlayModeTest] : OnInitalize() 1\n";    
}

void Player1::OnEnable()
{
    // cout << "[Player1 | PlayModeTest] : OnEnable() 2\n";    
    // soket = SceneUtil::CreateGameObject("PlayerSoket");
    // soket->SetParent(GetOwner());

}

void Player1::OnStart()
{
    // cout << "[Player1 | PlayModeTest] : OnStart() 3\n";    
    // GameObject* obj = SceneUtil::GetObjectByName("Weapon");

    // if(obj)
        // cout << "SceneUtiltest : " << obj->GetName() << endl;

    // GameObject* instantiated = PrefabUtil::Instantiate("Test1");
    // instantiated->GetTransform()->SetParent(this->GetOwner()->GetTransform());
    audioClip = GetOwner()->GetComponent<AudioSourceComponent>();

    if (audioClip)
    {
        audioClip->Play();
        bool isloop = audioClip->GetLoop();
        audioClip->SetLoop(!isloop);
    }
}

void Player1::OnDisable()
{
    // cout << "[Player1 | PlayModeTest] : OnDisable() 4\n";
}

void Player1::OnDestory()
{
    // cout << "[Player1 | PlayModeTest] : OnDestory() 5\n";
}

void Player1::OnUpdate(float delta)
{
    auto comp = GetOwner()->GetComponent<FBXRenderer>();
    if (comp != nullptr)
    {
        comp->SetDiffuseOverride({ r, g, b, 1.0f });
    }

    r += delta;
    g += delta * 2.0f;
    b += delta * 3.0f;

    if (r > 1.0f) r = 0.f;
    if (g > 1.0f) g = 0.f;
    if (b > 1.0f) b = 0.f;

    auto trans = GetOwner()->GetTransform();
    if (Input::GetKey(DirectX::Keyboard::Keys::Up))
        trans->Translate({ 0, 0, 1.f });
    else if (Input::GetKey(DirectX::Keyboard::Keys::Down))
        trans->Translate({ 0, 0, -1.f });
    if (Input::GetKey(DirectX::Keyboard::Keys::Left))
        trans->Translate({ -1.f, 0, 0 });
    else if (Input::GetKey(DirectX::Keyboard::Keys::Right))
        trans->Translate({ 1.f, 0, 0 });

    if (Input::GetKey(DirectX::Keyboard::Keys::Z))
        CameraSystem::Instance().SetCurrCameraByName("cam1");
    if (Input::GetKey(DirectX::Keyboard::Keys::X))
        CameraSystem::Instance().SetCurrCameraByName("cam2");
    if (Input::GetKey(DirectX::Keyboard::Keys::C))
        CameraSystem::Instance().SetCurrCameraByName("cam3");

    if (Input::GetKeyDown(DirectX::Keyboard::I))
    {
        audioClip->Pause(true);
    }

    if (Input::GetKeyDown(DirectX::Keyboard::O))
    {
        audioClip->Pause(false);
    }

    if (Input::GetKeyDown(DirectX::Keyboard::P))
    {
        audioClip->Stop();
    }

    if (Input::GetKeyDown(DirectX::Keyboard::L))
    {
        audioClip->Play();
    }

    if (Input::GetKeyDown(DirectX::Keyboard::K))
    {
        bool isloop = audioClip->GetLoop();
        audioClip->SetLoop(!isloop);
    }

    // sync 

    auto pos = GetOwner()->GetTransform()->GetWorldPosition();
    auto fwd = GetOwner()->GetTransform()->GetForward();
    auto up = GetOwner()->GetTransform()->GetUp();
    Vector3 vel = Vector3::Zero;
    if (hasPrev && delta > 0.0001f)
        vel = (pos - prevPos) / delta * 0.01f;

    prevPos = pos;
    hasPrev = true;

    AudioTransform t{};
    t.position = { pos.x, pos.y, pos.z };       // 위치
    t.forward = { fwd.x, fwd.y, fwd.z };        // forward
    t.up = { up.x,  up.y,  up.z };              // up vector
    t.velocity = { vel.x, vel.y, vel.z };       // "초당 이동량"(world-space). doppler 등에 사용됨. ( 청자(Listener)가 얼마나 / 어느 방향으로 움직이는지 알려주는 값 

    audioClip->SetFallback(t); // AudioListenerComponent가 Update()에서 적용(Engine/Components/AudioListenerComponent.cpp:61)
}

nlohmann::json Player1::Serialize()
{
    nlohmann::json datas;

    rttr::type t = rttr::type::get(*this);
    datas["type"] = t.get_name().to_string();
    datas["properties"] = nlohmann::json::object(); // 객체 생성

    for (auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);
        if (value.is_type<float>())
        {
            auto v = value.get_value<float>();
            datas["properties"][propName] = v;
        }
    }

    return datas;
}

void Player1::Deserialize(nlohmann::json data)
{
    auto propData = data["properties"];

    rttr::type t = rttr::type::get(*this);
    for (auto& prop : t.get_properties())
    {
        std::string propName = prop.get_name().to_string();
        rttr::variant value = prop.get_value(*this);
        if (value.is_type<float>() && propName == "r")
        {
            float data = propData["r"];
            // Test : prop.set_value 없이 값이 적용되는지 확인하기 위한 코드
            // prop.set_value(*this, data); 
            r = data;
        }
        else if (value.is_type<float>() && propName == "g")
        {
            float data = propData["g"];
            prop.set_value(*this, data);
            g = data;
        }
        else if (value.is_type<Color>() && propName == "b")
        {
            float data = propData["b"];
            prop.set_value(*this, data);
        }
    }
}
