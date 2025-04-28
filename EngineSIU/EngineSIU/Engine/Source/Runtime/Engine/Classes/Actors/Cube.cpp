#include "Cube.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/FLoaderOBJ.h"

#include "GameFramework/Actor.h"

#include "Components/Collision/SphereComponent.h"

ACube::ACube(): CollisionSphere(nullptr)
{    
    // USphereComponent 생성 및 설정
    CollisionSphere = AddComponent<USphereComponent>(FName("CollisionSphere"));

    // 생성된 컴포넌트를 액터의 계층 구조에 붙임. 
    // StaticMeshComponent 가 Root 컴포넌트라고 가정
    if (CollisionSphere && GetRootComponent()) // 루트 컴포넌트가 있는지 확인
    {
        CollisionSphere->SetupAttachment(GetRootComponent());
        UE_LOG(LogLevel::Display, TEXT("ACube '%s' : CollisionSphere attached to Root"), *GetName());
    }

    else if (CollisionSphere)
    {
        // 혹시 루트 컴포넌트가 없다면 (그럴 가능성은 낮음), 스피어를 루트로 설정 시도
        SetRootComponent(CollisionSphere);
        UE_LOG(LogLevel::Display, TEXT("ACube '%s' : CollisionSphere set as Root"), *GetName());
    }

    else
    {
        UE_LOG(LogLevel::Error, TEXT("ACube '%s' : Failed to create CollisionSphere."), *GetName());
    }

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

    // CollisionSphere가 유효한지 확인하고 수동 충돌 검사 호출
    if (CollisionSphere)
    {
        CollisionSphere->ManualTickCollisionCheck(); // 여기서 스피어 컴포넌트 검사 로직 실행
    }

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
