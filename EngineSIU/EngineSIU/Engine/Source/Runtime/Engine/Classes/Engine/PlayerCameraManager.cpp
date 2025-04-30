#include "PlayerCameraManager.h"
#include <Math/JungleMath.h>
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/UnrealEd/EditorViewportClient.h"
#include <Camera/UTestCameraModifier.h>

extern FEngineLoop GEngineLoop;

APlayerCameraManager::APlayerCameraManager()
{
    USceneComponent* Comp = AddComponent<USceneComponent>("USceneComponent_0");
    RootComponent = Comp;
    FadeColor = FLinearColor(0, 0, 0);
    FadeAmount = 0;
    FadeAlpha = FVector2D(0, 0);
    FadeTime = 0 ;
    FadeTimeRemaining = 0;

    AddTestCameraModifier();
}

void APlayerCameraManager::SetViewTargetEyeLocation(FVector pos)
{
    ViewTarget.EyeLocation = pos;
}

void APlayerCameraManager::SetViewTargetEyeRotation(FVector rot)
{
    ViewTarget.EyeRotation = rot;
}

UCameraModifier* APlayerCameraManager::AddTestCameraModifier()
{
    UCameraModifier* obj  = FObjectFactory::ConstructObject<UTestCameraModifier>(nullptr);

    obj->OwnerMgr = this;
    ModifierList.Add(obj);
    return obj;
}

void APlayerCameraManager::BeginPlay() {
}

void APlayerCameraManager::Tick(float DeltaTime) {
    for (auto modifier : ModifierList) {
        modifier->Modify(DeltaTime, ViewTarget);
    }
    // ApplyTest(DeltaTime);
}

void APlayerCameraManager::ApplyTest(float DeltaTime)
{
    const float MoveSpeed = 1.0f;
    const float RotationSpeed = 45.0f;

    ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;
    ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* NewModifier) {
    ModifierList.Add(NewModifier);
}
