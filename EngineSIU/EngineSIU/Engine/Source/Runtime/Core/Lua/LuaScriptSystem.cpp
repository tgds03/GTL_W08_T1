#include "LuaScriptSystem.h"
#include <Math/Vector.h>
//#include "Runtime/Launch/Launch.cpp"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
//#include "Runtime/Engine/Classes/Engine/Engine.cpp"

#include "Engine/EditorEngine.h"
#include "Runtime/Engine/World/World.h"
#include <InputCore/InputSystem.h>

extern FEngineLoop GEngineLoop;

void ScriptSystem::Initialize()
{
    // LuaJIT 런타임 초기화
    lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string);

    // 스크립트 경로 지정
    lua["SCRIPT_PATH"] = ScriptPath;

    lua.set_exception_handler(&LuaExceptionHandler);

    // 입력 이벤트를 Lua에 연결
    auto* Handler = GEngineLoop.GetAppMessageHandler();

    Handler->OnKeyDownDelegate.AddLambda([](const FKeyEvent& KeyEvent)
    {
        sol::function onKeyDown = FEngineLoop::ScriptSys.Lua()["OnKeyDown"];
        if (onKeyDown.valid()) {
            onKeyDown(KeyEvent.GetKeyCode());
        }
    });

    Handler->OnKeyUpDelegate.AddLambda([](const FKeyEvent& KeyEvent)
    {
        sol::function onKeyUp = FEngineLoop::ScriptSys.Lua()["OnKeyUp"];
        if (onKeyUp.valid()) {
            onKeyUp(KeyEvent.GetKeyCode());
        }
    });

    Handler->OnMouseDownDelegate.AddLambda([](const FPointerEvent& MouseEvent)
    {
        sol::function onMouseDown = FEngineLoop::ScriptSys.Lua()["OnMouseDown"];
        if (onMouseDown.valid()) {
            onMouseDown(MouseEvent.GetEffectingButton());
        }
    });

    Handler->OnMouseUpDelegate.AddLambda([](const FPointerEvent& MouseEvent)
        {
            sol::function onMouseUp = FEngineLoop::ScriptSys.Lua()["OnMouseUp"];
            if (onMouseUp.valid())
            {
                onMouseUp(MouseEvent.GetEffectingButton());
            }
        });

    Handler->OnMouseMoveDelegate.AddLambda([](const FPointerEvent& MouseEvent)
    {
        sol::function onMouseMove = FEngineLoop::ScriptSys.Lua()["OnMouseMove"];
        if (onMouseMove.valid())
        {
            onMouseMove(MouseEvent.GetScreenSpacePosition().X, MouseEvent.GetScreenSpacePosition().Y);
        }
    });
}

void ScriptSystem::BindTypes()
{
    BindPrimitiveTypes();
    BindUObject();
    BindInputSystem();
    BindEKeys();


    UWorld* World = GEngine->ActiveWorld;


    // AActor 클래스 노출
    lua.new_usertype<AActor>("Actor",
        "GetLocation", &AActor::GetActorLocation,
        "SetLocation", &AActor::SetActorLocation,
        "ActorTick", &AActor::Tick
    );

    // 스폰 함수 바인딩(예: 문자열로 클래스 지정)
    lua.set_function("SpawnActor", [&](const std::string& className, sol::optional<std::string> luaActorName) -> AActor* 
    {
            // 문자열 FName으로 변환
            FName fnClassName{ className.c_str() };

            // UClass* 검색
            UClass* cls = UClass::FindClass(fnClassName);
            if (!cls) {
                // 존재하지 않는 클래스 이름인 경우
                return nullptr;
            }

            FName fnActorName = luaActorName ? FName{luaActorName->c_str()}
            : FName{}; // 기본 생성자는 none

        //액터 스폰
        return World->SpawnActor(cls);
        });

    lua.set_function("PrintLog", [](const std::string& str)
    {
        UE_LOG(LogLevel::Display, str.c_str());
    });
}

void ScriptSystem::BindPrimitiveTypes()
{
    using mFunc = sol::meta_function;
    
    // FVector
    sol::usertype<FVector> vectorTypeTable = lua.new_usertype<FVector>("FVector");
    vectorTypeTable[mFunc::construct] = sol::constructors<FVector(), FVector(float, float, float)>(); 
    vectorTypeTable["x"] = &FVector::X;
    vectorTypeTable["y"] = &FVector::Y;
    vectorTypeTable["z"] = &FVector::Z;
    vectorTypeTable["Dot"] = &FVector::Dot;
    vectorTypeTable["Cross"] = &FVector::Cross;
    vectorTypeTable["Length"] = &FVector::Length;
    vectorTypeTable["LengthSquared"] = &FVector::LengthSquared;
    vectorTypeTable["Distance"] = &FVector::Distance;
    vectorTypeTable["Normal"] = &FVector::GetSafeNormal;
    vectorTypeTable["Normalize"] = &FVector::Normalize;
    
    vectorTypeTable[mFunc::addition] = [](const FVector& a, const FVector& b) { return a + b; };
    vectorTypeTable[mFunc::subtraction] = [](const FVector& a, const FVector& b) { return a - b; };
    vectorTypeTable[mFunc::multiplication] = [](const FVector& v, const float f) { return v * f; };
    vectorTypeTable[mFunc::equal_to] = [](const FVector& a, const FVector& b) { return a == b; };
}

void ScriptSystem::BindUObject()
{
    TMap<FName, UClass*> ClassMap = UClass::GetClassMap();
    for (auto [name, meta]: ClassMap)
    {
        meta->BindPropertiesToLua(lua);
    }
}

void ScriptSystem::BindInputSystem()
{
    sol::usertype<FInputSystem> inputSysType = lua.new_usertype<FInputSystem>("InputSystem");
    inputSysType.set_function("IsKeyDown", &FInputSystem::isKeyDown);
    lua["Input"] = &GEngineLoop.InputSystem;
}

void ScriptSystem::BindEKeys()
{
    sol::table keys = lua.create_named_table("EKeys");

    keys["Invalid"] = EKeys::Invalid;
    keys["SpaceBar"] = EKeys::SpaceBar;
    keys["W"] = EKeys::W;
    keys["A"] = EKeys::A;
    keys["S"] = EKeys::S;
    keys["D"] = EKeys::D;
    keys["LeftMouseButton"] = EKeys::LeftMouseButton;
    keys["RightMouseButton"] = EKeys::RightMouseButton;

    // NOTICE : 필요한 키 추가 시 확장
}

void ScriptSystem::LoadFile(const std::string& fileName)
{
    // lua.script_file(lua["SCRIPT_PATH"].get<std::string>() + fileName);
    sol::load_result res = lua.load_file(fileName);
    if (res.valid())
    {
        LoadScripts[fileName] = res;
        ScriptTimeStamps[fileName] = std::filesystem::last_write_time(fileName);
    }
}

void ScriptSystem::Tick(float dt)
{
    sol::function update = lua["Update"];
    if (update.valid()) {
        update(dt);
    }
}

void ScriptSystem::Reload()
{
    TArray<FString> paths;
    for (const auto& entry: std::filesystem::recursive_directory_iterator(GetData(ScriptPath)))
    {
        paths.Add(entry.path().string());
    }
    for (const auto& path: paths)
    {
        if (IsOutdated(GetData(path)))
        {
            LoadFile(GetData(path));
        }
    }
}

bool ScriptSystem::IsOutdated(const std::string& fileName)
{
    if (!std::filesystem::exists(fileName)) { return false; }
    auto currentTime = std::filesystem::last_write_time(fileName);
    auto* FoundTime = ScriptTimeStamps.Find(fileName);
    // 만약 메인 파일의 저장된 타임스탬프가 없거나 현재와 다르면 변경된 것으로 처리
    if (!FoundTime || (*FoundTime != currentTime)) { return true; }
    return false;
}

// https://sol2.readthedocs.io/en/latest/exceptions.html
int LuaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view desc)
{
    if (exception)
    {
        UE_LOG(LogLevel::Error, "Failed: %s", exception->what());
    }
    else
    {
        UE_LOG(LogLevel::Error, "Failed: %s", desc.data());
    }
    return sol::stack::push(L, desc);
}
