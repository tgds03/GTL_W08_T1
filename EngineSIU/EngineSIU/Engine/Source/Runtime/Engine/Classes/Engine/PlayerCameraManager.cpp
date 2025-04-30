#include "PlayerCameraManager.h"
#include <Math/JungleMath.h>
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/UnrealEd/EditorViewportClient.h"

extern FEngineLoop GEngineLoop;

APlayerCameraManager::APlayerCameraManager()
{
    USceneComponent* Comp = AddComponent<USceneComponent>("USceneComponent_0");
    RootComponent = Comp;
}

void APlayerCameraManager::BeginPlay() {
    ViewTarget.EyeLocation = FVector(10.0, 0, 0);
    ViewTarget.EyeRotation = JungleMath::EulerToQuaternion(FVector(45, 0, 0));
}

void APlayerCameraManager::Tick(float DeltaTime) {
    const float MoveSpeed = 1.0f;
    ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed*DeltaTime;
    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->PerspectiveCamera.SetLocation(ViewTarget.EyeLocation);
}
