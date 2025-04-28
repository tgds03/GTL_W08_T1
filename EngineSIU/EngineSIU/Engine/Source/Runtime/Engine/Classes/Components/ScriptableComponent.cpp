#include "GameFramework/Actor.h"
#include "ScriptableComponent.h"

UScriptableComponent::UScriptableComponent()
{
    //ScriptName = "Saved/LuaScripts/template.lua";
}

UObject* UScriptableComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->ScriptName = ScriptName;

    return NewComponent;
}

void UScriptableComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    OutProperties.Add(TEXT("ScriptName"), ScriptName);
}

void UScriptableComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("ScriptName"));
    if (TempStr)
    {
        ScriptName = *TempStr;
    }
}

void UScriptableComponent::BeginPlay()
{
    UActorComponent::BeginPlay();

    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    
    LoadScriptAndBind();

    if (EventFunc.BeginPlay.valid())
    {
        sol::protected_function_result Result = EventFunc.BeginPlay();
        LogIfErrorExsist("BeginPlay", Result);
    }
}

void UScriptableComponent::TickComponent(float DeltaTime)
{
    UActorComponent::TickComponent(DeltaTime);
    
    if (EventFunc.Tick.valid())
    {
        sol::protected_function_result Result = EventFunc.Tick(DeltaTime);
        LogIfErrorExsist("Tick", Result);
    }
}

void UScriptableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    
    if (EventFunc.EndPlay.valid())
    {
        sol::protected_function_result Result = EventFunc.EndPlay(EndPlayReason);
        LogIfErrorExsist("EndPlay", Result);
    }
}

void UScriptableComponent::LoadScriptAndBind()
{
    if (!FEngineLoop::ScriptSys.LoadScripts.Contains(ScriptName))
    {
        UE_LOG(LogLevel::Error, "Can not find %s", GetData(ScriptName));
        return;
    }
    
    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    if (!Environment.valid())
        Environment = sol::environment(lua, sol::create, lua.globals());
    Environment["obj"] = GetOwner();
    
    std::string scriptText = FEngineLoop::ScriptSys.LoadScripts[ScriptName];
    sol::load_result loadresult = lua.load_buffer(scriptText.c_str(), scriptText.length());
    sol::function script = loadresult;
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
        } else
        {
            LogIfErrorExsist("", res);
        }
    } else if (script)
    {
        sol::error err = loadresult;
        UE_LOG(LogLevel::Error, "Can not execute %s@%s", GetData(ScriptName), err.what());
    } else
    {
        UE_LOG(LogLevel::Error, "Can not find %s", GetData(ScriptName));
    }
}

void UScriptableComponent::LogIfErrorExsist(FString funcName, sol::protected_function_result& Result)
{
    if (!Result.valid())
    {
        if (Result.get_type() == sol::type::string) {
            sol::error err = Result;
            UE_LOG(LogLevel::Error, "Failed to call %s@%s", GetData(funcName), err.what());
        } else
        {
            UE_LOG(LogLevel::Error, "Failed to call %s with non-string error", GetData(funcName));
        }
        
    }
}
