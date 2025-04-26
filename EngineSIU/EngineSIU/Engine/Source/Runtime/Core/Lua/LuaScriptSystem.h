#pragma once

#include <sol/sol.hpp>

class ScriptSystem {
public:
    void Initialize();
    void Bind();
    void Tick(float dt);
    sol::state& Lua() { return lua; }
private:
    sol::state lua;
    
       
};
