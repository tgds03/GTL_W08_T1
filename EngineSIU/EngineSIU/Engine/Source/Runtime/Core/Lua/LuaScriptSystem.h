#pragma once

#include <sol/sol.hpp>

class ScriptSystem {
public:
    void Initialize();
    void BindTypes();
    void Tick(float dt);
    sol::state& Lua() { return lua; }
private:
    sol::state lua;
    void BindPrimitiveTypes();
    void BindUObject();
};
