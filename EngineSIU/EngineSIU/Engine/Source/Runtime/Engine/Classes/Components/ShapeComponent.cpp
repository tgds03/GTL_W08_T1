#include "ShapeComponent.h"
#include "Components/Collision/SphereComponent.h"
#include "UObject/Casts.h"

bool UShapeComponent::CheckCollision(const UShapeComponent* A, const UShapeComponent* B)
{
    if (A == nullptr || B == nullptr)
    {
        return false;
    }
    if (A->ShapeType == EShapeType::Sphere && B->ShapeType == EShapeType::Sphere)
    {
        const USphereComponent* SphereA = Cast<USphereComponent>(A);
        const USphereComponent* SphereB = Cast<USphereComponent>(B);
        if (SphereA && SphereB)
        {
            return USphereComponent::AreSpheresOverlapping(SphereA, SphereB);
        }
    }
    return false;
}

UObject* UShapeComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->ShapeColor = ShapeColor;
    NewComponent->bDrawOnlyIfSelected = bDrawOnlyIfSelected;


    return NewComponent;
}
