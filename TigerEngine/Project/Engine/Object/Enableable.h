#pragma once
#include "../Base/pch.h"
#include "System/ObjectSystem.h"

/// <summary>
/// 활성화 비활성화 여부를 가지고 있는 클래스
/// </summary>
/// <remarks>
/// Unity에서 GameObject는 SetActive, Component는 isEnable로 체크하지만 
/// 현재 프로젝트에서 나눌 이유는 없을거 같아서 해당 클래스를 상속받도록함. [02.03]
/// 용어가 Enable, Active가 혼용되어 있지만, 객체의 활성화 비활성화 여부만 확인한다.
/// </remarks>
class Enableable : public Object
{
    RTTR_ENABLE()
public:
    bool GetActiveSelf() const { return isActive; }
    void SetActive(bool value)
    {
        if (isActive == value) return;

        isActive = value;
        if (isActive) Enable_Inner();
        else Disable_Inner();
    }

protected:
    /// <summary>
    /// 활성화 시 등록 함수
    /// </summary>
    virtual void Enable_Inner() {}   // Engine Component 정의시 필수 override 필요

    /// <summary>
    /// 활성화 시 등록 해제 함수
    /// </summary>
    virtual void Disable_Inner() {}  // Engine Component 정의시 필수 override 필요

    bool isActive = true;
};
