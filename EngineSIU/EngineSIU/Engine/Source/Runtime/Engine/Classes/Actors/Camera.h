#pragma once
#include "Engine/PlayerCameraManager.h"
#include "GameFramework/Actor.h"
#include "Engine/Camera/ViewTargetDefine.h"

class ACamera: public AActor
{
    DECLARE_CLASS(ACamera, AActor)
public:
    ACamera();

    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void Interpolate();
    UObject* Duplicate(UObject* InOuter) override;

    FEasingCurve TransitionCurve;
    UFUNCTION(void, SetTargetCamera, APlayerCameraManager* InCameraManager, float InTransitionDuration)
private:
    void UpdateViewportClient(float DeltaTime);
    
    FViewTarget ViewTarget;
    APlayerCameraManager* TargetCamera;
    APlayerCameraManager* OldCamera;
    float TransitionStartTime = 0.f;
    float TransitionTime = 5.f;
    float TransitionDuration = 5.f;
};
