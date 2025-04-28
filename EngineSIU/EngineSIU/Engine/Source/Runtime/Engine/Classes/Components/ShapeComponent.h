#pragma once
#include "PrimitiveComponent.h"
enum class EShapeType
{
    Sphere,
    Box,
    Capsule,
};
class UShapeComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
public:
    UShapeComponent() = default;

    FColor ShapeColor;
    bool bDrawOnlyIfSelected;
    EShapeType ShapeType;
    bool bCollisionEnabled = true;

    static bool CheckCollision(const UShapeComponent* A, const UShapeComponent* B);
};

