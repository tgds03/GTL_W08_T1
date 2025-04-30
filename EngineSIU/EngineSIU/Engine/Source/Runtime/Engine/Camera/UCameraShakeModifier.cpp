#include "UCameraShakeModifier.h"
#include <cstdlib>
#include <ctime>     
#include "Engine/PlayerCameraManager.h"
UCameraShakeModifier::UCameraShakeModifier()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    UCurveFloat* CurveX = new UCurveFloat();
    CurveX->AddKey(0.0f, 1.0f);   // 시작 시 최대 진폭
    CurveX->AddKey(0.3f, 0.7f);   // 빠르게 감쇠
    CurveX->AddKey(0.7f, 0.3f);   // 중간 감쇠
    CurveX->AddKey(1.0f, 0.0f);   // 종료 시 진폭 0

    // 커브 감쇠 적용 (X축만 적용)
    LocOsc.AmplitudeCurve[0] = CurveX;

    LocOsc.Amplitude = FVector(0.1f, 0.1f, 0.1f);
    LocOsc.Frequency = FVector(5.f, 5.f, 5.f);
    LocOsc.InitialPhase = FVector(
        FMath::FRandRange(0.f, 2.f * PI),
        FMath::FRandRange(0.f, 2.f * PI),
        FMath::FRandRange(0.f, 2.f * PI)
    );

    RotOsc.Amplitude = FVector(0.1f, 0.1f, 0.1f);
    RotOsc.Frequency = FVector(3.f, 3.f, 3.f);
    RotOsc.InitialPhase = FVector(
        FMath::FRandRange(0.f, 2.f * PI),
        FMath::FRandRange(0.f, 2.f * PI),
        FMath::FRandRange(0.f, 2.f * PI)
    );

    Duration = 10.f;   // 셰이크 총 길이(초)
    PlayScale = 1.f;

}
void UCameraShakeModifier::SetOn()
{
    Super::SetOn();
    ElapsedTime = 0.f;
}

void UCameraShakeModifier::Modify(float DeltaTime, FViewTarget& ViewTarget)
{
    if (GetDisabled()) return;

    ElapsedTime += DeltaTime;
    float Blend = GetBlendAmount();

    /*if (Duration > 0.f && ElapsedTime > Duration + BlendOutTime)
    {
        SetOff();
        return;
    }*/

    float t = ElapsedTime;
    FVector locOffset(
        LocOsc.GetOffset(0, t),
        LocOsc.GetOffset(1, t),
        LocOsc.GetOffset(2, t)
    );
    FVector rotOffset(
        RotOsc.GetOffset(0, t),
        RotOsc.GetOffset(1, t),
        RotOsc.GetOffset(2, t)
    );

    locOffset *= (Blend * PlayScale);
    rotOffset *= (Blend * PlayScale);

    ViewTarget.EyeLocation += locOffset;
    ViewTarget.EyeRotation += rotOffset;

    OwnerMgr->SetViewTargetEyeLocation(ViewTarget.EyeLocation);
    OwnerMgr->SetViewTargetEyeRotation(ViewTarget.EyeRotation);
}

float UCameraShakeModifier::GetBlendAmount() const
{
    if (Duration > 0.f && ElapsedTime >= Duration + BlendOutTime) {
        return 0.f;
    }

    float BlendIn = 1.f;
    if (BlendInTime > SMALL_NUMBER) {
        BlendIn = FMath::Clamp(ElapsedTime / BlendInTime, 0.f, 1.f);
    }

    float BlendOut = 1.f;
    if(BlendOutTime > SMALL_NUMBER && Duration > 0.f)
    {
        float TimeIntoFadeOut = ElapsedTime - (Duration - BlendOutTime);
        BlendOut = FMath::Clamp(1.f - (TimeIntoFadeOut / BlendOutTime), 0.f, 1.f);
    }

    return FMath::Min(BlendIn, BlendOut);
}
