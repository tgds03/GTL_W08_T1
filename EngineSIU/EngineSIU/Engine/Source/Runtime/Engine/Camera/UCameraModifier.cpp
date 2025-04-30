#include "UCameraModifier.h"
#include "Engine/PlayerCameraManager.h"

UCameraModifier::UCameraModifier()
{
    BlendInCurve.Point1 = FVector2D::ZeroVector;
    BlendInCurve.Point2 = FVector2D::OneVector;
    BlendInCurve.Handle1 = BlendInCurve.Point1;
    BlendInCurve.Handle2 = BlendInCurve.Point2;
    BlendOutCurve.Point1 = FVector2D(0.f, 1.f);
    BlendOutCurve.Point2 = FVector2D(1.f, 0.f);
    BlendOutCurve.Handle1 = BlendOutCurve.Point1;
    BlendOutCurve.Handle2 = BlendOutCurve.Point2;
    bDisabled = false;
    BlendOutTime = 20.f;
}

void UCameraModifier::SetOn()
{
    bDisabled = false;
    StartTime = 0;
    // TODO: How get now time??
    // StartTime =
}

void UCameraModifier::SetOff()
{
    bDisabled = true;
}


float UCameraModifier::GetDisabled()
{
    return bDisabled;
}

float UCameraModifier::GetInterpolated(float x)
{
    // TODO: How get now time??
    float nowTime = 0.f;
    float calcX = (nowTime - StartTime) / (!bDisabled ? BlendInTime : BlendOutTime);
    if (!bDisabled)
        return BlendInCurve.GetYFromX(calcX);
    else
        return BlendOutCurve.GetYFromX(calcX);
}

void UCameraModifier::Modify(float DeltaTime, FViewTarget& ViewTarget) {

}
