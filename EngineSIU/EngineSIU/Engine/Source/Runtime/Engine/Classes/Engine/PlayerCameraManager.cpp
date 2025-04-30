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
}

void APlayerCameraManager::Tick(float DeltaTime) {
    /* UCameraModifier 클래스 추가 시 수도 코드*/
    /*for (auto modifier : ModifierList) {
        modifier->Apply(&ViewTarget);
    }*/
    ApplyTest(DeltaTime);
    UpdateViewportTarget();
}

void APlayerCameraManager::UpdateViewportTarget()
{
    std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient();
    ViewportClient->PerspectiveCamera.SetLocation(ViewTarget.EyeLocation);
    ViewportClient->PerspectiveCamera.SetRotation(ViewTarget.EyeRotation);

}

void APlayerCameraManager::ApplyTest(float DeltaTime)
{
    const float MoveSpeed = 1.0f;
    const float RotationSpeed = 45.0f;

    ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;
    ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
}

// FIXME : UCameraModifier 클래스 추가 시 수도 코드
//void APlayerCameraManager::AddCameraModifier(UCameraModifier* NewModifier) {
//    ModifierList.Add(NewModifier);
//}
