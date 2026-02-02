#pragma once
#include "pch.h"
#include "Datas/Mesh.h"
#include "Datas/Bone.h"
#include "Datas/FBXResourceData.h"
#include "Datas/ConstantBuffer.hpp"

class GameObject; // 전방선언

struct RenderItem
{
    // model
    ModelType modelType;
    const Mesh* mesh;
    Material material;
    
    // matrix
    Matrix world;       // static, rigid, skeletal
    Matrix model;       // rigid

    // skeleton
    int boneCount;
    int refBoneIndex;
    const PoseMatrixCB* poses;
    const OffsetMatrixCB* offsets;

    GameObject* objPtr = nullptr; // 해당 렌더 아이템의 게임 오브젝트 포인터
    // ground
    bool isGround;
};

/// <summary>
/// UI 렌더링에 필요한 데이터 모음
/// </summary>
struct UIRenderItem
{
    Color color;        // 색상
    Vector4 uvRect;     // L, R, T, B (px)
    Vector4 params;     // x = type, y = fillAmount;
    Vector4 imageSize;  // (rectW, rectH, texW, texH)
};

/// <summary>
/// RenderQueue는 처리한 렌더 item들을 보관한다.
/// </summary>
class RenderQueue
{
private:
    // Render Queue
    std::vector<RenderItem> opaqueQueue;        // 불투명 오브젝트 -> Deffered Rendering
    std::vector<RenderItem> transparentQueue;   // 투명 오브젝트   -> Forward Rendering
    std::vector<UIRenderItem> uiRenderQueue;    // UI 오브젝트 -> ui 렌더링에 필요한 내용 따로 queue 추가함 - 26.02.02 이성호

public:
    void AddOpaqueQueue(const RenderItem& item)
    {
        opaqueQueue.push_back(item);
    }

    void AddTransparentQueue(const RenderItem& item)
    {
        transparentQueue.push_back(item);
    }

    void AddUIRenderQueue(const UIRenderItem& item)
    {
        uiRenderQueue.push_back(item);
    }

    const auto& GetOpaqueQueue()  const { return opaqueQueue; }
    const auto& GetTransparentQueue()  const { return transparentQueue; }
    const auto& GetUIRenderQueue() const { return uiRenderQueue; }

    void Clear()
    {
        opaqueQueue.clear();
        transparentQueue.clear();
        uiRenderQueue.clear();
    };
};

