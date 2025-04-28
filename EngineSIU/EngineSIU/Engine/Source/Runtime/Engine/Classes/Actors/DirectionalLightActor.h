#pragma once
#include "LightActor.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/BillboardComponent.h"

class ADirectionalLight : public ALight
{
    DECLARE_CLASS(ADirectionalLight, ALight)
    
public:
    ADirectionalLight();
    virtual ~ADirectionalLight();

public:
    void SetIntensity(float Intensity);

protected:
    UPROPERTY
    (UDirectionalLightComponent*, DirectionalLightComponent, = nullptr);

    UPROPERTY
    (UBillboardComponent*, BillboardComponent, = nullptr);
};

