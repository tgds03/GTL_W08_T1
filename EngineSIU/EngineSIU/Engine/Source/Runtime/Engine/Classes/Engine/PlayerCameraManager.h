#pragma once
#include "Engine/Classes/GameFramework/Actor.h"
#include "Core/Math/Quat.h"
struct FViewTarget {
   
    // FIXME : FViewTarget or SpringArm 둘 중
    // 따라갈 액터 
   // AActor* TargetActor = nullptr;
    // 카메라 위치
    FVector EyeLocation = FVector::Zero();
    // 카메라 회전(쿼터니언)
    FVector EyeRotation = FVector::Zero();
};

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();

    // FIXME : UCameraModifier 클래스 추가 시 수도 코드
    //void AddCameraModifier(UCameraModifier* NewModifier);
private:
    FLinearColor FadeColor;
    float FadeAmount;
    FVector2D FadeAlpha;
    float FadeTime;
    float FadeTimeRemaining;

    FName CameraStyle;
    struct FViewTarget ViewTarget;

    //TArray<UCameraModifier*> ModifierList;
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void UpdateViewportTarget();

    // location, rotation 테스트용 임시 함수
    void ApplyTest(float DeltaTime);

    

};
