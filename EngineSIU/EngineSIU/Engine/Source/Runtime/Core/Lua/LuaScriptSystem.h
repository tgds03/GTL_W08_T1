#pragma once

#include <filesystem>
#include <sol/sol.hpp>
#include "Container/Map.h"
#include "Container/Array.h"

class AActor;
class ScriptSystem {
public:
    void Initialize();
    void BindTypes();
    void Tick(float dt);
    void Reload();
    void ReloadForce();
    sol::state& Lua() { return lua; }
    TMap<FString, std::string> LoadScripts;
    TMap<FString, std::filesystem::file_time_type> ScriptTimeStamps;
    sol::environment GetSharedEnviroment() { return SharedEnviroment; }
    void InitPIEScript(TArray<AActor*> LevelActors);
private:
    sol::state lua;
    void BindPrimitiveTypes();
    void BindUObject();
    void LoadFile(const std::string& fileName);
    bool IsOutdated(const std::string& fileName);
    sol::environment SharedEnviroment;
    const FString ScriptPath = "Saved/LuaScripts/";
    static std::string luaToString(const sol::object& obj, int depth, bool showHidden);
    
};

int LuaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view desc);
