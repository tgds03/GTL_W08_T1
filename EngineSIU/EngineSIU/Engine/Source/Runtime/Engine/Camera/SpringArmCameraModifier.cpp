#include "SpringArmCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"
#include "Engine/PlayerCameraManager.h"
#include "Math/JungleMath.h"

USpringArmCameraModifier::USpringArmCameraModifier()
{
    FakePlayerTarget = FVector(0, 0, 0);

}

void USpringArmCameraModifier::Modify(float DeltaTime, FViewTarget& ViewTarget)
{
    // TODO: 이 함수는 현재 Fake 데이터를 사용하고 있음.
    // TODO: 나중에 실제 카메라 계산을 위해서는 입력으로 들어온 ViewTarget의 Location과 Rotation을 사용해야 함.
    
    FVector FakePlayerLocation = FVector(0, 0, 0);
    FVector FakePlayerRotation = FVector::ZeroVector;

    
    // 기준 위치
    FVector Pivot = FakePlayerLocation + TargetOffset;
    

    // 카메라가 바라보는 방향
    // 기본 Forward 벡터 = X 축 기준 방향 (1, 0, 0)
    FVector ForwardBase = FVector(1, 0, 0);

    // 현재 시점(EyeRotation) 바탕으로 Forward vector 구함. 
    FVector ForwardDirection = JungleMath::FVectorRotate(ForwardBase, FakePlayerRotation);

    // 카메라 뒤로 밀기
    FVector CameraPosition = Pivot - ForwardDirection * ArmLength;
    
    FVector CameraRotation = FakePlayerRotation;

    // 위치 수정
    ViewTarget.EyeLocation = CameraPosition;
    ViewTarget.EyeRotation = CameraRotation;
    
    OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeLocation);
    OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);

    //const float MoveSpeed = 1.0f;
    //const float RotationSpeed = 45.0f;

    //// FIXME : 구조체 참조 체크하기
    //ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;;
    //ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
    //OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeRotation);
    //OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);
}


