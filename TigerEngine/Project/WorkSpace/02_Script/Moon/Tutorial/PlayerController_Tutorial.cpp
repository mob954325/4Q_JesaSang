#include "PlayerController_Tutorial.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"

void PlayerController_Tutorial::OnStart() 
{
    //transform = GetOwner()->GetComponent<Transform>();
    //anim = GetOwner()->GetComponent<AnimationController>();
}


void PlayerController_Tutorial::MoveTo(const Vector3& target, float speed, float delta)
{
    //if (!canMove) return;
    //Vector3 dir = target - transform->GetWorldPosition();
    //if (dir.LengthSquared() > 0.001f)
    //{
    //    transform->SetWorldPosition(transform->GetWorldPosition() + dir.Normalized() * speed * delta);
    //    // 바라보는 방향
    //    float targetYaw = atan2f(dir.x, dir.z);
    //    transform->SetEuler(Vector3(0, targetYaw, 0));
    //}
}

void PlayerController_Tutorial::PlayAnimation(const std::string& animName)
{
    //if (anim)
    //    anim->ChangeState(animName);
}