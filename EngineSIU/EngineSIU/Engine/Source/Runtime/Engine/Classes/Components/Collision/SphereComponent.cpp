#include "SphereComponent.h"

USphereComponent::USphereComponent()
{
    SphereRadius = 50.0f;
}

FVector USphereComponent::GetSphereCenterLocationInWorld() const
{
    // 부모 클래스(USceneComponent)에 이미 구현된 함수를 호출합니다.
    return GetWorldLocation();
}

float USphereComponent::GetUnscaledSphereRadius() const
{
    return SphereRadius;
}

float USphereComponent::CalculateAbsMaxXYZ(const FVector& WorldScale3D) const
{
    // 스케일 벡터의 X, Y, Z 중 절대값이 가장 큰 값을 찾습니다. (안전한 방법)
    const float AbsScaleX = FMath::Abs(WorldScale3D.X);
    const float AbsScaleY = FMath::Abs(WorldScale3D.Y);
    const float AbsScaleZ = FMath::Abs(WorldScale3D.Z);

    const float MaxAbsScaleXYZ = FMath::Max3(AbsScaleX, AbsScaleY, AbsScaleZ);

    return MaxAbsScaleXYZ;
}

float USphereComponent::GetSphereScaledRadius() const
{
    const float BaseRadius = GetUnscaledSphereRadius();

    // 부모 클래스(USceneComponent)에 이미 구현된 함수를 호출하여 최종 월드 스케일을 얻습니다.
    const FVector WorldScale3D = GetWorldScale3D();    

    const float MaxScaleXYZ = CalculateAbsMaxXYZ(WorldScale3D);

    const float ScaledRadius = BaseRadius * MaxScaleXYZ;
    
    return ScaledRadius;
}

