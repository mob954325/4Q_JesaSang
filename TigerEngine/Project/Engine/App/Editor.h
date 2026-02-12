#pragma once
#include "pch.h"
#include "System/InputSystem.h"
#include "../Object/GameObject.h"
#include "../../Externals/imguizmo/ImGuizmo.h"
#include <memory>
#include <unordered_map>
#include <deque>

struct PrefabEntry {
    std::string name;
    std::vector<std::string> jsons;
};

class Editor : public InputProcesser
{
public:
    void GetScreenSize(int width, int height);
    void Initialize(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    void GetDSV(const ComPtr<ID3D11DepthStencilView>& dsv) { depthStencliView = dsv; };
    void GetRTV(const ComPtr <ID3D11RenderTargetView>& rtv) { renderTargetView = rtv; };

    void Update();
    void Render(HWND& hwnd);
    void RenderEnd(const ComPtr<ID3D11DeviceContext>& context);

    void SelectObject(GameObject* obj);

    void ReleaseBackBufferResources();
    void CreatePickingStagingTex();

private:
    // Menu bar
    void RenderMenuBar(HWND& hwnd);
    void RenderPlayModeControls();
    void RenderWorldSettings();
    void RenderShadowMap();
    void RenderWorldManager();
    void SaveCurrentScene(HWND& hwnd);
    void LoadScene(HWND& hwnd);

    // Hierarchy
    void RenderHierarchy();
    void DrawHierarchyNode(GameObject* obj);
    void DrawHierarchyDropSpace();
    void RenderPrefabWindow(HWND& hwnd);
    std::string MakeUniquePrefabName(const std::string& base, const std::vector<PrefabEntry>& list);

    // 기존 함수 시그니처 변경(내부에서만 쓰는 preserveIds 플래그 추가)
    GameObject* InstantiatePrefabFromJson(const std::vector<std::string>& jsonStr, Transform* parent, bool preserveIds);
    GameObject* InstantiatePrefabFromJson(const std::vector<std::string>& jsonStr, Transform* parent); // 기존 호환용 wrapper

    void CollectSubtree(GameObject* root, std::vector<std::string>& out);
    bool SavePrefabToJson(HWND& hwnd, PrefabEntry& data, const char* filePath);
    void LoadPrefabsFromFolder(const std::string& folder);
    bool LoadPrefabFromJsonFile(const std::string& filepath, PrefabEntry& outPrefab);

    // inspector
    void RenderInspector();
    void DrawAddComponentPopup(GameObject* obj);

    // Debug Draw
    void RenderCameraFrustum();
    void RenderDebugAABBDraw();
    void RenderDebugGrid();
    void RenderDebugVision();

    // Reender RTTR
    template<typename T>
    void RenderComponentInfo(std::string name, T* comp);

    // Gizmo
    void RenderGizmoSettings();
    void RenderWorldGrid();
    void RenderGizmo();
    void ApplyGizmoToTransform(Transform* transform, const Matrix& worldMatrix);
    
    GameObject* selectedObject; // 현재 inspector 정보를 보고 있는 게임 오브젝트

    // camera info
    Matrix cameraView{};
    Matrix cameraProjection{};

    // screen info
    int screenWidth = 0;
    int screenHeight = 0;

    // debug bind
    ComPtr<ID3D11Device>                device{};
    ComPtr<ID3D11DeviceContext>         context{};
    ComPtr<ID3D11InputLayout>           inputLayout{};
    ComPtr<ID3D11RenderTargetView>      renderTargetView{};
    ComPtr<ID3D11DepthStencilView>      depthStencliView{};
    ComPtr<ID3D11RasterizerState>       rasterizerState{};

    // flags
    bool isDiretionalLightDebugOpen = false;
    bool isWorldSettingOpen = false;
    bool isPhysicsDebugOpen = true;

    std::string currScenePath{};

    // object picking
    bool isMouseLeftClick = false;
    XMINT2 mouseXY{};
    uint32_t currPickedID = -1;
    ComPtr<ID3D11Texture2D> coppedPickingTex{};

    void CheckObjectPicking();
    bool isAABBPicking = false;

    // rttr read
    void ReadVariants(rttr::variant& var);
    void ReadVariants(rttr::instance inst);

    // prefabs
    std::vector<PrefabEntry> prefabs;
    int selectedPrefabIndex = -1;
    bool isPrefabWindowOpen = true;

    // Check Key
    void CheckObjectDeleteKey();
    bool isHierarchyFocused = false;

    // check camerainfo
    void RenderCameraPanel();
    bool isCameraPanelOepn = false;
    // Gizmo state
    bool isGizmoEnabled = true;
    bool isWorldGridEnabled = true;
    bool useGizmoSnap = false;
    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE gizmoMode = ImGuizmo::WORLD;
    Vector3 snapTranslation = Vector3(1.0f, 1.0f, 1.0f);
    float snapRotation = 15.0f; // degrees
    float snapScale = 0.1f;
    float worldGridSize = 10.0f;
    // =========================
    // Undo / Redo System
    // =========================
private:
    struct ICommand
    {
        virtual ~ICommand() = default;
        virtual void Undo(Editor& ed) = 0;
        virtual void Redo(Editor& ed) = 0;
    };

    struct GizmoTransformCommand : ICommand
    {
        uint32_t objectId;
        Vector3 beforePos, afterPos, beforeScale, afterScale;
        Quaternion beforeRot, afterRot;
        void Undo(Editor&) override; void Redo(Editor&) override;
    };

    // Transform 변경(이동/회전/스케일)
    struct TransformEditCommand : ICommand
    {
        uint32_t objectId = 0;
        DirectX::SimpleMath::Vector3 before{};
        DirectX::SimpleMath::Vector3 after{};
        enum class Kind { Position, RotationRad, Scale } kind = Kind::Position;

        void Undo(Editor& ed) override;
        void Redo(Editor& ed) override;
    };

    // 계층(Parent) 변경
    struct ReparentCommand : ICommand
    {
        uint32_t objectId = 0;
        int oldParentId = -1; // -1 means root
        int newParentId = -1;

        void Undo(Editor& ed) override;
        void Redo(Editor& ed) override;
    };

    // 생성/제거: 서브트리 JSON 스냅샷으로 복원
    struct CreateDeleteCommand : ICommand
    {
        // create: redo=생성, undo=삭제
        // delete: undo=복원, redo=삭제
        bool isCreate = true;

        // 루트 오브젝트 ID(원래 있던 ID를 그대로 쓰는 방식)
        int parentId = -1; // -1 root
        std::vector<std::string> subtreeJsons;
        uint32_t representativeRootId = 0; // undo/redo 시 대표 루트를 찾기 위한 값

        void Undo(Editor& ed) override;
        void Redo(Editor& ed) override;
    };

    void PushCommand(std::unique_ptr<ICommand> cmd);
    void Undo();
    void Redo();

    // 유틸
    GameObject* FindGameObjectById(uint32_t id);
    void ApplyReparent(uint32_t objId, int parentId);
    void ApplyTransform(uint32_t objId, TransformEditCommand::Kind kind, const DirectX::SimpleMath::Vector3& v);

    // 생성/삭제 유틸
    void DestroyObjectById(uint32_t id);
    void DestroyObjectWithUndo(GameObject* victim);

    // 트랜스폼 편집 세션(드래그 시작~끝)
    struct TransformEditSession
    {
        DirectX::SimpleMath::Vector3 start{};
        TransformEditCommand::Kind kind{};
        bool active = false;
    };
    std::unordered_map<uintptr_t, TransformEditSession> transformSessions; // key: (comp ptr ^ kind)
    uintptr_t MakeTransformSessionKey(void* compPtr, TransformEditCommand::Kind kind) const;

    // stacks
    std::vector<std::unique_ptr<ICommand>> undoStack;
    std::vector<std::unique_ptr<ICommand>> redoStack;
    size_t maxHistory = 200;

    // === etc ==
    void RenderVariantDebugger();   // 각종 디버그 보여주는 창 ( 아무거나 ) 
    bool isOpenVariantDebugger = false;

public:
    void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
        const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;
};
