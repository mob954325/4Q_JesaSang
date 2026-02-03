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
    void SetActive(bool value) { isActive = value; }

protected:
    bool isActive = true;
};
