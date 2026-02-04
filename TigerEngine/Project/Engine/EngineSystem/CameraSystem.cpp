#include "CameraSystem.h"
#include "../Object/GameObject.h"
#include "../Components/Camera.h"


CameraSystem::CameraSystem(token)
{
    lightCamera = new DirectionalShadowCamera();
}

CameraSystem::~CameraSystem()
{
    delete lightCamera;
}

Camera *CameraSystem::GetFreeCamera()
{
    return freeCamera;
}

void CameraSystem::Register(Camera *cam)
{
    registered.push_back(cam);
    mappedRegistered[mappedRegistered.size()] = cam;
    cout << cam->GetName() << "cam register\n";
}

void CameraSystem::RemoveCamera(Camera *cam)
{
    if (cam->GetOwner()->GetName() == "FreeCamera") return; // NOTE : Editor 카메라는 무시한다.

    // vector에서 제거
    for(auto it = registered.begin(); it != registered.end();)
    {
        if(*it == cam)
        {
            if (registered.size() == 1)
            {
                registered.pop_back();
            }
            else
            {
                auto vback = registered.back();
                registered.back() = *it;
                *it = vback;
                registered.pop_back();
            }
            break;
        }
        it++;
    }

    // map에서 제거
    for (auto it = mappedRegistered.begin(); it != mappedRegistered.end(); it++)
    {
        if (it->second == cam)
        {
            mappedRegistered.erase(it);
            break;
        }
    }
}

int CameraSystem::SetCurrCamera(int index)
{
    if (index < 0 || index > registered.size()) // 잘못된 인덱스 선택 방지
    {
        currCameraIndex = 0;
    }
    else
    {
        currCameraIndex = index;
    }

    return currCameraIndex;
}

Camera* CameraSystem::GetCurrCamera()
{
    return registered[currCameraIndex];
}

Camera *CameraSystem::GetCameraByIndex(int index)
{
    if(auto it = mappedRegistered.find(index); it != mappedRegistered.end())
    {
        return it->second;
    }
    else
    {

        return nullptr;
    }
}

void CameraSystem::Clear()
{
    for (auto& cam : registered)
    {
        RemoveCamera(cam);
    }
}

void CameraSystem::LightCameraUpdate(float delta)
{
    lightCamera->Update(GetCurrCamera());
}

void CameraSystem::SetScreenSize(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
}

void CameraSystem::FreeCameraUpdate(float delta)
{
    if(!freeCamObj) return;
    auto comps = freeCamObj->GetComponents();
    for(auto& comp : comps)
    {
        comp->OnUpdate(delta);
    }
}

#include "../Components/FreeCamera.h"

Handle CameraSystem::CreateFreeCamera(int clientWidth, int clientHeight, Scene *currScene)
{
    Handle handle = ObjectSystem::Instance().Create<GameObject>();
    freeCamObj = ObjectSystem::Instance().Get<GameObject>(handle);
	freeCamObj->SetName("FreeCamera");
    freeCamera = freeCamObj->AddComponent<Camera>();
    auto freecam = freeCamObj->AddComponent<FreeCamera>();

    freeCamObj->Enable_Inner(); // 게임 오브젝트 active
    freeCamera->Enable_Inner(); // 카메라 컴포넌트 active
    freecam->Enable_Inner();    // freeCamera active 

	freeCamera->SetProjection(DirectX::XM_PIDIV2, clientWidth, clientHeight, 0.1, 3000);

    return handle;
}
