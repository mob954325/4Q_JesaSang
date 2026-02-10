#include "RectTransform.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"

RTTR_REGISTRATION
{
        rttr::registration::class_<RectTransform>("RectTransform") // NOTE : 로드시 rect가 두 개임 ??
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("Pos",    &RectTransform::GetPos,    &RectTransform::SetPos)
        .property("Size",   &RectTransform::GetSize,   &RectTransform::SetSize)
        .property("Pivot",  &RectTransform::GetPivot,  &RectTransform::SetPivot);
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

    Matrix T0 = Matrix::CreateTranslation(-pivot.x, -pivot.y, 0.0f);
    Matrix S = Matrix::CreateScale({ size.x, size.y, 1.0f });         // size = px
    Matrix R = Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
    Matrix T1 = Matrix::CreateTranslation({ pos.x, pos.y, 0.0f });

    localMatrix = T0 * S * R * T1;

    auto parentTran = GetOwner()->GetParent();
    if (!parentTran)
    {
        worldMatrix = localMatrix;
    }
    else
    {
        auto parentRect = parentTran->GetOwner()->GetComponent<RectTransform>();
        if (parentRect)
        {
            Matrix parentNoScale = RemoveScale(parentRect->GetWorldMatrix());
            worldMatrix = localMatrix * parentNoScale;   // 부모 스케일 미상속
        }
        else
        {
            // 부모가 일반 Transform이면 기존대로(원하면 이것도 NoScale 적용 가능)
            worldMatrix = localMatrix * parentTran->GetWorldMatrix();
        }
    }

    dirty = false;
}