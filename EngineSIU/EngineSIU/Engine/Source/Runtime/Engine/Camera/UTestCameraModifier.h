#pragma once
#include "Engine/Camera/UTestCameraModifier.h"
#include "Engine/Camera/UCameraModifier.h"
class UTestCameraModifier : public UCameraModifier 
{
    DECLARE_CLASS(UTestCameraModifier, UCameraModifier)
public:
    UTestCameraModifier();

    virtual void Modify(float DeltaTime, FViewTarget& ViewTarget) override;


};
