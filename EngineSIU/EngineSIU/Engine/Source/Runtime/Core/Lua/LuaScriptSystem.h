#pragma once

#include <filesystem>
#include <sol/sol.hpp>
#include "Container/Map.h"

class ScriptSystem {
public:
    void Initialize();
    void BindTypes();
    void Tick(float dt);
    void Reload();
    sol::state& Lua() { return lua; }
    TMap<FString, std::string> LoadScripts;
    TMap<FString, std::filesystem::file_time_type> ScriptTimeStamps;
private:
    sol::state lua;
    
    void BindPrimitiveTypes();
    void BindUObject();
    void BindInputSystem();
    void BindEKeys();
    void LoadFile(const std::string& fileName);
    bool IsOutdated(const std::string& fileName);
    const FString ScriptPath = "Saved/LuaScripts/";
    static std::string luaToString(const sol::object& obj, int depth, bool showHidden);
};

int LuaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view desc);
