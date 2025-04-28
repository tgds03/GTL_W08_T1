#include "Level.h"
#include "GameFramework/Actor.h"
#include "UObject/Casts.h"


void ULevel::InitLevel(UWorld* InOwningWorld)
{
    OwningWorld = InOwningWorld;

}

void ULevel::Release()
{
    // EndPlay에서 SpawnActor하는 이상한 상황 대응.
    // EndPlay와 RemoveObject를 분리.
    TArray<AActor*> actors = Actors;
    for (AActor* Actor : actors)
    {
        Actor->EndPlay(EEndPlayReason::WorldTransition);
    }
    
    for (AActor* Actor : Actors)
    {
        TSet<UActorComponent*> Components = Actor->GetComponents();
        for (UActorComponent* Component : Components)
        {
            GUObjectArray.MarkRemoveObject(Component);
        }
        GUObjectArray.MarkRemoveObject(Actor);
    }
    Actors.Empty();
}

UObject* ULevel::Duplicate(UObject* InOuter)
{
    ThisClass* NewLevel = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewLevel->OwningWorld = OwningWorld;

    for (AActor* Actor : Actors)
    {
        NewLevel->Actors.Emplace(static_cast<AActor*>(Actor->Duplicate(InOuter)));
    }

    return NewLevel;
}
