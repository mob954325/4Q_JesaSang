#pragma once
#include "Transform.h"

/// <summary>
/// UI컴포넌트가 사용하는 Transform으로 기존 transform 내용이 동작하지 않을 수 있음.
/// </summary>
/// <remarks>
/// pos     : 스크린 상의 포지션 좌표값
/// size    : width, height
/// pivot   : 0-1의 중심 값
/// </remarks>
class RectTransform : public Transform
{
    RTTR_ENABLE(Transform)
public:
    RectTransform() { SetName("RectTransform"); }
    ~RectTransform() = default;

    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    const Vector3& GetPos() const;
    void SetPos(const Vector3& vec);
    const Vector2& GetSize() const;
    void SetSize(const Vector2& vec);
    const Vector2& GetPivot() const;
    void SetPivot(const Vector2& vec);

protected:
    void UpdateMatricesIfDirty() override;

    /// <summary>
    /// 앵커를 기준으로 하는 사각형 피벗 포지션 -> transform의 position이랑 다름
    /// </summary>
    Vector3 pos{};

    /// <summary>
    /// 가로 새로 크기
    /// </summary>
    Vector2 size{};

    /// <summary>
    /// 회전 중심 점 위치, ((0,0)은 왼쪽 하단, (1,1)은 오른쪽 상단)
    /// </summary>
    Vector2 pivot{};
};