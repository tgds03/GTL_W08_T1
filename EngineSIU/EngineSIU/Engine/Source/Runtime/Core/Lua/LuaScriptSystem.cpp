#include "LuaScriptSystem.h"
#include <Math/Vector.h>
//#include "Runtime/Launch/Launch.cpp"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
//#include "Runtime/Engine/Classes/Engine/Engine.cpp"

#include "Engine/EditorEngine.h"
#include "Runtime/Engine/World/World.h"

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

    lua.set_exception_handler(&LuaExceptionHandler);
}

void ScriptSystem::BindTypes()
{
    BindPrimitiveTypes();
    BindUObject();

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
    
    // sol::usertype<AActor> AActorTypeTable = AActor::GetLuaUserType(lua);
    // AActorTypeTable["GetLocation"] = &AActor::GetActorLocation;
    // AActorTypeTable["SetLocation"] = &AActor::SetActorLocation;
    // AActorTypeTable["GetRotation"] = &AActor::GetActorRotation;
    // AActorTypeTable["SetRotation"] = &AActor::SetActorRotation;
    // AActorTypeTable["GetScale"] = &AActor::GetActorScale;
    // AActorTypeTable["SetScale"] = &AActor::SetActorScale;
}

void ScriptSystem::LoadFile(const std::string& fileName)
{
    // lua.script_file(lua["SCRIPT_PATH"].get<std::string>() + fileName);
    sol::load_result res = lua.load_file(fileName);
    if (res.valid())
    {
        LoadScripts[fileName] = res;
        ScriptTimeStamps[fileName] = std::filesystem::last_write_time(fileName);
    } else
    {
        UE_LOG(LogLevel::Error, "Failed to open %s", fileName.c_str());
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

std::string ScriptSystem::luaToString(const sol::object& obj, int depth = 0, bool showHidden = 0) const {
    if (obj.is<std::string>()) {
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
    } else if (obj.get_type() == sol::type::function) {
        return "[function]";
    } else if (obj.get_type() == sol::type::nil) {
        return "nil";
    } else if (obj.get_type() == sol::type::userdata) {
        sol::table tbl = obj;
        sol::table metatable = tbl[sol::metatable_key];
        std::string result = "[";
        result += metatable["__type"]["name"];
        result += "]:";
        result += luaToString(metatable, depth + 1, false);
        return result;
    } else
    {
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
        UE_LOG(LogLevel::Error, "Failed: %s", desc.data());
    }
    return sol::stack::push(L, desc);
}
