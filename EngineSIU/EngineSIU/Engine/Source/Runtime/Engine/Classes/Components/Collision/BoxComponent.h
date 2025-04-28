#pragma once
#include "Components/ShapeComponent.h"
#include "Delegates/Delegate.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentBeginOverlapSignature,
//    UBoxComponent*, OverlappedComponent, UBoxComponent*, OtherComponent);

class UBoxComponent : public UShapeComponent
{
    DECLARE_CLASS(UBoxComponent, UShapeComponent)
public:
    UBoxComponent();
    //~UBoxComponent();

    virtual UObject* Duplicate(UObject*)override;
    bool AreBoxesOverlapping(UBoxComponent* Other);

    FVector GetBoxExtent() const;

    //UPROPERTY(FComponentBeginOverlapSignature, OnComponentBeginOverlap)
private:
    FVector BoxExtent; // 각 길이의 반을 나타내는 벡터

};

