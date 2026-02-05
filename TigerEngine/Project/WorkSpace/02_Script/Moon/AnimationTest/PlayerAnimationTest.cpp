#include "PlayerAnimationTest.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "Components/FBXData.h"

REGISTER_COMPONENT(PlayerAnimationTest)
RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerAnimationTest>("PlayerAnimationTest")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

nlohmann::json PlayerAnimationTest::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PlayerAnimationTest::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}



void PlayerAnimationTest::OnInitialize()
{
}

//void PlayerAnimationTest::OnStart()
//{
//    animController = GetOwner()->GetComponent<AnimationController>();
//    fbxData = GetOwner()->GetComponent<FBXData>();
//
//    if (!animController || !fbxData)
//    {
//        std::cout << "[PlayerAnimationTest] AnimController or FbxData is null" << std::endl;
//        return;
//    }
//
//    // 애니메이션 파일 로드 (FBXData, 경로, 애니메이션 이름, loop여부)
//    bool isAnimLoad = FBXResourceManager::Instance().LoadAnimationByPath(fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\Player_Test.fbx", "PlayerTest");
//    if (!isAnimLoad) std::cout << "[PlayerAnimationTest] LoadAnimationByPath is false" << std::endl;
//
//    auto PlayerTestClip = animController->FindClip("PlayerTest");
//    if (!PlayerTestClip)
//    {
//        std::cout << "[PlayerAnimationTest] Clip not found! Name Check!!" << std::endl;
//        return;
//    }
//
//    // 상태 등록
//    animController->AddState(std::make_unique<AnimationState>("PlayerTest", PlayerTestClip, animController));
//
//    // 시작 상태
//    animController->ChangeState("PlayerTest");
//
//    elapsedTime = 0.0f;
//}

void PlayerAnimationTest::OnStart()
{
    animController = GetOwner()->GetComponent<AnimationController>();
    fbxData = GetOwner()->GetComponent<FBXData>();

    if (!animController || !fbxData)
    {
        std::cout << "[PlayerAnimationTest] AnimController or FbxData is null" << std::endl;
        return;
    }

    // 애니메이션 파일 로드 (FBXData, 경로, 애니메이션 이름, loop여부)
    FBXResourceManager::Instance().LoadAnimationByPath(fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\Test_Idle.fbx", "Idle");
    FBXResourceManager::Instance().LoadAnimationByPath(fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\Test_Walk.fbx", "Walk");

    auto idleClip = animController->FindClip("Idle");
    auto walkClip = animController->FindClip("Walk");

    if (!idleClip || !walkClip)
    {
        OutputDebugStringW(L"[CCTTest] Clip not found! 이름 확인 필요\n");
        return;
    }

    // 상태 등록
    animController->AddState(std::make_unique<AnimationState>("Idle", idleClip, animController));
    animController->AddState(std::make_unique<AnimationState>("Walk", walkClip, animController));

    // 시작 상태
    animController->ChangeState("Idle");

    elapsedTime = 0.0f;
}

// 3초 후에 Walk 상태로 전환 
void PlayerAnimationTest::OnUpdate(float delta)
{
    if (!animController) return;

    elapsedTime += delta;

    if (elapsedTime >= 3.0f)
    {
        animController->ChangeState("Walk", 0.2f); // 상태 전환 
    }
}

void PlayerAnimationTest::OnFixedUpdate(float dt)
{

}