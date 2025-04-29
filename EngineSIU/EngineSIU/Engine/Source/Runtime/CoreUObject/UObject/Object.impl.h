#pragma once
#include "Object.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

// 템플릿 함수 구현을 위함.
namespace SolTypeBinding
{
    // Register to AActor::GetComponentByClass
    template <typename T>
        requires (IsCompleteType_v<AActor> && IsCompleteType_v<UActorComponent> && std::derived_from<T, UActorComponent>)
    void RegisterGetComponentByClass(sol::state& lua, std::string className)
    {
        auto wrapper = [](AActor* actor)
        {
            return actor->GetComponentByClass<T>();
        };
        AActor::GetLuaUserType(lua)["Get" + className] = wrapper;
    }

    template <typename T>
    void RegisterGetComponentByClass(sol::state& lua, std::string className) {};
    
}
