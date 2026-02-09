#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class RectTransform;
class Image;
class Text;

/*
    [ MainGameUIManager Script Component ] <Singleton>

    메인게임의 screen space에 있는 ui를 모두 소유하고
    다른 스크립트에게 유틸을 제공합니다.
*/

class MainGameUIManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static MainGameUIManager* s_instance = nullptr;

    // --- [Screen UI] -----------------
    // player life
    Image* life_1 = nullptr;
    Image* life_2 = nullptr;
    Image* life_3 = nullptr;
    Image* life_4 = nullptr;
    Image* life_5 = nullptr;

public:
    // rttr
    std::string lifeOnImagePath = "";  
    std::string lifeOffImagePath = "";

public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // image path
    const std::string& GetPressImagePath() const;
    void SetPressImagePath(const std::string& path);

public:
    // 외부 call fucns..
    static MainGameUIManager* Instance() { return s_instance; }

    // Life
    void UpdateLifeUI(int currentLife);
};

