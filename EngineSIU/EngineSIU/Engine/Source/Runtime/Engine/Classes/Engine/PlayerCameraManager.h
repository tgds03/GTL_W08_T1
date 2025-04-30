#pragma once
#include "Engine/Classes/GameFramework/Actor.h"
#include "Core/Math/Quat.h"
struct FViewTarget {
    // 따라갈 액터
    AActor* TargetActor = nullptr;
    // 카메라 위치
    FVector EyeLocation = FVector::Zero();
    // 카메라 회전(쿼터니언)
    FQuat EyeRotation = FQuat::Identity();
};

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();
private:
    FLinearColor FadeColor;
    float FadeAmount;
    FVector2D FadeAlpha;
    float FadeTime;
    float FadeTimeRemaining;

    FName CameraStyle;
    struct FViewTarget ViewTarget;

    //TArray<UCameraModifier*> ModifierList;
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

};
