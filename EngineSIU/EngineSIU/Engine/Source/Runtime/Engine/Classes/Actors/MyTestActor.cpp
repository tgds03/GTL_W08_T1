
#include "MyTestActor.h"
#include "Components/Collision/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Define.h" // UE_LOG 등

AMyTestActor::AMyTestActor()
{
    // 필요한 기본 설정
}

void AMyTestActor::BeginPlay()
{
    // ★★★ 1. 부모(AActor)의 BeginPlay를 꼭 먼저 호출! ★★★
    AActor::BeginPlay();

    // ★★★ 2. 여기에 USphereComponent 추가하는 코드 삽입! ★★★
    if (!GetComponentByClass<USphereComponent>())
    {
        UE_LOG(LogLevel::Display, TEXT("Creating and attaching Test Sphere Component for %s"), *GetName());

        USphereComponent* TestSphere = AddComponent<USphereComponent>(FName("TestOverlapSphere"));

        if (TestSphere)
        {
            // TestSphere->RegisterComponent(); // <- 삭제! AddComponent가 처리할 것으로 예상
            UE_LOG(LogLevel::Warning, TEXT("BeginPlay: SphereComponent ADDED for %s"), *GetName()); // ★★★ 성공 로그 ★★★
            USceneComponent* CurrentRoot = GetRootComponent();
            if (CurrentRoot) {
                // SetupAttachment 함수가 USceneComponent에 있다고 가정
                TestSphere->SetupAttachment(CurrentRoot);
            }
            else {
                // SetRootComponent 함수가 AActor에 있다고 가정
                SetRootComponent(TestSphere);
            }
            // 필요시 겹침 설정 등 추가
            // TestSphere->bGenerateOverlapEvents = true; // 예시
            UE_LOG(LogLevel::Display, TEXT("Successfully attached Test Sphere Component to %s"), *GetName());
        }
    }
    // ★★★ ----------------------------------------- ★★★
}

void AMyTestActor::Tick(float DeltaTime)
{
    UE_LOG(LogLevel::Warning, TEXT("AMyTestActor Tick: %s"), *GetName()); // ★★★ 추가 ★★★
    Super::Tick(DeltaTime);

    // 내가 가진 SphereComp 찾기
    USphereComponent* MySphere = GetComponentByClass<USphereComponent>();

    // SphereComp 있다면 수동 충돌 검사 호출
    if (MySphere)
    {
        MySphere->ManualTickCollisionCheck();
    } else {
        UE_LOG(LogLevel::Warning, TEXT("Tick: %s has NO SphereComponent!"), *GetName()); // ★★★ 컴포넌트 없음 로그 ★★★
    }
}
