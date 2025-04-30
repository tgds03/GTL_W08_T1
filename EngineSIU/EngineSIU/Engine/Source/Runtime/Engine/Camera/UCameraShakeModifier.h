#pragma once
#include "Engine/Camera/UCameraModifier.h"
#include "Math/Rotator.h"

struct FOscillator {
    FVector Amplitude = FVector::Zero();
    FVector Frequency = FVector::Zero();
    FVector InitialPhase = FVector::Zero();

    // 시간 t에 대한 오실레이션 값 계산 ( 사인파 ) 
    float GetOffset(int Axis, float t) const {
        return Amplitude[Axis] *
            FMath::Sin(Frequency[Axis] * t + InitialPhase[Axis]);
    }
};

class UCameraShakeModifier : public UCameraModifier
{
    DECLARE_CLASS(UCameraShakeModifier, UCameraModifier)
public:
    UCameraShakeModifier();

    // 루트 패턴 설정
    FOscillator LocOsc; // 위치 진동
    FOscillator RotOsc;
    float Duration = 0.f;       // 0이하면 무한 재생
    float ElapsedTime = 0.f;
    float PlayScale = 1.f;      // 글로벌 강도 배수

    virtual void SetOn() override;
    virtual void Modify(float DeltaTime, FViewTarget& ViewTarget) override;

    float GetBlendAmount() const;

};
