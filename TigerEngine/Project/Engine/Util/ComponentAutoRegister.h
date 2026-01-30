#pragma once
#include "../Manager/ComponentFactory.h"

template<typename T>
struct AutoComponentRegister
{
    explicit AutoComponentRegister(const char* name)
    {
        ComponentFactory::Instance().Register<T>(name, ComponentCategory::Script); // NOTE : Workspace에서 밖에 못쓰기 때문에 script로 카테고리 고정
    }
};

#define REGISTER_COMPONENT(T) \
    static AutoComponentRegister<T> _auto_reg_##T(#T);