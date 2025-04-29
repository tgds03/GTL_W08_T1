#include "CollisionManager.h"
#include "Components/ShapeComponent.h"
#include "UObject/UObjectIterator.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "World/World.h"
FCollisionManager::FCollisionManager()
{
}

FCollisionManager::~FCollisionManager()
{
}

void FCollisionManager::Initialize()
{
    ShapeComponents.Empty();
}

void FCollisionManager::Release()
{
    ShapeComponents.Empty();
}

void FCollisionManager::Tick(float DeltaTime)
{
    for (const auto iter : TObjectRange<UShapeComponent>())
    {
        if (GEngine->ActiveWorld == iter->GetWorld() && iter->bCollisionEnabled)
        {
            ShapeComponents.Add(iter);
        }
    }

    CheckCollision();


    ShapeComponents.Empty();
}

void FCollisionManager::CheckCollision()
{
    if (!ShapeComponents.IsEmpty())
    {
        for (uint32 i = 0; i < ShapeComponents.Num(); i++)
        {
            for (uint32 j = i + 1; j < ShapeComponents.Num(); j++)
            {
                if (UShapeComponent::CheckCollision(ShapeComponents[i], ShapeComponents[j]))
                {
                    // 겹치는 경우 처리
                    AActor* MyOwner = ShapeComponents[i]->GetOwner();
                    AActor* OtherOwner = ShapeComponents[j]->GetOwner();
                    FString MyOwnerName = MyOwner ? ShapeComponents[i]->GetName() : TEXT("Unknown");
                    FString OtherOwnerName = OtherOwner ? ShapeComponents[j]->GetName() : TEXT("Unknown");

                    // 로그 매크로 사용 (LogLevel 및 포맷팅 방식 확인)
                    // UE_LOG(LogLevel::Warning, TEXT("[Manual Overlap Check] %s's Sphere overlaps with %s's Sphere!"), *MyOwnerName, *OtherOwnerName);

                }
            }
        }
    }
}
