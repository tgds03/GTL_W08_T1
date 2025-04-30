#include "PlayerCameraManager.h"
#include <Math/JungleMath.h>
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/UnrealEd/EditorViewportClient.h"
#include "Camera/SpringArmCameraModifier.h"
#include <Camera/UTestCameraModifier.h>
#include <Camera/UCameraShakeModifier.h>

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

    AddSpringArmCameraModifier();
    // FIXME : 테스트용 코드
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

UCameraModifier* APlayerCameraManager::AddSpringArmCameraModifier()
{
    //UCameraModifier* obj  = FObjectFactory::ConstructObject<UTestCameraModifier>(nullptr);
    UCameraModifier* obj  = FObjectFactory::ConstructObject<UCameraShakeModifier>(nullptr);

    //obj->OwnerMgr = this;
    //ModifierList.Add(obj);
    //return obj;

    // 만든 '스프링 암 카메라 필터' 도구를 만들어서 목록에 추가합니다.
    //    (FObjectFactory 대신 NewObject<> 사용하는 것이 더 최신 방식입니다)
    USpringArmCameraModifier* MySpringArmTool = FObjectFactory::ConstructObject<USpringArmCameraModifier>(nullptr); // this는 "이 카메라 감독님 소유로 만들어줘" 라는 뜻
    // 3. 혹시 도구가 잘 만들어졌는지 확인하고 (만약 null이 아니라면)
    if (MySpringArmTool)
    {
        MySpringArmTool->OwnerMgr = this; // 도구에게 "네 주인은 나야" 라고 알려주고
        ModifierList.Add(MySpringArmTool); // 도구 목록(ModifierList)에 진짜로 추가!
    }

    return MySpringArmTool; // 예를 들어 이렇게 생성된 객체를 반환
}

void APlayerCameraManager::BeginPlay() {
}

void APlayerCameraManager::Tick(float DeltaTime) {


    // Modifier 가 수정한 최종 viewtarget 값으로 화면 업데이트 
    /* UCameraModifier 클래스 추가 시 수도 코드*/
    for (auto modifier : ModifierList) {
        modifier->Modify(DeltaTime, ViewTarget);
    }
    UpdateViewportTarget();

    ApplySpringArmCamera(DeltaTime);
}

void APlayerCameraManager::UpdateViewportTarget()
{
    std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient();
    ViewportClient->PerspectiveCamera.SetLocation(ViewTarget.EyeLocation);
    ViewportClient->PerspectiveCamera.SetRotation(ViewTarget.EyeRotation);

}

void APlayerCameraManager::ApplySpringArmCamera(float DeltaTime)
{
    //const float MoveSpeed = 1.0f;
    //const float RotationSpeed = 45.0f;

    //ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;
    //ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
    
    // --- 1단계: 가짜 플레이어 정보 만들기 ---
    

    for (UCameraModifier* Modifier : ModifierList)
    {
        if (Modifier)
        {
            // 위에서 만든 가짜 ViewTarget 정보 전달
            Modifier->Modify(DeltaTime, ViewTarget);
        }
    }
}

// FIXME : UCameraModifier 클래스 추가 시 수도 코드
//void APlayerCameraManager::AddCameraModifier(UCameraModifier* NewModifier) {
//    ModifierList.Add(NewModifier);
//}
