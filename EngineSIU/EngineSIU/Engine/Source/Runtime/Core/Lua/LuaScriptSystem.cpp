#include "LuaScriptSystem.h"
#include <Math/Vector.h>
//#include "Runtime/Launch/Launch.cpp"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
//#include "Runtime/Engine/Classes/Engine/Engine.cpp"

#include "Engine/EditorEngine.h"
#include "Runtime/Engine/World/World.h"
#include <InputCore/InputSystem.h>
#include <Components/ScriptableComponent.h>

#include "Components/Light/LightComponent.h"
#include "Components/Light/PointLightComponent.h"

extern FEngineLoop GEngineLoop;
#include <fstream>
#include "Components/ScriptableComponent.h"
#include "UObject/UObjectIterator.h"

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
    lua["USERTYPES"] = lua.create_table();
    SharedEnviroment = sol::environment(lua, sol::create, lua.globals());

    lua.set_exception_handler(&LuaExceptionHandler);
}

void ScriptSystem::BindTypes()
{
    BindPrimitiveTypes();
    BindUObject();
    BindInputSystem();
    BindEKeys();

    // 스폰 함수 바인딩(예: 문자열로 클래스 지정)
    lua.set_function("SpawnActor", [&](const std::string& className, sol::optional<std::string> luaActorName) -> AActor* 
    {
        UWorld* World = GEngine->ActiveWorld;
        // 문자열 FName으로 변환
        FString ClassName = className;
        FName fnClassName(ClassName);

        // UClass* 검색
        UClass* cls = UClass::FindClass(fnClassName);
        if (!cls) {
            // 존재하지 않는 클래스 이름인 경우
            return nullptr;
        }

        FName fnActorName = luaActorName ? FName{luaActorName->c_str()}
        : FName{}; // 기본 생성자는 none

        //액터 스폰
        return World->SpawnActor(cls, fnActorName);
    });

    lua.set_function("PrintLog", [](const std::string& str)
    {
        UE_LOG(LogLevel::Display, str.c_str());
    });

    lua.set_function("PrintObject", [&](const sol::object& obj)
    {
        UE_LOG(LogLevel::Display, luaToString(obj, 0, true).c_str());
    });

    lua.set_function("GetActorByName", [&](const std::string& str)
    {
        return GetActorByName(str);
    });
}

void ScriptSystem::BindPrimitiveTypes()
{
    using mFunc = sol::meta_function;
    
    // FVector
    sol::usertype<FVector> vectorTypeTable = lua.new_usertype<FVector>("FVector",
        sol::call_constructor,
        sol::constructors<FVector(), FVector(float, float, float)>()
    );

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

    // FRotator
    sol::usertype<FRotator> rotatorTypeTable = lua.new_usertype<FRotator>("FRotator",
        sol::call_constructor,
        sol::constructors<FRotator(), FRotator(float, float, float)>()
    );
    
    rotatorTypeTable["Pitch"] = &FRotator::Pitch;
    rotatorTypeTable["Yaw"] = &FRotator::Yaw;
    rotatorTypeTable["Roll"] = &FRotator::Roll;
    rotatorTypeTable["Clamp"] = &FRotator::Clamp;
    rotatorTypeTable["GetNormalized"] = &FRotator::GetNormalized;
    
    rotatorTypeTable[mFunc::addition] = [](const FRotator& a, const FRotator& b) { return a + b; };
    rotatorTypeTable[mFunc::subtraction] = [](const FRotator& a, const FRotator& b) { return a - b; };
    rotatorTypeTable[mFunc::multiplication] = [](const FRotator& v, const float f) { return v * f; };
    rotatorTypeTable[mFunc::equal_to] = [](const FRotator& a, const FRotator& b) { return a == b; };
    
    // FString
    sol::usertype<FString> stringTypeTable = lua.new_usertype<FString>("FString",
        sol::call_constructor,
        sol::constructors<FString(), FString(const std::string&), FString(const ANSICHAR*)>()
    );
    
    stringTypeTable["ToBool"] = &FString::ToBool;
    stringTypeTable["ToFloat"] = &FString::ToFloat;
    stringTypeTable["ToInt"] = &FString::ToInt;
    stringTypeTable["ToString"] = [](const FString& String) -> const char* { return GetData(String); };
    stringTypeTable["Len"] = &FString::Len;
    stringTypeTable["IsEmpty"] = &FString::IsEmpty;

    stringTypeTable[mFunc::addition] = [](const FString& a, const FString& b) { return a + b; };
    stringTypeTable[mFunc::equal_to] = [](const FString& a, const FString& b) { return a == b; };
}

void ScriptSystem::BindUObject()
{
    sol::usertype<UObject> UObjectTypeTable = lua.new_usertype<UObject>("UObject");
    UObjectTypeTable["GetUUID"] = &UObject::GetUUID;
    
    TMap<FName, UClass*> ClassMap = UClass::GetClassMap();
    for (auto [name, meta]: ClassMap)
    {
        meta->BindPropertiesToLua(lua);
    }
    
    // UFUNCTION으로 안되는 케이스들.
    sol::usertype<UActorComponent> UActorTypeTable = UActorComponent::GetLuaUserType(lua);
    UActorTypeTable["GetOwner"] = &UActorComponent::GetOwner;

    // GetComponentByClass
    sol::usertype<AActor> AActorTypeTable = AActor::GetLuaUserType(lua);
    AActorTypeTable["GetUActorComponent"] = &AActor::GetComponentByClass<UActorComponent>;
    AActorTypeTable["GetUSceneComponent"] = &AActor::GetComponentByClass<USceneComponent>;
    AActorTypeTable["GetUPrimitiveComponent"] = &AActor::GetComponentByClass<UPrimitiveComponent>;
    AActorTypeTable["GetULightComponentBase"] = &AActor::GetComponentByClass<ULightComponentBase>;
    AActorTypeTable["GetUPointLightComponent"] = &AActor::GetComponentByClass<UPointLightComponent>;
}

void ScriptSystem::InitPIEScript(TArray<AActor*> LevelActors)
{
    SharedEnviroment = sol::environment(lua, sol::create, lua.globals());
    for (AActor* Actor: LevelActors)
    {
        SharedEnviroment[GetData(Actor->GetOriginalActorLabel())] = Actor;
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
    sol::table keyNames = lua.create_named_table("EKeyNames");

#define ADD_KEY(name) \
        keys[#name] = EKeys::name; \
        keyNames[EKeys::name] = #name;

    ADD_KEY(Invalid);
    ADD_KEY(SpaceBar);
    ADD_KEY(W);
    ADD_KEY(A);
    ADD_KEY(S);
    ADD_KEY(D);
    ADD_KEY(LeftMouseButton);
    ADD_KEY(RightMouseButton);

#undef ADD_KEY

    lua["KeyCodeToString"] = keyNames;

    // NOTICE : 필요한 키 추가 시 확장
}

void ScriptSystem::LoadFile(const std::string& fileName)
{
    // lua.script_file(lua["SCRIPT_PATH"].get<std::string>() + fileName);
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        UE_LOG(LogLevel::Error, "Failed to open %s", fileName.c_str());
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    LoadScripts[fileName] = buffer.str();
    ScriptTimeStamps[fileName] = std::filesystem::last_write_time(fileName);
    
    sol::load_result res = lua.load_file(fileName);
    if (!res.valid())
    {
        sol::error err = res;
        UE_LOG(LogLevel::Error, "Failed to open %s", err.what());
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

            for (const auto iter : TObjectRange<UScriptableComponent>())
            {
                iter->LoadScriptAndBind();
            }
        }
    }
}

void ScriptSystem::ReloadForce()
{
    
    TArray<FString> paths;
    for (const auto& entry: std::filesystem::recursive_directory_iterator(GetData(ScriptPath)))
    {
        paths.Add(entry.path().string());
    }
    for (const auto& path: paths)
    {
        LoadFile(GetData(path));

        for (const auto iter : TObjectRange<UScriptableComponent>())
        {
            iter->InitEnvironment();
            iter->LoadScriptAndBind();
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

AActor* ScriptSystem::GetActorByName(const std::string& name) const
{
    TArray<AActor*> Actors = GEngine->ActiveWorld->GetActiveLevel()->Actors;
    for (AActor* Actor: Actors)
    {
        if (Actor->GetOriginalActorLabel() == FString(name))
        {
            return Actor;
        }
    }
    return nullptr;
}

std::string ScriptSystem::luaToString(const sol::object& obj, int depth = 0, bool showHidden = 0) {
    if (obj.get_type() == sol::type::nil) {
       return "nil";
    } else if (obj.is<std::string>()) {
        return "\"" + obj.as<std::string>() + "\"";
    } else if (obj.is<int>()) {
        return std::to_string(obj.as<int>());
    } else if (obj.is<double>()) {
        return std::to_string(obj.as<double>());
    } else if (obj.is<bool>()) {
        return obj.as<bool>() ? "true" : "false";
    } else if (obj.get_type() == sol::type::table) {
        std::string result = "{";
        sol::table tbl = obj;
        bool first = true;
        for (auto& kv : tbl) {
            if (!showHidden && kv.first.as<std::string>().starts_with("__"))
                continue;
            if (!first) result += ", ";
            first = false;
            result += luaToString(kv.first, depth + 1, showHidden) + " : " + luaToString(kv.second, depth + 1, showHidden);
        }
        result += "}";
        return result;
    } else  if (obj.get_type() == sol::type::userdata) {
        sol::table tbl = obj;
        sol::table metatable = tbl[sol::metatable_key];
        std::string type = metatable["__type"]["name"];
        if (obj.is<FVector>())
        {
            FVector vec = obj.as<FVector>();
            std::string result = "[FVector](";
            result += GetData(vec.ToString());
            result += ")";
            return result;
        }
        else if (obj.is<FString>())
        {
            std::string result = "[FString]\"";
            result += GetData(obj.as<FString>());
            result += "\"";
            return result;
        }
        else if (obj.is<FRotator>())
        {
            FRotator rot = obj.as<FRotator>();
            std::string result = "[FRotator](";
            result += GetData(rot.ToString());
            result += ")";
            return result;
        }
        else
        {
            std::string result = "[";
            result += metatable["__type"]["name"];
            result += "]:";
            result += luaToString(metatable, depth + 1, false);
            return result;
        }
    } else if (obj.get_type() == sol::type::function) {
        return "[function]";
    } else {
        return "[unknown type]";
    }
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
        std::string msg;
        msg.assign(desc.data(), desc.size());
        UE_LOG(LogLevel::Error, "Failed: %s", msg.c_str());
    }
    return sol::stack::push(L, desc);
}
