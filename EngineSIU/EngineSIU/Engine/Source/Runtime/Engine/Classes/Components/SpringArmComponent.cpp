#include "SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World/World.h"

USpringArmComponent::USpringArmComponent()
{
    // 기본값 초기화
}

// 월드 가져오기 (엔진에 맞게 구현)
UWorld* USpringArmComponent::GetWorld() const
{
    AActor* Owner = GetOwner();
    return Owner ? Owner->GetWorld() : nullptr;
}

void USpringArmComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime); // 부모 클래스의 Tick 호출 (필요시)
}
