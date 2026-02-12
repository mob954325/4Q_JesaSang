#pragma once
#include "System/Singleton.h"
#include <queue>

class Component; // 순환 참조 방지

/// <summary>
/// 렌더링을 하지 않는 단순 컴포넌트를 상속받은 컴포넌트들을 관리합니다.
/// 객체의 생명주기는 관리하지 않습니다.
/// </summary>
/// <remarks>
/// 등록 흐름 
///     등록 호출 -> pending에 삽입 -> 매 프레임마다 pending에서 각 컨테이너로 추가 후 pending clear
///     제거 호출 -> removeal에 삽입 -> 매 마지막 (LateUpdate) 에서 removals에 있는 컴포넌트 제거
/// </remkars>
class ScriptSystem : public Singleton<ScriptSystem>
{
public:
    ScriptSystem(token) {}
    ~ScriptSystem() = default;

    /// <summary>
    /// readyQueue에 있는 컴포넌트들의 OnStart 실행 함수
    /// </summary>
    void CheckReadyQueue();

    /// <summary>
    /// 일반 컴포넌트 등록
    /// </summary>
    void Register(Component* comp);

    /// <summary>
    /// 해당 컴포넌트 등록 해제
    /// </summary>
    void UnRegister(Component* comp);

    /// <summary>
    /// 스크립트 컴포넌트 등록
    /// </summary>
    /// <param name="comp">ScriptComponent 객체</param>
    void RegisterScript(Component* comp);

    /// <summary>
    /// 스크립트 컴포넌트 등록 해제
    /// </summary>
    /// <param name="comp">ScriptComponent 객체</param>
    void UnRegisterScript(Component* comp);

    /// <summary>
    /// 등록된 컴포넌트 호출
    /// </summary>
    void Update(float delta);

    /// <summary>
    /// 등록된 컴포넌트 호출
    /// </summary>
    void FixedUpdate(float dt);

    /// <summary>
   /// 등록된 컴포넌트 호출
   /// </summary>
    void LateUpdate(float dt);

    /// <summary>
    /// 모든 컨테이너 비우는 함수
    /// </summary>
    void Clear();

private:
    // === 일반 Component ===
    /// <summary>
    /// 등록된 컴포넌드 목록
    /// </summary>
    std::vector<Component*> comps{};

    /// <summary>
    /// OnStart 실행하지 않는 컴포넌트 모음 
    /// </summary>
    std::queue<Component*> readyQueue{};

    // === Script Component ===
    /// <summary>
    /// Play모드에만 돌아가는 스크립트 컴포넌트 목록
    /// </summary>
    std::vector<Component*> scriptComps{};

    /// <summary>
    /// component 지연 등록을 위한 펜딩 벡터 
    /// </summary>
    std::vector<Component*> pending_components{};

    /// <summary>
    /// script component 지연 등록을 위한 펜딩 벡터 
    /// </summary>
    std::vector<Component*> pending_scriptComponents{};

    /// <summary>
    /// 제거될 예정인 컴포넌트 모음 
    /// </summary>
    std::vector<Component*> pending_scriptRemovals{};

    void SwapErase(std::vector<Component*>& comps, Component* target);
    void ProcessRemovals();
};
