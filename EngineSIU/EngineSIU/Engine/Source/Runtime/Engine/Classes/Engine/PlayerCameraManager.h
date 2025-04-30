#pragma once
#include "Engine/Classes/GameFramework/Actor.h"
#include "Core/Math/Quat.h"
#include "Camera/UCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();

    void SetViewTargetEyeLocation(FVector pos);
    void SetViewTargetEyeRotation(FVector rot);
    FViewTarget GetViewTarget() { return ViewTarget; };

    // FIXME : 템플릿 활용해서 수정하기
    UCameraModifier* AddTestCameraModifier();

    void AddCameraModifier(UCameraModifier* NewModifier);
private:
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

    // location, rotation 테스트용 임시 함수
    void ApplyTest(float DeltaTime);
    void UpdateViewportTarget(); 
};
