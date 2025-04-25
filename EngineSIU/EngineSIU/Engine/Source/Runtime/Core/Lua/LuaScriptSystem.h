#pragma once

#include <sol/sol.hpp>

class AActor;
class ScriptSystem {
public:
    sol::state lua;
    void Initialize();
    void Tick(float dt);
    void DoFile(const std::string& filename);

private:
    
    
       
};
