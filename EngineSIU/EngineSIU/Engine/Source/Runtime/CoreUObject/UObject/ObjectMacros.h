// ReSharper disable CppClangTidyBugproneMacroParentheses
// ReSharper disable CppClangTidyClangDiagnosticPedantic
#pragma once
#include "Class.h"

// name을 문자열화 해주는 매크로
#define INLINE_STRINGIFY(name) #name


// 공통 클래스 정의 부분
#define __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
private: \
    TClass(const TClass&) = delete; \
    TClass& operator=(const TClass&) = delete; \
    TClass(TClass&&) = delete; \
    TClass& operator=(TClass&&) = delete; \
    inline static struct TClass##_StaticClassRegistrar_ \
    { \
        TClass##_StaticClassRegistrar_() \
        { \
            UClass::GetClassMap().Add(#TClass, ThisClass::StaticClass()); \
        } \
    } TClass##_StaticClassRegistrar_{}; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass;


// RTTI를 위한 클래스 매크로
#define DECLARE_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            TEXT(#TClass), \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { \
                void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(sizeof(TClass)); \
                ::new (RawMemory) TClass; \
                return static_cast<UObject*>(RawMemory); \
            } \
        }; \
        return &ClassInfo; \
    } \
    static TMap<FString, std::function<void(sol::usertype<TClass>)>> BindFunctions() { \
        static TMap<FString, std::function<void(sol::usertype<TClass>)>> _binds; \
        return _binds; \
    } \
    static void BindPropertiesToLua(sol::state& lua) { \
        sol::usertype<TClass> table = lua.new_usertype<TClass>(#TClass, sol::base_classes, sol::bases<TSuperClass>()); \
        for (const auto [name, bind] : BindFunctions()) \
        { \
            bind(table); \
        } \
    } \

// RTTI를 위한 추상 클래스 매크로
#define DECLARE_ABSTRACT_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            TEXT(#TClass), \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { return nullptr; } \
        }; \
        return &ClassInfo; \
    }


#define FIRST_ARG(Arg1, ...) Arg1

/**
 * UClass에 Property를 등록합니다.
 * @param Type 선언할 타입
 * @param VarName 변수 이름
 * @param ... 기본값
 *
 * Example Code
 * ```
 * UPROPERTY
 * (int, Value, = 10)
 * ```
 */
#define UPROPERTY(Type, VarName, ...) \
    Type VarName FIRST_ARG(__VA_ARGS__); \
    inline static struct VarName##_PropRegistrar \
    { \
        VarName##_PropRegistrar() \
        { \
            constexpr int64 Offset = offsetof(ThisClass, VarName); \
            ThisClass::StaticClass()->RegisterProperty( \
                { #VarName, sizeof(Type), Offset } \
            ); \
            BindFunctions().Add(#VarName, [](sol::usertype<ThisClass> table) { \
                table[#VarName] = &ThisClass::VarName; \
            }); \
        } \
    } VarName##_PropRegistrar_{};
