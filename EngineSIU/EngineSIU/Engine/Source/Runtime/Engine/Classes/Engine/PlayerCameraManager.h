#pragma once
#include "Engine/Classes/GameFramework/Actor.h"
#include "Core/Math/Quat.h"
#include "Camera/UCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"
#include "Camera/SpringArmCameraModifier.h"

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();

    void SetViewTargetEyeLocation(FVector pos);
    void SetViewTargetEyeRotation(FVector rot);

    UCameraModifier* AddSpringArmCameraModifier();
    // FIXME : 템플릿 활용해서 수정하기
    UCameraModifier* AddCameraShakeModifier();

    // FIXME : UCameraModifier 클래스 추가 시 수도 코드
    //void AddCameraModifier(UCameraModifier* NewModifier);
public:
    FLinearColor FadeColor;
    float FadeAmount;
    FVector2D FadeAlpha;
    float FadeTime;
    float FadeTimeRemaining;

    FName CameraStyle;
    struct FViewTarget ViewTarget;

    TArray<UCameraModifier*> ModifierList;
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void UpdateViewportTarget();
};
