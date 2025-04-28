#include "Cube.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/FLoaderOBJ.h"

#include "GameFramework/Actor.h"

#include "Components/Collision/SphereComponent.h"

ACube::ACube()
{    
    USphereComponent* sphere = AddComponent<USphereComponent>();
    sphere->AttachToComponent(RootComponent);
    sphere->SetSphereRadius(GetActorScale());

    // Begin Test
    //StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/helloBlender.obj"));
    // --- 기존 StaticMesh 설정 코드 ---
    // StaticMeshComponent는 AStaticMeshActor 생성자에서 이미 생성되었을 가능성이 높습니다.
    // 여기서 또 생성할 필요는 없습니다.
    if (StaticMeshComponent) // StaticMeshComponent 포인터가 유효한지 확인
    {
        // StaticMeshComponent->SetStaticMesh(...); // 기존 코드 유지
        StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    }
}

void ACube::Tick(float DeltaTime)
{
    UE_LOG(LogLevel::Warning, TEXT("ACube Tick: %s"), *GetName()); // 확인용 로그

    Super::Tick(DeltaTime);
    USphereComponent* sphere = GetComponentByClass<USphereComponent>();
    // CollisionSphere가 유효한지 확인하고 수동 충돌 검사 호출
    if (sphere)
    {
        //sphere->ManualTickCollisionCheck(); // 여기서 스피어 컴포넌트 검사 로직 실행
    }

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
