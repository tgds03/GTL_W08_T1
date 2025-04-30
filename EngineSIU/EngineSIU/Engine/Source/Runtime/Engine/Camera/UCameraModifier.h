#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Camera/ViewTargetDefine.h"

class APlayerCameraManager;

class UCameraModifier: public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier();

    APlayerCameraManager* OwnerMgr;

    FEasingCurve BlendInCurve;
    FEasingCurve BlendOutCurve;
    UPROPERTY(float, BlendInTime)
    UPROPERTY(float, BlendOutTime)

    UFUNCTION(void, SetOn)
    UFUNCTION(void, SetOff)
    UFUNCTION(virtual void, Modify, float, FViewTarget&)
protected:
    UFUNCTION(virtual float, GetBlendAmount)
    uint8 Priority;
    float NowBlendAmount;
private:
    
    
    float GetInterpolated(float x);
    float StartTime;
    bool bDisabled;
};
