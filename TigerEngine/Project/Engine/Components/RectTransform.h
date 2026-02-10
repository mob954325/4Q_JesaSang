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
    void Enable_Inner() override;
    void Disable_Inner() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    const Vector3& GetPos() const;
    void SetPos(const Vector3& vec);
    const Vector2& GetSize() const;
    void SetSize(const Vector2& vec);
    const Vector2& GetPivot() const;
    void SetPivot(const Vector2& vec);

    // rectTransform의 계층 구조는 사용하지 않고 OWner의 구조에 있는 children를 dirty 활성화
    void SetChildrenDirty() override;

    Matrix RemoveScale(Matrix& m);

    // === layout data ===
    Vector2 anchorMin = { 0.5f, 0.5f };
    Vector2 anchorMax = { 0.5f, 0.5f };
    Vector2 anchoredPos = { 0.0f, 0.0f }; // px in REF
    Vector2 sizeDelta = { 100.0f, 100.0f }; // px in REF

    const Vector2& GetAnchorMin() const { return anchorMin; }
    const Vector2& GetAnchorMax() const { return anchorMax; }
    const Vector2& GetAnchoredPos() const { return anchoredPos; }
    const Vector2& GetSizeDelta() const { return sizeDelta; }

    Vector2 one = Vector2::One;
    Vector2 zero = Vector2::Zero;
    void SetAnchorMin(const Vector2& v) { anchorMin = { std::clamp(v.x, 0.0f, 1.0f), std::clamp(v.y,0.0f,1.0f) }; dirty = true; SetChildrenDirty(); }
    void SetAnchorMax(const Vector2& v) { anchorMax = { std::clamp(v.x, 0.0f, 1.0f), std::clamp(v.y,0.0f,1.0f) }; dirty = true; SetChildrenDirty(); }
    void SetAnchoredPos(const Vector2& v) { anchoredPos = v; dirty = true; SetChildrenDirty(); }
    void SetSizeDelta(const Vector2& v) { sizeDelta = v; dirty = true; SetChildrenDirty(); }

    Vector2 GetRectSizeRef() const;

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