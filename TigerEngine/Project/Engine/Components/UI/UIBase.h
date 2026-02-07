#pragma once
#include "../RenderComponent.h"
#include "../RectTransform.h"

class UIManager; // 순환참조 방지를 위한 전방 선언

/// <summary>
/// UI 컴포넌트가 상속받을 클래스
/// </summary>
/// <remarks>
/// rttr 설정 번거로울 꺼 같아서 공통된 부분 그냥 수동 작성함.
/// </remarks>
class UIBase : public RenderComponent
{
public:
    // NOTE 
    // 1. Init은 UIBase를 상속받은 객체가 정의한다.
    // 2. Render은 UIBase를 상속받은 객체가 정의한다.

protected:
    // TODO : zLayer 추가하기

    UIManager* canvas; // 현재 상위 객체가 가지고 있는 canvas
    RectTransform rect;
};