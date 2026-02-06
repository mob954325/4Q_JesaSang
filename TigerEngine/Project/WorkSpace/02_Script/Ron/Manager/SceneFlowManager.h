#pragma once
#include "Components/ScriptComponent.h"


/*
    [ SceneFlowManager Script Component ] <Singleton>

    숫자키로 씬 전환 (1~4)
    Start 씬에서 Space/Enter로 Intro 전환
*/

class SceneFlowManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    static SceneFlowManager* s_instance;

public:
    void OnInitialize();
    void OnStart();
    void OnDestory();
    void OnUpdate(float delta);

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    static SceneFlowManager* Instance() { return s_instance; }

    // Scene paths
    std::string startScenePath = "..\\Assets\\Scenes\\01_StartScene.json";
    std::string introScenePath = "..\\Assets\\Scenes\\02_IntroScene.json";
    std::string tutorialScenePath = "..\\Assets\\Scenes\\03_TutorialScene.json";
    std::string mainScenePath = "..\\Assets\\Scenes\\04_MainGameScene.json";

    // Options
    bool allowNumberKeys = true;
    bool allowStartAdvance = false; // only enable in Start scene

private:
    void LoadScene(const std::string& path);
};
