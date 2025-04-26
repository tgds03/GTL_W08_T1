#pragma once
#include "Components/ShapeComponent.h"

class USphereComponent : public UShapeComponent
{
    float SphereRadius;

public:
    USphereComponent();
    
    // Getter Function
    // 월드 공간에서의 구의 중심점 위치를 FVector 타입으로 돌려주는 함수
    FVector GetSphereCenterLocationInWorld() const;

    float GetUnscaledSphereRadius() const;

    // 스케일(크기 조절)이 적용된 최종 반지름을 float 타입으로 돌려주는 함수
    float GetSphereScaledRadius() const;
    
    // Utility Function
    float CalculateMaxAbsXYZ(const FVector& WorldScale3D) const;

    static bool AreSpheresOverlapping(const USphereComponent* SphereA, const USphereComponent* SphereB);
};

