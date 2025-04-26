#include "GameFramework/Actor.h"
#include "ScriptableComponent.h"

UScriptableComponent::UScriptableComponent()
{
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
    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    lua.script_file(GetData((lua["SCRIPT_PATH"] + ScriptName)), Environment);

    EventFunc.BeginPlay = Environment["BeginPlay"];
    EventFunc.Tick = Environment["Tick"];
    EventFunc.EndPlay = Environment["EndPlay"];
    EventFunc.OnOverlap = Environment["OnOverlap"];
}
