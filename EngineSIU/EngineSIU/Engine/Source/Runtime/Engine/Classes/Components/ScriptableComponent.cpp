#include "GameFramework/Actor.h"
#include "ScriptableComponent.h"

extern FEngineLoop GEngineLoop;

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
        LogIfErrorExist("BeginPlay", Result);   
    }

    // Key down 이벤트 델리게이트 등록
    auto* Handler = GEngineLoop.GetAppMessageHandler();


    // NOTCIE : Add 순서 HandleType 키워드 순서에 맞게 해줘야 함.
    if (EventFunc.OnKeyDown.valid())
    {
        FDelegateHandle KeyDownHandle = Handler->OnKeyDownDelegate.AddLambda([this](const FKeyEvent& KeyEvent)
        {
                sol::protected_function_result Result = EventFunc.OnKeyDown(KeyEvent.GetKey());
                LogIfErrorExist("OnKeyDown", Result);

        });

        InputHandlers.Add(KeyDownHandle);


    }
    if (EventFunc.OnKeyUp.valid())
    {
        FDelegateHandle KeyUpHandle = Handler->OnKeyUpDelegate.AddLambda([this](const FKeyEvent& KeyEvent)
        {
            sol::protected_function_result Result = EventFunc.OnKeyUp(KeyEvent.GetKey());
            LogIfErrorExist("OnKeyUp", Result);

        });

        InputHandlers.Add(KeyUpHandle);
    }
    if (EventFunc.OnMouseDown.valid())
    {
        FDelegateHandle MouseDownHandle = Handler->OnMouseDownDelegate.AddLambda([this](const FPointerEvent& PointerEvent)
        {
            sol::protected_function_result Result = EventFunc.OnMouseDown(PointerEvent.GetEffectingButton());
            LogIfErrorExist("OnMouseDown", Result);
        });
        InputHandlers.Add(MouseDownHandle);
    }
    if (EventFunc.OnMouseMove.valid())
    {
        FDelegateHandle MouseMoveHandle = Handler->OnMouseMoveDelegate.AddLambda([this](const FPointerEvent& PointerEvent)
        {
            sol::protected_function_result Result = EventFunc.OnMouseMove(
                PointerEvent.GetScreenSpacePosition().X, PointerEvent.GetScreenSpacePosition().Y);
            LogIfErrorExist("OnMouseMove", Result);
        });
        InputHandlers.Add(MouseMoveHandle);
    }
}

void UScriptableComponent::TickComponent(float DeltaTime)
{
    UActorComponent::TickComponent(DeltaTime);
    
    if (EventFunc.Tick.valid())
    {
        sol::protected_function_result Result = EventFunc.Tick(DeltaTime);
        LogIfErrorExist("Tick", Result);
    }
}

void UScriptableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    auto* Handler = GEngineLoop.GetAppMessageHandler();

    for (int i = 0; i < InputHandlers.Num(); i++) {
        if (InputHandlers[i].IsValid())
        {
            switch (i) {
            case static_cast<int>(HandlerType::KeyDown):
                Handler->OnKeyDownDelegate.Remove(InputHandlers[i]);
                break;
            case static_cast<int>(HandlerType::KeyUp):
                Handler->OnKeyUpDelegate.Remove(InputHandlers[i]);
                break;
            case static_cast<int>(HandlerType::MouseDown):
                Handler->OnMouseDownDelegate.Remove(InputHandlers[i]);
                break;
            case static_cast<int>(HandlerType::MouseMove):
                Handler->OnMouseMoveDelegate.Remove(InputHandlers[i]);
                break;
            }
        }
    }

    InputHandlers.Empty();

    if (EventFunc.EndPlay.valid())
    {
        sol::protected_function_result Result = EventFunc.EndPlay(EndPlayReason);
        LogIfErrorExist("EndPlay", Result);
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
            EventFunc.OnKeyDown = Environment["OnKeyDown"];
            EventFunc.OnKeyUp = Environment["OnKeyUp"];
            EventFunc.OnMouseDown = Environment["OnMouseDown"];
            EventFunc.OnMouseMove = Environment["OnMouseMove"];
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

void UScriptableComponent::LogIfErrorExist(FString funcName, sol::protected_function_result& Result)
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
