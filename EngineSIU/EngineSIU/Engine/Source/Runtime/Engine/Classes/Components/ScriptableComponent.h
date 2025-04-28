#pragma once
#include "ActorComponent.h"
#include "sol/sol.hpp"
#include "Engine/Classes/Components/ScriptableComponent.h"
#include "Collision/SphereComponent.h"

enum class HandlerType : int
{
    KeyDown,
    KeyUp,
    MouseDown,
    MouseMove,
    InputEnd
};

struct SolEventFunc
{
    sol::protected_function Tick, BeginPlay, EndPlay;
    sol::protected_function OnOverlap;
    sol::protected_function OnKeyDown;  
    sol::protected_function OnKeyUp;    
    sol::protected_function OnMouseDown;
    sol::protected_function OnMouseMove;
};

class UScriptableComponent: public UActorComponent
{
    DECLARE_CLASS(UScriptableComponent, UActorComponent)
public:
    UScriptableComponent();
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(FString, ScriptName)
    
    sol::environment GetEnvironment() { return Environment; }
    SolEventFunc GetEventFunc() { return EventFunc; }

    void GetProperties(TMap<FString, FString>& OutProperties) const;
    void SetProperties(const TMap<FString, FString>& InProperties);

    void LoadScriptAndBind();
    void InitEnvironment();

    // Lua OnOverlap 콜백을 호출하기 위한 C++ 핸들러
    UFUNCTION(void, HandleSphereOverlap, USphereComponent* OverlappedComponent, USphereComponent* OtherComponent)

protected:
    SolEventFunc EventFunc;
private:
    sol::environment Environment;

    void LogIfErrorExist(FString funcName, sol::protected_function_result& Result);

    TMap<HandlerType, TArray<FDelegateHandle>> DelegateHandlers;
};
