#pragma once
#include "LightActor.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/BillboardComponent.h"

class AAmbientLight : public ALight
{
    DECLARE_CLASS(AAmbientLight, ALight)
public:
    AAmbientLight();
    virtual ~AAmbientLight();

protected:
    UPROPERTY
    (UAmbientLightComponent*, AmbientLightComponent, = nullptr);

    UPROPERTY
    (UBillboardComponent*, BillboardComponent, = nullptr);
};
