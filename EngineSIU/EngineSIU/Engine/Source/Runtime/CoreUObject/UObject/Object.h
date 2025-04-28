#pragma once
#include "EngineLoop.h"
#include "NameTypes.h"
#include "sol/sol.hpp"

// for sol2 typing
template<typename... Types>
struct TypeList {};

// push type
template<typename List, typename NewType>
struct PushBack;

template<typename... Types, typename NewType>
struct PushBack<TypeList<Types...>, NewType> {
    using type = TypeList<Types..., NewType>;
};

template<typename List, typename NewType>
using PushBack_t = typename PushBack<List, NewType>::type;

// pop type
template<typename List>
struct PopBack;

template<typename T, typename... Rest>
struct PopBack<TypeList<T, Rest...>> {
private:
    template<typename... Accum>
    struct Helper;

    template<typename Head, typename... Tail, typename... Accum>
    struct Helper<TypeList<Head, Tail...>, Accum...> {
        using type = typename Helper<TypeList<Tail...>, Accum..., Head>::type;
    };

    template<typename Last, typename... Accum>
    struct Helper<TypeList<Last>, Accum...> {
        using type = TypeList<Accum...>; // 마지막은 버린다
    };

public:
    using type = typename Helper<TypeList<Rest...>, T>::type;
};

template<typename List>
using PopBack_t = typename PopBack<List>::type;

// unpack types
template<typename TypeList>
struct TypeListToBases {
    static auto Get() {
        using BasesOnly = PopBack_t<TypeList>;
        return TypeListToBases<BasesOnly>::Get();
    }
};

template<typename... Types>
struct TypeListToBases<TypeList<Types...>> {
    static auto Get() {
        return sol::bases<Types...>();
    }
};



extern FEngineLoop GEngineLoop;
class UClass;
class UWorld;
class AActor;

class UObject
{
    friend class AActor;
private:
    UObject(const UObject&) = delete;
    UObject& operator=(const UObject&) = delete;
    UObject(UObject&&) = delete;
    UObject& operator=(UObject&&) = delete;

protected:
   
    // Base 클래스를 상속하는 모든 타입을 리스트로 모은다
    template<typename Derived, typename Base = void>
    struct InheritList;

    // Base 없는 경우 (Root 클래스)
    template<typename Derived>
    struct InheritList<Derived, void> {
        using type = TypeList<Derived>;
    };
 
public:
    using Super = UObject;
    using ThisClass = UObject;

    static UClass* StaticClass();

private:
    friend class FObjectFactory;
    friend class FSceneMgr;
    friend class UClass;

    uint32 UUID;
    uint32 InternalIndex; // Index of GUObjectArray

    FName NamePrivate;
    UClass* ClassPrivate = nullptr;
    UObject* OuterPrivate = nullptr;

    
    // FName을 키값으로 넣어주는 컨테이너를 모두 업데이트 해야합니다.
    void SetFName(const FName& InName) { NamePrivate = InName; }

public:
    UObject();
    virtual ~UObject() = default;

    virtual UObject* Duplicate(UObject* InOuter);

    UObject* GetOuter() const { return OuterPrivate; }
    virtual UWorld* GetWorld() const;
    virtual void Serialize(FArchive& Ar);

    FName GetFName() const { return NamePrivate; }
    FString GetName() const { return NamePrivate.ToString(); }


    uint32 GetUUID() const { return UUID; }
    uint32 GetInternalIndex() const { return InternalIndex; }

    UClass* GetClass() const { return ClassPrivate; }


    /** this가 SomeBase인지, SomeBase의 자식 클래스인지 확인합니다. */
    bool IsA(const UClass* SomeBase) const;

    template <typename T>
        requires std::derived_from<T, UObject>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }

public:
    void* operator new(size_t size)
    {
        UE_LOG(LogLevel::Display, "UObject Created : %d", size);

        void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(size);
        UE_LOG(
            LogLevel::Display,
            "TotalAllocationBytes : %d, TotalAllocationCount : %d",
            FPlatformMemory::GetAllocationBytes<EAT_Object>(),
            FPlatformMemory::GetAllocationCount<EAT_Object>()
        );
        return RawMemory;
    }

    void operator delete(void* ptr, size_t size)
    {
        UE_LOG(LogLevel::Display, "UObject Deleted : %d", size);
        FPlatformMemory::Free<EAT_Object>(ptr, size);
    }

    FVector4 EncodeUUID() const {
        FVector4 result;

        result.X = UUID % 0xFF;
        result.Y = UUID >> 8 & 0xFF;
        result.Z = UUID >> 16 & 0xFF;
        result.W = UUID >> 24 & 0xFF;

        return result;
    }
};
