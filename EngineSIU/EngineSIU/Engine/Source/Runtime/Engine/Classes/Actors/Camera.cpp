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
}

void ACamera::SetTargetCamera(APlayerCameraManager* InCameraManager, float InTransitionDuration)
{
    OldCamera = TargetCamera;
    TargetCamera = InCameraManager;
    TransitionDuration = 0.f;
    TransitionDuration = InTransitionDuration;
}

void ACamera::Interpolate()
{
    const float t = TransitionCurve.GetYFromX(TransitionTime / TransitionDuration);
    FVector OldLoc = OldCamera->GetViewTarget().EyeLocation;
    FVector OldRot = OldCamera->GetViewTarget().EyeRotation;
    FVector NewLoc = TargetCamera->GetViewTarget().EyeLocation;
    FVector NewRot = TargetCamera->GetViewTarget().EyeRotation;
    ViewTarget.EyeLocation = FMath::Lerp(OldLoc, NewLoc, t);
    ViewTarget.EyeRotation = FMath::Lerp(OldRot, NewRot, t);
}
