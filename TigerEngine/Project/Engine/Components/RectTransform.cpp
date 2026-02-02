#include "RectTransform.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"

RTTR_REGISTRATION
{
        rttr::registration::class_<RectTransform>("RectTransform")
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
    pivot = vec;
    SetChildrenDirty();
    dirty = true;
}

void RectTransform::UpdateMatricesIfDirty()
{
    if (!dirty) return;

    auto& r = GetEuler();
    Matrix T0 = Matrix::CreateTranslation(-pivot.x, -pivot.y, 0.0f);
    Matrix S = Matrix::CreateScale({ size.x, size.y, 1.0f });
    Matrix R = Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
    Matrix T1 = Matrix::CreateTranslation({ pos.x, pos.y, 0 });

    localMatrix = T0 * S * R * T1;

    if (!parent)
    {
        worldMatrix = localMatrix;
    }
    else
    {
        worldMatrix = localMatrix * parent->GetWorldMatrix();
    }

    dirty = false;
}
