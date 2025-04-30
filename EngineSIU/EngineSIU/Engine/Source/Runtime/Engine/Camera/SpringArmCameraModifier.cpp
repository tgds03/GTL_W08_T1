#include "SpringArmCameraModifier.h"
#include "Engine/Camera/ViewTargetDefine.h"
#include "Engine/PlayerCameraManager.h"
#include "Math/JungleMath.h"
#include "Classes/Engine/Engine.h"
#include "Engine/World/World.h"
#include "Classes/GameFramework/Actor.h"
#include <Components/SphereComp.h>
#include <Engine/FLoaderOBJ.h>

USpringArmCameraModifier::USpringArmCameraModifier()
{
    //FakePlayerTarget = FVector(0, 0, 0);
    /*FollowTarget = GEngine->ActiveWorld->SpawnActor<AActor>();
    FollowTarget->SetActorLabel(TEXT("TargetActor"));
    USphereComp* SphereComp = FollowTarget->AddComponent<USphereComp>();
    SphereComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Sphere.obj"));
    FollowTarget->SetRootComponent(SphereComp);*/
    
}

void USpringArmCameraModifier::Modify(float DeltaTime, FViewTarget& ViewTarget)
{

    //if (!FollowTarget) return;

    //FVector TargetLocation = FollowTarget->GetActorLocation() + TargetOffset;
    FVector TargetLocation = FVector(0, 0, 0);
    FVector Forward = FVector(1, 0, 0);
    //FVector Forward = FollowTarget->GetActorForwardVector();

    FVector DesiredCameraLocation = TargetLocation - Forward * TargetArmLength;

    FVector NewCameraLocation = DesiredCameraLocation;

    if (bEnableLag && DeltaTime > SMALL_NUMBER)
    {
        NewCameraLocation = VInterpTo(PreviousCameraLocation, DesiredCameraLocation, DeltaTime, CameraLagSpeed);
    }

    PreviousCameraLocation = NewCameraLocation;
    ViewTarget.EyeLocation = NewCameraLocation;
    ViewTarget.EyeRotation = FVector(0, 0, 0); 
    //ViewTarget.EyeRotation = FollowTarget->GetActorRotation().ToVector(); // 혹은 View 로직에 맞게 처리

    OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeLocation);
    OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);

    //FVector TargetLocation = FollowTarget->GetActorLocation() + TargetOffset;
    //FVector DesiredCameraLocation = TargetLocation - FollowTarget->GetActorForwardVector() * TargetArmLength;
    //// TODO: 이 함수는 현재 Fake 데이터를 사용하고 있음.
    //// TODO: 나중에 실제 카메라 계산을 위해서는 입력으로 들어온 ViewTarget의 Location과 Rotation을 사용해야 함.
    //
    //FVector FakePlayerLocation = FVector(0, 0, 0);
    //FVector FakePlayerRotation = FVector::ZeroVector;

    //
    //// 기준 위치
    //FVector Pivot = FakePlayerLocation + TargetOffset;
    //

    //// 카메라가 바라보는 방향
    //// 기본 Forward 벡터 = X 축 기준 방향 (1, 0, 0)
    //FVector ForwardBase = FVector(1, 0, 0);

    //// 현재 시점(EyeRotation) 바탕으로 Forward vector 구함. 
    //FVector ForwardDirection = JungleMath::FVectorRotate(ForwardBase, FakePlayerRotation);

    //// 카메라 뒤로 밀기
    //FVector CameraPosition = Pivot - ForwardDirection * TargetArmLength;
    //
    //FVector CameraRotation = FakePlayerRotation;

    //// 위치 수정
    //ViewTarget.EyeLocation = CameraPosition;
    //ViewTarget.EyeRotation = CameraRotation;
    //
    //OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeLocation);
    //OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);

    //const float MoveSpeed = 1.0f;
    //const float RotationSpeed = 45.0f;

    //// FIXME : 구조체 참조 체크하기
    //ViewTarget.EyeLocation.X = ViewTarget.EyeLocation.X + MoveSpeed * DeltaTime;;
    //ViewTarget.EyeRotation.Y = ViewTarget.EyeRotation.Y + RotationSpeed * DeltaTime;
    //OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeRotation);
    //OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);
}

FVector USpringArmCameraModifier::VInterpTo(const FVector& Current, const FVector& Target, float DeltaTime, float InterpSpeed)
{
    if (InterpSpeed <= 0.f || DeltaTime <= 0.f)
    {
        return Current;
    }

    FVector Delta = Target - Current;
    float DeltaLength = Delta.Length();

    // 너무 가까우면 그냥 Target 리턴
    if (DeltaLength < SMALL_NUMBER)
    {
        return Target;
    }

    // 이동할 거리 = 속도 * 시간
    const float MoveStep = DeltaLength * FMath::Clamp(InterpSpeed * DeltaTime, 0.f, 1.f);
    return Current + (Delta / DeltaLength) * MoveStep;
}
