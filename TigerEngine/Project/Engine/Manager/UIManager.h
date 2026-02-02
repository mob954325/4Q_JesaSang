#pragma once
#include "../Components/UI/UIBase.h"
#include "System/Singleton.h"

/// <summary>
/// 자식 계층중 UIBase를 가지고 있는 게임 오브젝트를 관리하는 매니저
/// 특수한 컴포넌트 클래스로 해상도, 스케일 정책을 공유하고, sorting 등을 수행합니다.
/// </summary>
class UIManager : public Singleton<UIManager>
{
public:
    UIManager(token) {};
    ~UIManager() = default;

    /// <summary>
    /// client screen size 가져오기, 카메라 투영 계산도 포함됨.
    /// </summary>
    void SetSize(int w, int h);
    Vector2 GetSize();

    void Register(UIBase* ui);
    void UnRegister(UIBase* ui);

    Matrix GetProjection() const;

protected:
    /// <summary>
    /// 등록된 ui 컴포넌트들
    /// </summary>
    std::vector<UIBase*> uiComps;

    // UI ㅡMVP 계산 할 때 사용하는 투영 매트릭스 ( 직교 투영 )
    Matrix proj; 

    int width{};
    int height{};
};