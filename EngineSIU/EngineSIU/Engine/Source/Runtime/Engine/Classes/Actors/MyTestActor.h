#pragma once
#pragma once
#include "GameFramework/Actor.h" // AActor 헤더 포함
//#include "MyTestActor.generated.h" // (만약 UObject 시스템 사용 시 필요)

class AMyTestActor : public AActor // AActor를 상속받음
{
    DECLARE_CLASS(AMyTestActor, AActor) // (만약 UObject 시스템 사용 시 필요)

public:
    AMyTestActor(); // 생성자

protected:
    /** 게임 시작 시 호출됩니다. 부모 클래스의 함수를 오버라이드합니다. */
    virtual void BeginPlay() override; // <- ★★★ 오버라이드 선언 ★★★

    void Tick(float DeltaTime);

    // ... (다른 멤버 변수나 함수) ...
};

