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
    static TMap<FString, std::function<void(sol::usertype<TClass>)>>& BindFunctions() { \
        static TMap<FString, std::function<void(sol::usertype<TClass>)>> _binds; \
        return _binds; \
    } \
public: \
    using InheritTypes = SolTypeBinding::InheritList<TClass, TSuperClass>::type; \
    static sol::usertype<TClass> GetLuaUserType(sol::state& lua) { \
        static sol::usertype<TClass> usertype = lua.new_usertype<TClass>( \
            #TClass, \
            sol::base_classes, \
            SolTypeBinding::TypeListToBases<typename SolTypeBinding::InheritList<TClass, TSuperClass>::base_list>::Get() \
        ); \
        return usertype; \
    } \
    static void BindPropertiesToLua(sol::state& lua) { \
        sol::usertype<TClass> table = GetLuaUserType(lua); \
        for (const auto [name, bind] : BindFunctions()) \
        { \
            bind(table); \
        } \
        SolTypeBinding::RegisterGetComponentByClass<TClass>(lua, #TClass); \
    } \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass; \



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
            }, \
            TClass::BindPropertiesToLua \
        }; \
        return &ClassInfo; \
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
            []() -> UObject* { return nullptr; }, \
            TClass::BindPropertiesToLua \
        }; \
        return &ClassInfo; \
    } \


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
    } VarName##_PropRegistrar_{}; \

#define UFUNCTION(Type, FuncName, ...) \
    Type FuncName (__VA_ARGS__); \
    inline static struct FuncName##_PropRegister \
    { \
        FuncName##_PropRegister() \
        { \
            BindFunctions().Add(#FuncName, [](sol::usertype<ThisClass> table) { \
                table[#FuncName] = &ThisClass::FuncName; \
            }); \
        } \
    } FuncName##_PropRegister_{}; \

#define UFUNCTION_CONST(Type, FuncName, ...) \
    Type FuncName (__VA_ARGS__) const; \
    inline static struct FuncName##_PropRegister \
    { \
        FuncName##_PropRegister() \
        { \
            BindFunctions().Add(#FuncName, [](sol::usertype<ThisClass> table) { \
                table[#FuncName] = &ThisClass::FuncName; \
            }); \
        } \
    } FuncName##_PropRegister_{}; \
    
