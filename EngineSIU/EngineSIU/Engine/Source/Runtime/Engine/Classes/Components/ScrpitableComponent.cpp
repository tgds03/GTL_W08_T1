#include "GameFramework/Actor.h"
#include "ScrpitableComponent.h"

UScrpitableComponent::UScrpitableComponent()
{
}

void UScrpitableComponent::BeginPlay()
{
    UActorComponent::BeginPlay();

    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    Environment = sol::environment(lua, sol::create, lua.globals());
    Environment["obj"] = GetOwner();

    LoadScriptAndBind();

    if (EventFunc.BeginPlay.valid())
        EventFunc.BeginPlay();
}

void UScrpitableComponent::TickComponent(float DeltaTime)
{
    UActorComponent::TickComponent(DeltaTime);

    if (EventFunc.Tick.valid())
        EventFunc.Tick(DeltaTime);
}

void UScrpitableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (EventFunc.EndPlay.valid())
        EventFunc.EndPlay(EndPlayReason);
}

void UScrpitableComponent::LoadScriptAndBind()
{
    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    lua.script_file(GetData((lua["SCRIPT_PATH"] + ScriptName)), Environment);

    EventFunc.BeginPlay = Environment["BeginPlay"];
    EventFunc.Tick = Environment["Tick"];
    EventFunc.EndPlay = Environment["EndPlay"];
    EventFunc.OnOverlap = Environment["OnOverlap"];
}
