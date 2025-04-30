#include "Camera.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/UnrealEd/EditorViewportClient.h"
extern FEngineLoop GEngineLoop;

ACamera::ACamera() {
    TransitionCurve.Point1 = FVector2D::ZeroVector;
    TransitionCurve.Point2 = FVector2D::OneVector;
    TransitionCurve.Handle1 = TransitionCurve.Point1;
    TransitionCurve.Handle2 = TransitionCurve.Point2;

    RootComponent = AddComponent<USceneComponent>("USceneComponent");
    OldCamera = nullptr;
    TargetCamera = nullptr;
}

void ACamera::BeginPlay()
{
    AActor::BeginPlay();
}

void ACamera::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
    
    UpdateViewportClient(DeltaTime);
}

void ACamera::UpdateViewportClient(float DeltaTime)
{
    if (TransitionTime < TransitionDuration)
    {
        TransitionTime = std::min(TransitionTime + DeltaTime, TransitionDuration);
        Interpolate();
    }
    std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient();
    ViewportClient->PerspectiveCamera.SetLocation(ViewTarget.EyeLocation);
    ViewportClient->PerspectiveCamera.SetRotation(ViewTarget.EyeRotation);
    SetActorLocation(ViewTarget.EyeLocation);
    SetActorRotation(ViewTarget.EyeRotation);
}

void ACamera::SetTargetCamera(APlayerCameraManager* InCameraManager, float InTransitionDuration)
{
    if (InCameraManager == nullptr)
    {
        UE_LOG(LogLevel::Error, "SetTargetCamera: InCameraManager is null");
        return;
    }
    if (InCameraManager == TargetCamera)
        return;
    OldCamera = TargetCamera;
    TargetCamera = InCameraManager;
    TransitionTime = 0.f;
    TransitionDuration = InTransitionDuration;
}

void ACamera::Interpolate()
{
    if (OldCamera != nullptr)
    {
        const float t = TransitionCurve.GetYFromX(TransitionTime / TransitionDuration);
        FVector OldLoc = OldCamera->GetViewTarget().EyeLocation;
        FVector OldRot = OldCamera->GetViewTarget().EyeRotation;
        FVector NewLoc = TargetCamera->GetViewTarget().EyeLocation;
        FVector NewRot = TargetCamera->GetViewTarget().EyeRotation;
        ViewTarget.EyeLocation = FMath::Lerp(OldLoc, NewLoc, t);
        ViewTarget.EyeRotation = FMath::Lerp(OldRot, NewRot, t);
    } else
    {
        FVector NewLoc = TargetCamera->GetViewTarget().EyeLocation;
        FVector NewRot = TargetCamera->GetViewTarget().EyeRotation;
        ViewTarget.EyeLocation = NewLoc;
        ViewTarget.EyeRotation = NewRot;
    }


    UE_LOG(LogLevel::Display, "TargetCamera: %s", GetData(TargetCamera->GetActorLabel()));
    UE_LOG(LogLevel::Display, "CamPos: %s", GetData(ViewTarget.EyeLocation.ToString()));
    UE_LOG(LogLevel::Display, "CamRot: %s", GetData(ViewTarget.EyeRotation.ToString()));
}
