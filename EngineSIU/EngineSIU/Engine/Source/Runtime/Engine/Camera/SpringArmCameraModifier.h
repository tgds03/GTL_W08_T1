#pragma once
#include "Engine/Camera/UCameraModifier.h"

class USpringArmCameraModifier : public UCameraModifier
{
    DECLARE_CLASS(USpringArmCameraModifier, UCameraModifier)

public:
    USpringArmCameraModifier();

    // TargetOffset: 카메라 팔(Arm)이 시작될 위치 눈높이 조정
    FVector TargetOffset = FVector(0, 0, 10); 
    // ArmLength: 카메라를 기준점(Pivot)에서 얼마나 뒤로 뺄지
    float TargetArmLength = 10.f; 
    float CameraLagSpeed = 10.f;
    bool bEnableLag = true;

    FVector PreviousCameraLocation = FVector::Zero();

    virtual void Modify(float DeltaTime, FViewTarget& ViewTarget) override;
    
    AActor* FollowTarget = nullptr;

    void SetFollowTargetActor(AActor* InTarget);

    FVector VInterpTo(const FVector& Current, const FVector& Target, float DeltaTime, float InterpSpeed);

};
