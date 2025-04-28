#pragma once

#include "Classes/Engine/StaticMeshActor.h"


class USphereComponent;

class ACube : public AStaticMeshActor
{
    DECLARE_CLASS(ACube, AStaticMeshActor)

protected :
    //UPROPERTY() // 변수 선언 바로 위에 위치
    USphereComponent* CollisionSphere; // 이제 모든 ACube 객체는 자신의 CollisionSphere를 가리킬 포인터를 갖게 됩니다.


public:
    ACube();

    virtual void Tick(float DeltaTime) override;
};

