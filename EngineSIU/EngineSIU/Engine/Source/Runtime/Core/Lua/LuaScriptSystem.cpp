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
    lua["SCRIPT_PATH"] = "Saved/LuaScripts/";
}

void ScriptSystem::BindTypes()
{
    BindPrimitiveTypes();
    BindUObject();


    UWorld* World = GEngine->ActiveWorld;


    // AActor 클래스 노출
    lua.new_usertype<AActor>("Actor",
        "GetLocation", &AActor::GetActorLocation,
        "SetLocation", &AActor::SetActorLocation,
        "Tick", &AActor::Tick
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

void ScriptSystem::Tick(float dt)
{
    sol::function update = lua["Update"];
    if (update.valid()) {
        update(dt);
    }
}
