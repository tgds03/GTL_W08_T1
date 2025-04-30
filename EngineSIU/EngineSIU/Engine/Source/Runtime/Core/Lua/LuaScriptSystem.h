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
    void BindInputSystem();
    void BindEKeys();
    void LoadFile(const std::string& fileName);
    bool IsOutdated(const std::string& fileName);
    sol::environment SharedEnviroment;
    const FString ScriptPath = "Saved/LuaScripts/";
    AActor* GetActorByName(const std::string& name) const;
    static std::string luaToString(const sol::object& obj, int depth, bool showHidden);

    template <typename T>
    sol::table ArrayToTable(TArray<T> arr);
    template <typename T>
    sol::table ArrayToTable(std::vector<T> arr);
};

int LuaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view desc);

template <typename T>
sol::table ScriptSystem::ArrayToTable(TArray<T> arr)
{
    sol::table table = lua.create_table();
    for (int i = 1; i <= arr.Num(); ++i)
    {
        table[i] = arr[i-1];
    }
    return table;
}

template <typename T>
sol::table ScriptSystem::ArrayToTable(std::vector<T> arr)
{
    sol::table table = lua.create_table();
    for (int i = 1; i <= arr.size(); ++i)
    {
        table[i] = arr[i-1];
    }
    return table;
}
