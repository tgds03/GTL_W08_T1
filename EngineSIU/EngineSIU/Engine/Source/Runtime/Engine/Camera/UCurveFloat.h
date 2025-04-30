#pragma once
#include "Core/Container/Array.h"

class UCurveFloat
{
public:
    struct FKey
    {
        float Time;
        float Value;
    };

    void AddKey(float InTime, float InValue);
    float GetFloatValue(float InTime) const;

private:
    TArray<FKey> Keys;
};
