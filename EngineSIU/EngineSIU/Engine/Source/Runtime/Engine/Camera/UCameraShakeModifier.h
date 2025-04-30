#pragma once
#include "Engine/Camera/UCameraModifier.h"
#include "Math/Rotator.h"
#include "Engine/Camera/UCurveFloat.h"

enum class EOscillationType {
    Sine,
    PerlinNoise
};

struct FOscillator {
    FVector Amplitude = FVector::Zero();
    FVector Frequency = FVector::Zero();
    FVector InitialPhase = FVector::Zero();
    FVector Seed = FVector::Zero();

    EOscillationType Type = EOscillationType::PerlinNoise;

    // 시간에 따른 진폭 조절을 위한 커브
    
    UCurveFloat* AmplitudeCurve[3] = { nullptr, nullptr, nullptr };
    float GetOffset(int Axis, float t) const {
        float timeScale = 1.f;

        // 커브가 존재하면 시간에 따른 진폭 스케일 조정
        if (AmplitudeCurve[Axis]) {
            timeScale = AmplitudeCurve[Axis]->GetFloatValue(t);
        }

        switch (Type) {
            case EOscillationType::Sine:
                return Amplitude[Axis] * FMath::Sin(Frequency[Axis] * t + InitialPhase[Axis]);
                break;
            case EOscillationType::PerlinNoise:
                return Amplitude[Axis] *
                    FMath::PerlinNoise1D(Frequency[Axis] * t + Seed[Axis]);
            default:
                return 0.f;
        }
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
