#pragma once
#include "PrimitiveComponent.h"
class UShapeComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
public:
    UShapeComponent() = default;
    virtual UObject* Duplicate(UObject* InOuter) override;

    FColor ShapeColor;
    bool bDrawOnlyIfSelected;
};

