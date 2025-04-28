#pragma once
#include "Components/ShapeComponent.h"
#include "Delegates/Delegate.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentBeginOverlapSignature, USphereComponent*, OverlappedComponent, USphereComponent*, OtherComponent);

class USphereComponent : public UShapeComponent
{
    DECLARE_CLASS(USphereComponent, UShapeComponent)
    float SphereRadius;

public:
    USphereComponent();

    UPROPERTY(FComponentBeginOverlapSignature, OnComponentBeginOverlap)
    FComponentBeginOverlapSignature OnComponentBeginOverlap;

    virtual UObject* Duplicate(UObject* InOuter) override;
    // Getter Function
    // 월드 공간에서의 구의 중심점 위치를 FVector 타입으로 돌려주는 함수
    FVector GetSphereCenterLocationInWorld() const;
    float GetUnscaledSphereRadius() const;
    // 스케일(크기 조절)이 적용된 최종 반지름을 float 타입으로 돌려주는 함수
    float GetSphereScaledRadius() const;
    
    void SetSphereRadius(float NewRadius); 
    void SetSphereRadius(FVector InScale);

    // Utility Function
    float CalculateMaxAbsXYZ(const FVector& WorldScale3D) const;

    static bool AreSpheresOverlapping(const USphereComponent* SphereA, const USphereComponent* SphereB);

    //virtual void TickComponent(float DeltaTime) override;

    void ManualTickCollisionCheck();
};

