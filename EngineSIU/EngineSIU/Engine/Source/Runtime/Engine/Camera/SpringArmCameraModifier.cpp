#include "SpringArmCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"
#include "Engine/PlayerCameraManager.h"
#include "Math/JungleMath.h"

USpringArmCameraModifier::USpringArmCameraModifier()
{
}

void USpringArmCameraModifier::Modify(float DeltaTime, FViewTarget& ViewTarget)
{
    // 기준 위치
    FVector Pivot = ViewTarget.EyeLocation + TargetOffset;

    // 카메라가 바라보는 방향
    // 기본 Forward 벡터 = X 축 기준 방향 (1, 0, 0)
    FVector ForwardBase = FVector(1, 0, 0);

    // 현재 시점(EyeRotation) 바탕으로 Forward vector 구함. 
    FVector ForwardDirection = JungleMath::FVectorRotate(ForwardBase, ViewTarget.EyeRotation);

    // 카메라 뒤로 밀기
    FVector CameraPosition = Pivot - ForwardDirection * ArmLength;
    
    // 위치 수정
    ViewTarget.EyeLocation = CameraPosition;
    
    
    //const float MoveSpeed = 1.0f;
    //const float RotationSpeed = 45.0f;

    //// FIXME : 구조체 참조 체크하기
    //ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;;
    //ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
    //OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeRotation);
    //OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);
}


