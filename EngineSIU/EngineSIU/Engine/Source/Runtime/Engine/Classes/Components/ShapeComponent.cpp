#include "ShapeComponent.h"
#include "Components/Collision/SphereComponent.h"
#include "Components/Collision/BoxComponent.h"
#include "UObject/Casts.h"

bool UShapeComponent::CheckCollision(UShapeComponent* A, UShapeComponent* B)
{
    if (A == nullptr || B == nullptr)
    {
        return false;
    }
    if (A->ShapeType == EShapeType::Sphere && B->ShapeType == EShapeType::Sphere)
    {
        USphereComponent* SphereA = Cast<USphereComponent>(A);
        USphereComponent* SphereB = Cast<USphereComponent>(B);
        if (SphereA && SphereB)
        {
            return false;
            return SphereA->AreSpheresOverlapping(SphereB);
        }
    }
    else if (A->ShapeType == EShapeType::Box && B->ShapeType == EShapeType::Box)
    {
        UBoxComponent* BoxA = Cast<UBoxComponent>(A);
        UBoxComponent* BoxB = Cast<UBoxComponent>(B);
        if (BoxA && BoxB)
        {
            return BoxA->AreBoxesOverlapping(BoxB);
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
