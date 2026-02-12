#pragma once
#include "../../../Engine/Components/ScriptComponent.h"
#include "../ResultPanels/LosePanel.h"  
#include "../ResultPanels/WinPanel.h"

/// <summary>
/// WinPanel, losePanel 컨트롤 스크립트 클래스
/// 패널들은 시작 시 setActive(false)를 호출한다.
/// </summary>
class ResultPanel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnStart() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    LosePanel* losePanel{};
    WinPanel* winPanel{};
};