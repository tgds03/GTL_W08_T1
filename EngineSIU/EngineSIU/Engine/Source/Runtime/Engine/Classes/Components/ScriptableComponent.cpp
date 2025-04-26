#include "GameFramework/Actor.h"
#include "ScriptableComponent.h"

UScriptableComponent::UScriptableComponent()
{
    ScriptName = "./Saved/LuaScripts/template.lua";
}

UObject* UScriptableComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->ScriptName = ScriptName;

    return NewComponent;
}

void UScriptableComponent::BeginPlay()
{
    UActorComponent::BeginPlay();

    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    Environment = sol::environment(lua, sol::create, lua.globals());
    Environment["obj"] = GetOwner();

    LoadScriptAndBind();

    if (EventFunc.BeginPlay.valid())
        EventFunc.BeginPlay();
}

void UScriptableComponent::TickComponent(float DeltaTime)
{
    UActorComponent::TickComponent(DeltaTime);

    if (EventFunc.Tick.valid())
        EventFunc.Tick(DeltaTime);
}

void UScriptableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (EventFunc.EndPlay.valid())
        EventFunc.EndPlay(EndPlayReason);
}

void UScriptableComponent::LoadScriptAndBind()
{
    // lua.script_file(GetData((lua["SCRIPT_PATH"] + ScriptName)), Environment);
    sol::function script = FEngineLoop::ScriptSys.LoadScripts[ScriptName];
    if (script && script.valid())
    {
        set_environment(Environment, script);
        sol::protected_function_result res = script();
        if (res.valid())
        {
            EventFunc.BeginPlay = Environment["BeginPlay"];
            EventFunc.Tick = Environment["Tick"];
            EventFunc.EndPlay = Environment["EndPlay"];
            EventFunc.OnOverlap = Environment["OnOverlap"];
        } 
    } else
    {
        UE_LOG(LogLevel::Error, "Can not execute %s", GetData(ScriptName));
    }
}

