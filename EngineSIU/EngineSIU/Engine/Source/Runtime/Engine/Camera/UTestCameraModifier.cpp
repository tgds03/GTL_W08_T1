#include "UTestCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"
#include "Engine/PlayerCameraManager.h"
UTestCameraModifier::UTestCameraModifier()
{
    //Super();
}

void UTestCameraModifier::Modify(float DeltaTime, FViewTarget& ViewTarget)
{
    const float MoveSpeed = 1.0f;
    const float RotationSpeed = 45.0f;
   
    // FIXME : 구조체 참조 체크하기
    ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;;
    ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
    OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeRotation);
    OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);
}
