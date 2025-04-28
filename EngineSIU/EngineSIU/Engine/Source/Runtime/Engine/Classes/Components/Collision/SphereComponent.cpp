// --- 엔진/라이브러리 헤더 (Include 경로 기준으로 작성) ---

// ActorComponent.h ( 기준: ...\Engine\Classes\ -> Components/ActorComponent.h )
#include "Components/ActorComponent.h"

// World.h ( 기준: ...\Engine\ -> World/World.h - 실제 경로 확인 필요! )
// 예시 경로: Engine/World/World.h 또는 Public/World.h 등일 수 있음
#include "Engine/World/World.h" // <- World.h의 정확한 상대 경로 확인 후 수정!

// Object.h ( 기준: ...\CoreUObject\ -> UObject/Object.h )
#include "UObject/Object.h"

// String.h ( 기준: ...\Core\ -> Container/String.h )
#include "Container/String.h"

// Actor.h ( 기준: ...\Engine\Classes\ -> GameFramework/Actor.h )
#include "GameFramework/Actor.h"

// MathUtility.h ( 기준: ...\Core\ -> Math/MathUtility.h )
#include "Math/MathUtility.h" // <- FMath 들어있는 헤더 이름 확인!

// Vector.h ( 기준: ...\Core\ -> Math/Vector.h )
#include "Math/Vector.h"

// Define.h ( 기준: Engine\Source\ -> Define.h 또는 특정 폴더 아래?)
// Define.h의 정확한 위치에 따라 경로 수정 필요
#include "Define.h" // <- Define.h의 정확한 상대 경로 확인 후 수정!


// --- 자기 자신 헤더 ---
#include "SphereComponent.h" // SphereComponent.cpp와 같은 폴더에 있으므로 경로 없음

USphereComponent::USphereComponent()
{
    SphereRadius = 100.f;
}

UObject* USphereComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->SphereRadius = SphereRadius;

    return NewComponent;
}

FVector USphereComponent::GetSphereCenterLocationInWorld() const
{
    // 부모 클래스(USceneComponent)에 이미 구현된 함수를 호출합니다.
    //return GetOwner()->GetActorLocation();
    return GetWorldLocation();
}

float USphereComponent::GetUnscaledSphereRadius() const
{
    return SphereRadius;
}

float USphereComponent::CalculateMaxAbsXYZ(const FVector& WorldScale3D) const
{
    // 스케일 벡터의 X, Y, Z 중 절대값이 가장 큰 값을 찾습니다. (안전한 방법)
    const float AbsScaleX = FMath::Abs(WorldScale3D.X);
    const float AbsScaleY = FMath::Abs(WorldScale3D.Y);
    const float AbsScaleZ = FMath::Abs(WorldScale3D.Z);

    const float MaxAbsScaleXYZ = FMath::Max3(AbsScaleX, AbsScaleY, AbsScaleZ);

    return MaxAbsScaleXYZ;
}

float USphereComponent::GetSphereScaledRadius() const
{
    const float BaseRadius = GetUnscaledSphereRadius();

    // 부모 클래스(USceneComponent)에 이미 구현된 함수를 호출하여 최종 월드 스케일을 얻습니다.
    const FVector WorldScale3D = GetWorldScale3D();    

    const float MaxAbsScaleXYZ = CalculateMaxAbsXYZ(WorldScale3D);

    const float ScaledRadius = BaseRadius * MaxAbsScaleXYZ;
    
    return ScaledRadius;
}

void USphereComponent::SetSphereRadius(float NewRadius)
{
    SphereRadius = NewRadius;
}

void USphereComponent::SetSphereRadius(FVector InScale)
{
    SphereRadius = CalculateMaxAbsXYZ(InScale);
}

bool USphereComponent::AreSpheresOverlapping(const USphereComponent* SphereA, const USphereComponent* SphereB)
{
    if (SphereA == nullptr || SphereB == nullptr)
    {
        return false;
    }
    
    FVector CenterOfSphereA = SphereA->GetSphereCenterLocationInWorld();
    FVector CenterOfSphereB = SphereB->GetSphereCenterLocationInWorld();

    float RadiusOfSphereA = SphereA->GetSphereScaledRadius();
    float RadiusOfSphereB = SphereB->GetSphereScaledRadius();

    const float DeltaX = CenterOfSphereA.X - CenterOfSphereB.X;
    const float DeltaY = CenterOfSphereA.Y - CenterOfSphereB.Y;
    const float DeltaZ = CenterOfSphereA.Z - CenterOfSphereB.Z;

    const float DistanceSquared =
        FMath::Square(DeltaX) +
        FMath::Square(DeltaY) +
        FMath::Square(DeltaZ)
        ;

    const float RadiusSumAB = RadiusOfSphereA + RadiusOfSphereB;
    const float RadiusSumABSquared = FMath::Square(RadiusSumAB);

    return DistanceSquared <= RadiusSumABSquared;
}

//void USphereComponent::TickComponent(float DeltaTime)
//{
//    Super::TickComponent(DeltaTime);
//
//    UWorld* MyWorld = GetWorld();
//    if (!MyWorld) { return; }
//
//    ULevel* ActiveLevel = MyWorld->GetActiveLevel();
//    if (!ActiveLevel) { return; }
//
//    // 모든 USphereComponent 찾기. 
//    for (AActor* ActorInLevel : ActiveLevel->Actors)
//    {
//        if (!ActorInLevel) continue;
//
//        // 액터의 모든 컴포넌트 순회 (Actor 클래스에 OwnedComponents 같은 멤버가 있다고 가정)
//        const TSet<UActorComponent*>& Components = ActorInLevel->GetComponents(); // <- AActor의 실제 멤버 변수/함수로 수정!
//
//        for (UActorComponent* FoundComp : Components)
//        {
//            USphereComponent* OtherSphere = dynamic_cast<USphereComponent*>(FoundComp);
//
//            if (!OtherSphere || OtherSphere == this)
//            {
//                continue;
//            }
//
//            if (USphereComponent::AreSpheresOverlapping(this, OtherSphere))
//            {
//                // 겹치면 로그 출력
//                AActor* MyOwner = GetOwner();
//                AActor* OtherOwner = OtherSphere ? OtherSphere->GetOwner() : nullptr;
//                FString MyOwnerName = MyOwner ? MyOwner->GetName() : TEXT("Unknown");
//                FString OtherOwnerName = OtherOwner ? OtherOwner->GetName() : TEXT("Unknown");
//
//                UE_LOG(LogLevel::Warning, TEXT("[Overlap Tick] %s's Sphere overlaps with %s's Sphere!"), *MyOwnerName, *OtherOwnerName);
//            }
//        }
//    }
//}

void USphereComponent::ManualTickCollisionCheck()
{
    // UE_LOG(LogLevel::Warning, TEXT("ManualTickCollisionCheck called for: %s"), GetData(GetOwner()->GetName())); // ★★★ 추가 ★★★
    
    UWorld* MyWorld = GetWorld();
    if (!MyWorld) { return; }

    ULevel* ActiveLevel = MyWorld->GetActiveLevel();
    if (!ActiveLevel) { return; }

    // ★★★ 월드의 모든 액터 순회 ★★★
    for (AActor* ActorInLevel : ActiveLevel->Actors)
    {
        if (!ActorInLevel) continue;

        // ★★★ 액터의 컴포넌트 목록 가져오기 ★★★
        // const TSet<UActorComponent*>& Components = ActorInLevel->GetComponents();
        USphereComponent* OtherSphere = ActorInLevel->GetComponentByClass<USphereComponent>();
        // 유효하지 않거나 자기 자신이거나, 파괴 중(확인 기능 있다면)이면 건너뛰기
        if (!OtherSphere || OtherSphere == this /* || OtherSphere->IsBeingDestroyed() */)
        {
            continue;
        }

        // ★★★ Static 겹침 확인 함수 호출 ★★★
        if (USphereComponent::AreSpheresOverlapping(this, OtherSphere))
        {
            // 겹치면 로그 출력
            AActor* MyOwner = GetOwner();
            AActor* OtherOwner = OtherSphere->GetOwner();
            FString MyOwnerName = MyOwner ? MyOwner->GetName() : TEXT("Unknown");
            FString OtherOwnerName = OtherOwner ? OtherOwner->GetName() : TEXT("Unknown");

            // 로그 매크로 사용 (LogLevel 및 포맷팅 방식 확인)
            // UE_LOG(LogLevel::Warning, TEXT("[Manual Overlap Check] %s's Sphere overlaps with %s's Sphere!"), *MyOwnerName, *OtherOwnerName);

            // 델리게이트 호출
            OnComponentBeginOverlap.Broadcast(this, OtherSphere);
        }
    }
}
