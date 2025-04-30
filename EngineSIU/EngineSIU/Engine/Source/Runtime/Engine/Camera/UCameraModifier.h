#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UCameraModifier: public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier();

    FEasingCurve BlendInCurve;
    FEasingCurve BlendOutCurve;
    UPROPERTY(float, BlendInTime)
    UPROPERTY(float, BlendOutTime)

    UFUNCTION(void, SetOn)
    UFUNCTION(void, SetOff)
    UFUNCTION(virtual void, Modify)
protected:
    UFUNCTION(virtual float, GetBlendAmount)
    uint8 Priority;
    float NowBlendAmount;
private:
    float GetInterpolated(float x);
    float StartTime;
    bool bDisabled;
};
