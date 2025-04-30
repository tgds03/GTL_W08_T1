#include "GameFramework/Actor.h"
#include "ScriptableComponent.h"
#include "Collision/SphereComponent.h"
#include "World/World.h"
#include "UObject/UObjectIterator.h"
#include "Engine/Camera/SpringArmCameraModifier.h"
#include "Engine/Classes/Engine/PlayerCameraManager.h"

#include "Engine/Engine.h"
#include "Engine/EditorEngine.h"

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
    if (GetWorld()->WorldType == EWorldType::Game || GetWorld()->WorldType == EWorldType::PIE)
        BindDelegate();
}

void UScriptableComponent::BindDelegate()
{
    // OnOverlap이 Lua에 정의되어 있으면
    if (EventFunc.OnOverlap.valid())
    {
        if (USphereComponent* SphereComp = GetOwner()->GetComponentByClass<USphereComponent>())
        {
            FDelegateHandle Handle = SphereComp->OnComponentBeginOverlap.AddDynamic(
                this,
                &UScriptableComponent::HandleSphereOverlap
            );
            DelegateHandlers[HandlerType::Overlap].Add(Handle);
        }
    }

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

        DelegateHandlers[HandlerType::KeyDown].Add(KeyDownHandle);


    }
    if (EventFunc.OnKeyUp.valid())
    {
        FDelegateHandle KeyUpHandle = Handler->OnKeyUpDelegate.AddLambda([this](const FKeyEvent& KeyEvent)
        {
            sol::protected_function_result Result = EventFunc.OnKeyUp(KeyEvent.GetKey());
            LogIfErrorExist("OnKeyUp", Result);

        });

        DelegateHandlers[HandlerType::KeyUp].Add(KeyUpHandle);
    }
    if (EventFunc.OnMouseDown.valid())
    {
        FDelegateHandle MouseDownHandle = Handler->OnMouseDownDelegate.AddLambda([this](const FPointerEvent& PointerEvent)
        {
            sol::protected_function_result Result = EventFunc.OnMouseDown(PointerEvent.GetEffectingButton());
            LogIfErrorExist("OnMouseDown", Result);
        });
        DelegateHandlers[HandlerType::MouseDown].Add(MouseDownHandle);
    }
    if (EventFunc.OnMouseMove.valid())
    {
        FDelegateHandle MouseMoveHandle = Handler->OnMouseMoveDelegate.AddLambda([this](const FPointerEvent& PointerEvent)
        {
            sol::protected_function_result Result = EventFunc.OnMouseMove(
                PointerEvent.GetScreenSpacePosition().X, PointerEvent.GetScreenSpacePosition().Y);
            LogIfErrorExist("OnMouseMove", Result);
        });
        DelegateHandlers[HandlerType::MouseMove].Add(MouseMoveHandle);
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

    for (const auto [type, handles]: DelegateHandlers)
    {
        for (auto handler : handles)
        {
            switch (type)
            {
                case HandlerType::KeyDown:
                    Handler->OnKeyDownDelegate.Remove(handler);
                    break;
                case HandlerType::KeyUp:
                    Handler->OnKeyUpDelegate.Remove(handler);
                    break;
                case HandlerType::MouseDown:
                    Handler->OnMouseDownDelegate.Remove(handler);
                    break;
                case HandlerType::MouseMove:
                    Handler->OnMouseMoveDelegate.Remove(handler);
                    break;
            }
        }
    }

    if (USphereComponent* SphereComp = GetOwner()->GetComponentByClass<USphereComponent>())
    {
       SphereComp->OnComponentBeginOverlap.Clear();
    }
    DelegateHandlers.Empty();

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
        InitEnvironment();
    Environment["obj"] = GetOwner();
    
    if (ScriptName == "Saved/LuaScripts/Player.lua") 
    {
        for (const auto iter : TObjectRange<APlayerCameraManager>()) 
        {
            if (iter->GetWorld() != GEngine->ActiveWorld) 
            {
                continue;
            }

            for (const auto modi : iter->ModifierList) {

                if (USpringArmCameraModifier* springArm = Cast<USpringArmCameraModifier>(modi)) 
                {
                    springArm->SetFollowTargetActor(GetOwner());
                }
            }

            
        }
    }

    std::string scriptText = FEngineLoop::ScriptSys.LoadScripts[ScriptName];
    sol::load_result loadresult = lua.load_buffer(scriptText.c_str(), scriptText.length());
    sol::function script;
    if (loadresult.valid())
        script = loadresult;
    else
    {
        sol::error err = loadresult;
        UE_LOG(LogLevel::Error, "Failed to compile %s@%s", GetData(ScriptName), err.what());
    }
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

void UScriptableComponent::InitEnvironment()
{
    sol::state& lua = FEngineLoop::ScriptSys.Lua();
    Environment = sol::environment(lua, sol::create, FEngineLoop::ScriptSys.GetSharedEnviroment());
}

void UScriptableComponent::HandleSphereOverlap(USphereComponent* OverlappedComponent, USphereComponent* OtherComponent)
{
    if (EventFunc.OnOverlap.valid())
    {
        sol::protected_function_result Result = EventFunc.OnOverlap(OverlappedComponent, OtherComponent);
        LogIfErrorExist("OnOverlap", Result);
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
            UE_LOG(LogLevel::Error, "Failed to call %s@%s with non-string error", GetData(funcName), ScriptName);
        }
        
    }
}
