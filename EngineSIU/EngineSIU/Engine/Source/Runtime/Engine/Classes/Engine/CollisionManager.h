#pragma once
#include "Container/Array.h"

class UShapeComponent;
class FCollisionManager
{
public:
    FCollisionManager();
    ~FCollisionManager();
    void Initialize();
    void Release();

    void Tick(float DeltaTime);

private:
    TArray<UShapeComponent*> ShapeComponents;

    void CheckCollision();

};

