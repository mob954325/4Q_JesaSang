#pragma once
#pragma once
#include <vector>
#include <functional>
#include "../../Base/Entity/Object.h"

template<typename... Args>
class MultiDelegate {
    struct Slot
    {
        Object* instance;				    // 콜백 구분용 포인터	(GameObject, Component, Scene)
        std::function<void(Args...)> func;	// 콜백 함수 리스트
    };
    std::vector<Slot> slots;

public:
    // Add Function
    void AddListener(Object* instance, const std::function<void(Args...)>& f)
    {
        slots.push_back({ instance, f });
    }

    // Remove Function (등록한 instacne 활용)
    void Remove(Object* instance)
    {
        slots.erase(std::remove_if(slots.begin(), slots.end(),
            [instance](const Slot& s) { return s.instance == instance; }),
            slots.end());
    }

    // Claer Functions
    void Clear() { slots.clear(); }

    // 등록된 Functions Call
    void Invoke(Args... args) const
    {
        for (const auto& s : slots)
        {
            // Handle로 테이블에서 유효한지 검사
            if (ObjectTable::Get().IsValid(s.instance))
                s.func(args...);
        }
    }
};
