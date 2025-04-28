#include "Cube.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/FLoaderOBJ.h"

#include "GameFramework/Actor.h"

#include "Components/Collision/SphereComponent.h"

    

ACube::ACube()
{
    // 임시 주석 처리
    //USphereComponent* sphere = AddComponent<USphereComponent>();
    //sphere->AttachToComponent(RootComponent);
    // --- ★★★ 생성자 로직 수정 ★★★ ---
    UE_LOG(LogLevel::Display, TEXT("ACube '%s': Constructor started."), *GetName());

    // 1. AddComponent 호출 (결과를 UActorComponent* 로 받음)
    UActorComponent* AddedComp = AddComponent<USphereComponent>(FName("MySimpleSphere"));

    // 2. 반환된 포인터가 유효한지 확인
    if (AddedComp != nullptr)
    {
        // 3. USphereComponent* 로 캐스팅 시도 (중요!)
        USphereComponent* CreatedSphere = dynamic_cast<USphereComponent*>(AddedComp); // 또는 Cast<USphereComponent>(AddedComp)

        // 4. 캐스팅 성공 여부 확인
        if (CreatedSphere != nullptr)
        {
            UE_LOG(LogLevel::Display, TEXT("ACube '%s': Successfully created and casted SphereComponent."), *GetName());

            // 5. 루트 컴포넌트에 붙이기 (이제 CreatedSphere는 USceneComponent의 자식이므로 가능!)
            USceneComponent* CurrentRoot = GetRootComponent();
            if (CurrentRoot)
            {
                // ★★★ SetupAttachment 호출 가능! ★★★
                CreatedSphere->SetupAttachment(CurrentRoot);
                CreatedSphere->SetSphereRadius(GetActorScale()); // 반지름 설정
                UE_LOG(LogLevel::Display, TEXT("ACube '%s': SphereComponent attached to Root."), *GetName());
            }
            else
            {
                // ★★★ SetRootComponent 호출 가능! ★★★
                SetRootComponent(CreatedSphere);
                CreatedSphere->SetSphereRadius(GetActorScale());
                UE_LOG(LogLevel::Display, TEXT("ACube '%s': SphereComponent set as Root."), *GetName());
            }
        }
        else
        {
            // 캐스팅 실패 로그
            FString AddedCompClassName = AddedComp->GetClass() ? AddedComp->GetClass()->GetName() : TEXT("UnknownClass");
            UE_LOG(LogLevel::Error, TEXT("ACube '%s': AddComponent succeeded but Cast/dynamic_cast to USphereComponent FAILED! Added component was type: %s"), *GetName(), *AddedCompClassName);
        }
    }
    else
    {
        // AddComponent 실패 로그
        UE_LOG(LogLevel::Error, TEXT("ACube '%s': AddComponent<USphereComponent> returned NULL!"), *GetName());
    }

    // --- StaticMesh 설정 ---
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    }

    UE_LOG(LogLevel::Display, TEXT("ACube '%s': Constructor finished."), *GetName());
}

void ACube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    USphereComponent* MySphere = GetComponentByClass<USphereComponent>();
    // CollisionSphere가 유효한지 확인하고 수동 충돌 검사 호출
    if (MySphere != nullptr)
    {
        MySphere->ManualTickCollisionCheck(); // 여기서 스피어 컴포넌트 검사 로직 실행
    }

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
