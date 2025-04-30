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
    FVector rot = ViewTarget.EyeRotation;
    rot.X = FMath::RadiansToDegrees(rot.X);
    rot.Y = -FMath::RadiansToDegrees(rot.Y);    // why???
    rot.Z = FMath::RadiansToDegrees(rot.Z);
    ViewportClient->PerspectiveCamera.SetLocation(ViewTarget.EyeLocation);
    ViewportClient->PerspectiveCamera.SetRotation(rot);
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
        // float p[5] = {
        //     TransitionCurve.Point1.X,
        //     TransitionCurve.Point1.Y,
        //     TransitionCurve.Point2.X,
        //     TransitionCurve.Point2.Y,
        //     0.f
        // };
        // const float t = ImGui::BezierValue(TransitionTime / TransitionDuration, p);
        FVector OldLoc = OldCamera->GetViewTarget().EyeLocation;
        FVector OldRot = OldCamera->GetViewTarget().EyeRotation;
        FVector NewLoc = TargetCamera->GetViewTarget().EyeLocation;
        FVector NewRot = TargetCamera->GetViewTarget().EyeRotation;
        FVector loc = OldLoc * (1 - t) + NewLoc * t;
        FVector rot = OldRot * (1 - t) + NewRot * t;
        ViewTarget.EyeLocation = loc;
        ViewTarget.EyeRotation = rot;
        UE_LOG(LogLevel::Display, "t: %f", t);       
    } else
    {
        FVector NewLoc = TargetCamera->GetViewTarget().EyeLocation;
        FVector NewRot = TargetCamera->GetViewTarget().EyeRotation;
        ViewTarget.EyeLocation = NewLoc;
        ViewTarget.EyeRotation = NewRot;
    }
}

UObject* ACamera::Duplicate(UObject* InOuter)
{
    ThisClass* NewActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewActor->TransitionCurve = TransitionCurve;
    NewActor->TransitionStartTime = TransitionStartTime;
    NewActor->TransitionTime = TransitionTime;
    NewActor->TransitionDuration = TransitionDuration;
    return NewActor;
}
