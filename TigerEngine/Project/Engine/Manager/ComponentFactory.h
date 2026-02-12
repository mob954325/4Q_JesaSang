#pragma once
#include "pch.h"
#include "System/Singleton.h"
#include "../Object/GameObject.h"

enum class ComponentCategory
{
    Core, Rendering, Audio, Physics, Animation, Script, UI, Other
};

using createCompFunc = std::function<Component* (GameObject*)>;

struct ComponentEntry
{
    std::string name;
    ComponentCategory category;
    createCompFunc creator;
};

using createCompFunc = std::function<Component*(GameObject*)>;

/// @brief 컴포넌트 조회용 클래스
/// 엔진에 사용하는 모든 컴포넌트들은 해당 클래스에 등록된다.
class ComponentFactory : public Singleton<ComponentFactory>
{
public:
    ComponentFactory(token) {};
    ~ComponentFactory() = default;       

    bool isRegisteredAll = false;
    bool suppressAutoActivate = false; // NOTE : Scene 점진 로딩 등에서 생성 직후 활성화(등록) 방지

    template<typename T>
    void Register(std::string compName, ComponentCategory cat = ComponentCategory::Other);

    const std::unordered_map<std::string, ComponentEntry>& GetRegisteredComponents();

private:
    std::unordered_map<std::string, ComponentEntry> registeredComponents; // 컴포넌트 이름, 컴포넌트 생성 람다 함수
};

template<typename T>
inline void ComponentFactory::Register(std::string compName, ComponentCategory cat)
{
    auto createComp = [this, name = compName](GameObject* obj)
        {
            auto comp = obj->AddComponent<T>();
            if (auto sp = comp)
            {
                sp->SetName(name);
                if (!suppressAutoActivate)
                {
                    sp->SetActive(true); // 해당 컴포넌트 활성화 ( false -> true )
                }
                cout << name << " : factory call \n";
            }
            return comp;
        };
    
    registeredComponents[compName] = { compName, cat, createComp };
}
