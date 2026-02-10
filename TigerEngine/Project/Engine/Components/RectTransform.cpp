#include "RectTransform.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"
#include "../Manager/UIManager.h"

RTTR_REGISTRATION
{
        rttr::registration::class_<RectTransform>("RectTransform") // NOTE : 로드시 rect가 두 개임 ??
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("Pos",    &RectTransform::GetPos,    &RectTransform::SetPos)
        .property("Size",   &RectTransform::GetSize,   &RectTransform::SetSize)
        .property("Pivot",  &RectTransform::GetPivot,  &RectTransform::SetPivot)

        .property("AnchorMin", &RectTransform::GetAnchorMin, &RectTransform::SetAnchorMin)
        .property("AnchorMax", &RectTransform::GetAnchorMax, &RectTransform::SetAnchorMax)
        .property("AnchoredPos", &RectTransform::GetAnchoredPos, &RectTransform::SetAnchoredPos)
        .property("SizeDelta", &RectTransform::GetSizeDelta, &RectTransform::SetSizeDelta);
}

void RectTransform::OnUpdate(float delta)
{
    // NOTE : Transform이랑 동일함.
    if (parent && parent->GetOwner()->IsDestory())
    {
        RemoveSelfAtParent();
        parent = nullptr;
        dirty = true;
        SetChildrenDirty();
    }

    UpdateMatricesIfDirty();
}

void RectTransform::Enable_Inner()
{
    auto ptr = ObjectSystem::Instance().Get<RectTransform>(handle);
    ScriptSystem::Instance().Register(ptr);
    UIManager::Instance().Register(ptr);
    OnEnable();
}

void RectTransform::Disable_Inner()
{
    auto ptr = ObjectSystem::Instance().Get<RectTransform>(handle);
    ScriptSystem::Instance().UnRegister(ptr);
    UIManager::Instance().UnRegister(ptr);
    OnDisable();
}

nlohmann::json RectTransform::Serialize()
{
    // transform 내용 저장 -> rect가 가진 transform 
    return JsonHelper::MakeSaveData(this);
}

void RectTransform::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
    SetChildrenDirty();
}

const Vector3& RectTransform::GetPos() const
{
    return pos;
}

void RectTransform::SetPos(const Vector3& vec)
{
    pos = vec;
    SetChildrenDirty();
    dirty = true;
}

const Vector2& RectTransform::GetSize() const
{
    return size;
}

void RectTransform::SetSize(const Vector2& vec)
{
    size = vec;
    SetChildrenDirty();
    dirty = true;
}

const Vector2& RectTransform::GetPivot() const
{
    return pivot;
}

void RectTransform::SetPivot(const Vector2& vec)
{
    float x = std::clamp(vec.x, 0.0f, 1.0f);
    float y = std::clamp(vec.y, 0.0f, 1.0f);

    pivot = { x, y };
    SetChildrenDirty();
    dirty = true;
}

void RectTransform::SetChildrenDirty()
{
    auto ownerChildren = GetOwner()->GetTransform()->GetChildren();
    for (auto& child : ownerChildren)
    {
        child->SetDirty();
        auto rect = child->GetOwner()->GetComponent<RectTransform>();
        if (rect)
        {
            rect->SetDirty();
            rect->SetChildrenDirty();
        }
    }
}

Matrix RectTransform::RemoveScale(Matrix& m)
{
    Vector3 s, t;
    Quaternion q;
    m.Decompose(s, q, t); // scale, rotation, translation

    Matrix R = Matrix::CreateFromQuaternion(q);
    Matrix T = Matrix::CreateTranslation(t);
    return R * T; // (scale 제외)
}

void RectTransform::UpdateMatricesIfDirty()
{
    if (!dirty) return;

    auto& r = GetEuler();

    // 1) 부모 크기(REF)
    Vector2 parentSize;
    auto parentTran = GetOwner()->GetParent();
    RectTransform* parentRect = nullptr;

    if (parentTran) parentRect = parentTran->GetOwner()->GetComponent<RectTransform>();
    // 위 줄은 실제 반환 타입에 맞게 수정 필요(지금은 shared_ptr인지 raw인지 불명)

    if (parentRect)
        parentSize = parentRect->GetRectSizeRef();   // 아래에 추가할 함수
    else
        parentSize = Vector2((float)UIManager::Instance().refW, (float)UIManager::Instance().refH);

    // 2) 앵커 영역
    Vector2 aMin(parentSize.x * anchorMin.x, parentSize.y * anchorMin.y);
    Vector2 aMax(parentSize.x * anchorMax.x, parentSize.y * anchorMax.y);
    Vector2 aSize(aMax.x - aMin.x, aMax.y - aMin.y);

    // 3) 내 Rect 크기(REF)
    Vector2 rectSizeRef(aSize.x + sizeDelta.x, aSize.y + sizeDelta.y);

    // 4) pivot 기준 위치(REF)
    Vector2 aCenter((aMin.x + aMax.x) * 0.5f, (aMin.y + aMax.y) * 0.5f);
    Vector2 pivotPosRef(aCenter.x + anchoredPos.x, aCenter.y + anchoredPos.y);

    // 5) 로컬 매트릭스 (REF 공간)
    Matrix T0 = Matrix::CreateTranslation(-pivot.x, -pivot.y, 0.0f);
    Matrix S = Matrix::CreateScale({ rectSizeRef.x, rectSizeRef.y, 1.0f });
    Matrix Rm = Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
    Matrix T1 = Matrix::CreateTranslation({ pivotPosRef.x, pivotPosRef.y, 0.0f });

    localMatrix = T0 * S * Rm * T1;

    // 6) 월드 결합 (REF 공간)
    if (!parentTran)
        worldMatrix = localMatrix;
    else
    {
        if (parentRect)
        {
            Matrix parentNoScale = RemoveScale(parentRect->GetWorldMatrix());
            worldMatrix = localMatrix * parentNoScale;
        }
        else
        {
            worldMatrix = localMatrix * parentTran->GetWorldMatrix();
        }
    }

    // 7) 기존 size/pos 호환 값 갱신(선택)
    size = rectSizeRef;
    pos = Vector3(pivotPosRef.x, pivotPosRef.y, pos.z);

    dirty = false;
}

Vector2 RectTransform::GetRectSizeRef() const
{
    return size;
}