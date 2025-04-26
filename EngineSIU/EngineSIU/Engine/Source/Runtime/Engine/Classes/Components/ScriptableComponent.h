#pragma once
#include "ActorComponent.h"
#include "sol/sol.hpp"

struct SolEventFunc
{
    sol::function Tick, BeginPlay, EndPlay;
    sol::function OnOverlap;
};

class UScriptableComponent: public UActorComponent
{
    DECLARE_CLASS(UScriptableComponent, UActorComponent)
public:
    UScriptableComponent();
    virtual UObject* Duplicate(UObject* InOuter) override;
    FString GetScriptName();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(FString, ScriptName)
    
    sol::environment GetEnvironment() { return Environment; }
protected:
    SolEventFunc EventFunc;
private:
    sol::environment Environment;
    void LoadScriptAndBind();
};
